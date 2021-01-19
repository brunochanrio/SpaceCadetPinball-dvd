#include "pch.h"
#include "TEdgeSegment.h"

#include "objlist_class.h"
#include "TCircle.h"
#include "TCollisionComponent.h"
#include "TLine.h"

TEdgeSegment::TEdgeSegment(TCollisionComponent* collComp, char* someFlag, unsigned visualFlag)
{
	CollisionComponent = collComp;
	PinbCompFlag2Ptr = someFlag;
	VisualFlag = visualFlag;
	ProcessedFlag = 0;
}

void TEdgeSegment::port_draw()
{
}

TEdgeSegment* TEdgeSegment::install_wall(float* floatArr, TCollisionComponent* collComp, char* flagPtr,
                                         unsigned int visual_flag,
                                         float offset, int wallValue)
{
	vector_type center{}, start{}, end{}, prevCenter{}, vec1{}, vec2{}, dstVec{};
	TEdgeSegment* edge = nullptr;

	wall_type wallType = static_cast<wall_type>(static_cast<int>(floor(*floatArr) - 1.0f));
	switch (wallType)
	{
	case wall_type::Circle:
		{
			center.X = floatArr[1];
			center.Y = floatArr[2];
			auto radius = offset + floatArr[3];
			auto circle = new TCircle(collComp, flagPtr, visual_flag, &center, radius);
			edge = circle;

			if (circle)
			{
				circle->WallValue = reinterpret_cast<void*>(wallValue);
				circle->place_in_grid();
			}

			collComp->EdgeList->Add(circle);
			break;
		}
	case wall_type::Line:
		{
			start.X = floatArr[1];
			start.Y = floatArr[2];
			end.X = floatArr[3];
			end.Y = floatArr[4];
			auto line = new TLine(collComp, flagPtr, visual_flag, &start, &end);
			edge = line;

			if (line)
			{
				line->WallValue = reinterpret_cast<void*>(wallValue);
				line->Offset(offset);
				line->place_in_grid();
				collComp->EdgeList->Add(line);
			}
			break;
		}
	default:
		{
			int wallTypeI = static_cast<int>(wallType);
			auto floatArrPtr = floatArr + 1;
			prevCenter.X = floatArr[2 * wallTypeI - 1];
			prevCenter.Y = floatArr[2 * wallTypeI];

			for (int index = 0; index < wallTypeI; index++, floatArrPtr += 2)
			{
				float centerX2, centerY2;
				if (index >= wallTypeI - 1)
				{
					centerX2 = floatArr[1];
					centerY2 = floatArr[2];
				}
				else
				{
					centerX2 = floatArrPtr[2];
					centerY2 = floatArrPtr[3];
				}
				auto centerX1 = floatArrPtr[0];
				auto centerY1 = floatArrPtr[1];

				center.X = centerX1;
				center.Y = centerY1;
				if (offset != 0.0)
				{
					vec1.X = centerX1 - prevCenter.X;
					vec1.Y = center.Y - prevCenter.Y;
					vec2.X = centerX2 - centerX1;
					vec2.Y = centerY2 - center.Y;
					maths::cross(&vec1, &vec2, &dstVec);
					if (dstVec.Z > 0.0 && offset > 0.0 ||
						dstVec.Z < 0.0 && offset < 0.0)
					{
						float radius = offset * 1.001f;
						auto circle = new TCircle(collComp, flagPtr, visual_flag, &center, radius);

						if (circle)
						{
							circle->WallValue = reinterpret_cast<void*>(wallValue);
							circle->place_in_grid();
							collComp->EdgeList->Add(circle);
						}
					}
				}

				start.X = floatArrPtr[0];
				start.Y = floatArrPtr[1];
				end.X = floatArrPtr[2];
				end.Y = floatArrPtr[3];
				auto line = new TLine(collComp, flagPtr, visual_flag, &start, &end);
				edge = line;

				if (line)
				{
					line->WallValue = reinterpret_cast<void*>(wallValue);
					line->Offset(offset);
					line->place_in_grid();
					collComp->EdgeList->Add(line);
				}

				prevCenter = center;
			}
		}
	}

	return edge;
}