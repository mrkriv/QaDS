#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

class FCanvas;
class FRenderTarget;

class FBrushSet
{
public:
	static void Register();
	static void Unregister();
	static void ReloadTextures();
	static const ISlateStyle& Get();
	static void AddImage(FName name, FString file, float size);

private:
	static TSharedPtr<class FSlateStyleSet> Instance; 
};