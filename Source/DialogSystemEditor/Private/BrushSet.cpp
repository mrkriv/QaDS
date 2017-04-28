// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "BrushSet.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "SlateGameResources.h"

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
	TSharedRef<FSlateStyleSet> StyleRef = MakeShareable(new FSlateStyleSet("DialogSystem"));
	FSlateStyleSet& Style = StyleRef.Get();

	auto dir = FPaths::ConvertRelativePathToFull(FPaths::GamePluginsDir() / "DialogSystem/Resources/Icons/");
	auto find_mask = dir + TEXT("*.png");

	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *find_mask, true, false);

	for (auto file : Files)
	{
		auto name = FName(*("DialogSystem." + FPaths::GetBaseFilename(file)));
		Style.Set(name, new FSlateImageBrush(FPaths::Combine(dir, file), FVector2D(24, 24)));
	}

	return StyleRef;
}

#undef IMAGE_BRUSH

void FBrushSet::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FBrushSet::Get()
{
	return *NodeStyleInstance;
}