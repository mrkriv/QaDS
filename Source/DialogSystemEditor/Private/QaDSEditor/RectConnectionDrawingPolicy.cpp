#include "DialogSystemEditor.h"
#include "GraphEditor.h"
#include "SGraphNode.h"
#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"
#include "RectConnectionDrawingPolicy.h"

FRectConnectionDrawingPolicy::FRectConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
, GraphObj(InGraphObj)
{
}

void FRectConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	//Collect node geometry
	//NodeWidgetMap.Empty();
	//for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	//{
	//	FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
	//	TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
	//	NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	//}

	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FRectConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, NULL, Params);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, EndPoint), EndPoint, Params);
	else
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, StartPoint), StartPoint, Params);
}

void FRectConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.WireThickness = 2.0f;
	Params.WireColor = FLinearColor::White;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;

	if (bDeemphasizeUnhoveredPins)
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
}

void FRectConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	auto LineSeparationAmount = 2.5f;

	auto DeltaPos = EndAnchorPoint - StartAnchorPoint;
	auto UnitDelta = DeltaPos.GetSafeNormal();
	auto Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	auto DirectionBias = Normal * LineSeparationAmount;
	auto LengthBias = ArrowRadius.X * UnitDelta;
	auto StartPoint = StartAnchorPoint + DirectionBias + LengthBias;
	auto EndPoint = EndAnchorPoint + DirectionBias - LengthBias;

	FVector2D CenterLeft;
	FVector2D CenterRight;

	if (Normal.X < 0)
	{
		CenterLeft = FVector2D(EndPoint.X + (StartPoint.X - EndPoint.X) / 2, StartPoint.Y);
		CenterLeft = FVector2D(EndPoint.X + (StartPoint.X - EndPoint.X) / 2, EndPoint.Y);

		DrawConnection(WireLayerID, StartPoint, EndPoint, Params);
	}
	else
	{
		CenterLeft = FVector2D(StartPoint.X, StartPoint.Y + (EndPoint.Y - StartPoint.Y) / 2);
		CenterRight = FVector2D(EndPoint.X, StartPoint.Y + (EndPoint.Y - StartPoint.Y) / 2);

		if (Normal.Y > 0)
		{
			StartPoint.X += 25;
			EndPoint.X -= 25;
		}
		else if (Normal.Y < 0)
		{
			StartPoint.X -= 25;
			EndPoint.X += 25;
		}

		DrawConnection(WireLayerID, StartPoint, CenterLeft, Params);
		DrawConnection(WireLayerID, CenterRight, CenterLeft, Params);
		DrawConnection(WireLayerID, CenterRight, EndPoint, Params);
	}

	auto ArrowDrawPos = EndPoint - CenterLeft;//((StartPoint + EndPoint) ) / 2;
	auto AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		Params.WireColor
	);
}

void FRectConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
	auto Delta = End - Start;
	auto NormDelta = Delta.GetSafeNormal();

	auto P0Tangent = NormDelta;
	auto P1Tangent = NormDelta;

	FSlateDrawElement::MakeDrawSpaceSpline(
		DrawElementsList,
		LayerId,
		Start, P0Tangent,
		End, P1Tangent,
		Params.WireThickness,
		ESlateDrawEffect::None,
		Params.WireColor
	);
}