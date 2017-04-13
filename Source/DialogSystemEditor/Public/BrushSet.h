#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

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