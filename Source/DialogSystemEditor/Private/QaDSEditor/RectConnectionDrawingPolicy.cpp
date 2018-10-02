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

	DrawConnection(WireLayerID, StartPoint, EndPoint, Params);
}

void FRectConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.WireThickness = 2.0f;
	Params.WireColor = FLinearColor::White;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;

	if (bDeemphasizeUnhoveredPins)
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
}

void FRectConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	auto padding = 40.0f * ZoomFactor;

	auto StartSize = StartGeom.GetDrawSize();
	auto StartPS = StartGeom.AbsolutePosition + FVector2D(padding, 0);
	auto StartPC = StartGeom.AbsolutePosition + FVector2D(StartSize.X * 0.5f, StartSize.Y * 0.5f);
	auto StartPE = StartGeom.AbsolutePosition + FVector2D(StartSize.X - padding, StartSize.Y);

	auto EndSize = StartGeom.GetDrawSize();
	auto EndPS = EndGeom.AbsolutePosition + FVector2D(padding, 0);
	auto EndPC = EndGeom.AbsolutePosition + FVector2D(EndSize.X * 0.5f, EndSize.Y * 0.5f);
	auto EndPE = EndGeom.AbsolutePosition + FVector2D(EndSize.X - padding, EndSize.Y);

	auto center = (StartPC + EndPC) * 0.5f;

	auto StartAnchor = FVector2D(FMath::Clamp(center.X, StartPS.X, StartPE.X), StartPC.Y);
	auto EndAnchor = FVector2D(FMath::Clamp(center.X, EndPS.X, EndPE.X), EndPC.Y);

	DrawSplineWithArrow(StartAnchor, EndAnchor, Params);
}

void FRectConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params)
{
	if (StartPoint.Y < EndPoint.Y)
	{
		auto CenterLeft = FVector2D(StartPoint.X, StartPoint.Y + (EndPoint.Y - StartPoint.Y) / 2);
		auto CenterRight = FVector2D(EndPoint.X, StartPoint.Y + (EndPoint.Y - StartPoint.Y) / 2);

		DrawConnection(WireLayerID, StartPoint, CenterLeft, Params);
		DrawConnection(WireLayerID, CenterRight, CenterLeft, Params);
		DrawConnection(WireLayerID, CenterRight, EndPoint, Params);
	}
	else if (StartPoint.Y > EndPoint.Y)
	{
		auto newParams = FConnectionParams(Params);
		newParams.WireColor *= 0.6f;

		auto d = 20.0f * ZoomFactor;
		auto p1 = FVector2D(StartPoint.X, StartPoint.Y + d);
		auto p2 = FVector2D(EndPoint.X + (StartPoint.X - EndPoint.X) * 0.5f, p1.Y);
		auto p3 = FVector2D(p2.X, EndPoint.Y - d);
		auto p4 = FVector2D(EndPoint.X, p3.Y);

		DrawConnection(WireLayerID, StartPoint, p1, newParams);
		DrawConnection(WireLayerID, p1, p2, newParams);
		DrawConnection(WireLayerID, p2, p3, newParams);
		DrawConnection(WireLayerID, p3, p4, newParams);
		DrawConnection(WireLayerID, p4, EndPoint, newParams);
	}
	else
	{
		DrawConnection(WireLayerID, StartPoint, EndPoint, Params);
	}
}

void FRectConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
	auto NormDelta = (End - Start).GetSafeNormal();
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