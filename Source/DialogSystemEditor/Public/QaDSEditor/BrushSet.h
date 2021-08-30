#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"

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