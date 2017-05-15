// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
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

private:
	static TSharedRef<class FSlateStyleSet> Create();
	static TSharedPtr<class FSlateStyleSet> NodeStyleInstance;
};