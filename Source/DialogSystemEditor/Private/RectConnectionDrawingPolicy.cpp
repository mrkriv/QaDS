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

static const FLinearColor DefaultColor(1.0f, 1.0f, 1.0f);
void FRectConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	Params.WireThickness = 2.0f;
	Params.WireColor = DefaultColor;
	Params.bUserFlag1 = false;	// bidirectional

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;

	if (bDeemphasizeUnhoveredPins)
		ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
}

void FRectConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
		TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	// Now draw
	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FRectConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	bool bBiDirectional = false;
	FConnectionParams Params;
	Params.WireThickness = 0.5f;
	Params.WireColor = FLinearColor::White;
	Params.bDrawBubbles = false;
	Params.bUserFlag1 = bBiDirectional;
	DetermineWiringStyle(Pin, NULL, /*inout*/ Params);

	if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, EndPoint), EndPoint, Params);
	else
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, StartPoint), StartPoint, Params);
}

void FRectConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	// hacky: use bBidirectional flag to reverse direction of connection (used by debugger)
	bool Bidirectional = Params.bUserFlag1;
	const FVector2D& P0 = Bidirectional ? EndAnchorPoint : StartAnchorPoint;
	const FVector2D& P1 = Bidirectional ? StartAnchorPoint : EndAnchorPoint;

	Internal_DrawLineWithArrow(P0, P1, Params);
}

void FRectConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

void FRectConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	//@TODO: Should this be scaled by zoom factor?
	auto LineSeparationAmount = 2.5f;

	auto DeltaPos = EndAnchorPoint - StartAnchorPoint;
	auto UnitDelta = DeltaPos.GetSafeNormal();
	auto Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
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
	const FVector2D& P0 = Start;
	const FVector2D& P1 = End;

	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	const FVector2D P0Tangent = NormDelta;
	const FVector2D P1Tangent = NormDelta;

	// Draw the spline itself
	FSlateDrawElement::MakeDrawSpaceSpline(
		DrawElementsList,
		LayerId,
		P0, P0Tangent,
		P1, P1Tangent,
		Params.WireThickness,
		ESlateDrawEffect::None,
		Params.WireColor
	);
}
