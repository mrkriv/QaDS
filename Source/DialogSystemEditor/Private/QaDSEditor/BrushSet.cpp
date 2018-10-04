#include "DialogSystemEditor.h"
#include "BrushSet.h"
#include "Engine/Texture2D.h"
#include "CanvasTypes.h"
#include "IPluginManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "SlateGameResources.h" 
#include "SlateStyle.h"

TSharedPtr<FSlateStyleSet> FBrushSet::Instance = NULL;

void FBrushSet::Register()
{
	if (Instance.IsValid())
		return;

	Instance = MakeShareable(new FSlateStyleSet("DialogSystem"));

	auto dir = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DialogSystem"))->GetBaseDir() / "Resources" / "Icons");
	auto find_mask = dir / TEXT("*.png");

	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *find_mask, true, false);

	for (auto file : Files)
	{
		auto name = "DialogSystem." + FPaths::GetBaseFilename(file);
		auto spectator = 0;
		auto size = 24;

		if (name.FindLastChar('_', spectator))
		{
			auto numberStr = name.Right(name.Len() - spectator - 1);
			auto number = FCString::Atoi(*numberStr);

			if (number > 2 && number < 256)
			{
				size = number;
			}
		}

		AddImage(FName(*name), FPaths::Combine(dir , file), size);
	}

	AddImage("ClassIcon.DialogAsset", dir / "DialogAsset_16.png", 16.0f);
	AddImage("ClassThumbnail.DialogAsset", dir / "DialogAsset_64.png", 64.0f);

	AddImage("ClassIcon.DialogScript", dir / "DialogScript_16.png", 16.0f);
	AddImage("ClassThumbnail.DialogScript", dir / "DialogScript_64.png", 64.0f);

	AddImage("ClassIcon.QuestAsset", dir / "QuestAsset_16.png", 16.0f);
	AddImage("ClassThumbnail.QuestAsset", dir / "QuestAsset_64.png", 64.0f);

	AddImage("ClassIcon.QuestScript", dir / "QuestScript_16.png", 16.0f);
	AddImage("ClassThumbnail.QuestScript", dir / "QuestScript_64.png", 64.0f);

	FSlateStyleRegistry::RegisterSlateStyle(*Instance);
}

void FBrushSet::AddImage(FName name, FString file, float size)
{
	if (Instance.IsValid())
		Instance->Set(name, new FSlateImageBrush(file, FVector2D(size, size)));
}

void FBrushSet::Unregister()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*Instance);
	ensure(Instance.IsUnique());
	Instance.Reset();
}

void FBrushSet::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FBrushSet::Get()
{
	return *Instance;
}
