#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	OriginX = -SpaceWidth / 2.f;
	OriginY = -SpaceHeight / 2.f;
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	// TODO create the cells
	for (int row = 0; row < NrOfRows; ++row)
	{
		for (int col = 0; col < NrOfCols; ++col)
		{
			float left = OriginX + col * CellWidth;
			float bottom = OriginY + row * CellHeight;

			Cells.emplace_back(left, bottom, CellWidth, CellHeight);
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	// TODO Add the agent to the correct cell
	int index = PositionToIndex(Agent.GetPosition());
	Cells[index].Agents.push_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
	int oldIndex = PositionToIndex(OldPos);
	int newIndex = PositionToIndex(Agent.GetPosition());

	if (oldIndex != newIndex)
	{
		Cells[oldIndex].Agents.remove(&Agent);
		Cells[newIndex].Agents.push_back(&Agent);
	}
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	// TODO Register the neighbors for the provided agent
	NrOfNeighbors = 0;
	FVector2D pos = Agent.GetPosition();

	FRect radiusRect;
	radiusRect.Min = { pos.X - QueryRadius, pos.Y - QueryRadius };
	radiusRect.Max = { pos.X + QueryRadius, pos.Y + QueryRadius };
	
	for (Cell& cell : Cells)
	{
	// TODO Only check the cells that are within the radius of the neighborhood
		if (!DoRectsOverlap(cell.BoundingBox, radiusRect))
			continue;
		
		for (ASteeringAgent* pNeighbor : cell.Agents)
		{
			if (pNeighbor == &Agent)
				continue;

			float distSq = FVector2D::DistSquared(
				pos,
				pNeighbor->GetPosition()
			);

			if (distSq <= QueryRadius * QueryRadius)
			{
				Neighbors[NrOfNeighbors] = pNeighbor;
				++NrOfNeighbors;
			}
		}
	}
}

void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it

	for (const Cell& cell : Cells)
	{
		const auto points = cell.GetRectPoints();

		for (size_t i = 0; i < points.size(); ++i)
		{
			FVector start(points[i], 0.f);
			FVector end(points[(i + 1) % points.size()], 0.f);

			DrawDebugLine(pWorld,start,end,FColor::Green,false,-1.f,0,4.f);
		}

		// Draw number of agents in cell
		FVector center(
			(cell.BoundingBox.Min.X + cell.BoundingBox.Max.X) / 2.f,
			(cell.BoundingBox.Min.Y + cell.BoundingBox.Max.Y) / 2.f,
			0.f
		);

		DrawDebugString(pWorld,center,FString::FromInt(cell.Agents.size()),nullptr,FColor::White,0.f,true);
	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	// TODO Calculate the index of the cell based on the position	
	float localX = Pos.X - OriginX;
	float localY = Pos.Y - OriginY;

	int col = static_cast<int>(localX / CellWidth);
	int row = static_cast<int>(localY / CellHeight);

	// Clamp to grid bounds
	col = FMath::Clamp(col, 0, NrOfCols - 1);
	row = FMath::Clamp(row, 0, NrOfRows - 1);

	return row * NrOfCols + col;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	return true;
}