// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "BrushSet.h"
#include "Engine/Texture2D.h"
#include "CanvasTypes.h"
#include "IPluginManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "SlateGameResources.h" 
#include "SlateStyle.h"

TSharedPtr<FSlateStyleSet> FBrushSet::NodeStyleInstance = NULL;

void FBrushSet::Register()
{
	if ( !NodeStyleInstance.IsValid() )
	{
		NodeStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle( *NodeStyleInstance );
	}
}

void FBrushSet::Unregister()
{
	FSlateStyleRegistry::UnRegisterSlateStyle( *NodeStyleInstance );
	ensure( NodeStyleInstance.IsUnique() );
	NodeStyleInstance.Reset();
}

TSharedRef<FSlateStyleSet> FBrushSet::Create()
{
	TSharedPtr<FSlateStyleSet>  StyleSet = MakeShareable(new FSlateStyleSet("DialogSystem"));

	auto dir = FPaths::ConvertRelativePathToFull(
		IPluginManager::Get().FindPlugin(TEXT("DialogSystem"))->GetBaseDir() / "Resources/Icons");

	auto find_mask = dir / TEXT("*.png");

	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *find_mask, true, false);

#define ImageBrush(Name, Size) new FSlateImageBrush(dir / Name + ".png", FVector2D(Size, Size))

	StyleSet->Set("ClassIcon.DialogAsset", ImageBrush("DialogAsset_16", 16.0f));
	StyleSet->Set("ClassThumbnail.DialogAsset", ImageBrush("DialogAsset_128", 64.0f));

	StyleSet->Set("ClassIcon.DialogScript", ImageBrush("DialogScript_16", 16.0f));
	StyleSet->Set("ClassThumbnail.DialogScript", ImageBrush("DialogScript_128", 64.0f));

#undef ImageBrush

	for (auto file : Files)
	{
		auto name = FName(*("DialogSystem." + FPaths::GetBaseFilename(file)));
		StyleSet->Set(name, new FSlateImageBrush(FPaths::Combine(dir, file), FVector2D(24, 24)));
	}

	return StyleSet.ToSharedRef();
}

void FBrushSet::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FBrushSet::Get()
{
	return *NodeStyleInstance;
}

