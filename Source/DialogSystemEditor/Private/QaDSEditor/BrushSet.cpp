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
	if (!NodeStyleInstance.IsValid())
	{
		NodeStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*NodeStyleInstance);
	}
}

void FBrushSet::Unregister()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*NodeStyleInstance);
	ensure(NodeStyleInstance.IsUnique());
	NodeStyleInstance.Reset();
}

TSharedRef<FSlateStyleSet> FBrushSet::Create()
{
	TSharedPtr<FSlateStyleSet>  StyleSet = MakeShareable(new FSlateStyleSet("DialogSystem"));

	auto dir = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DialogSystem"))->GetBaseDir() / "Resources" / "Icons");
	auto find_mask = dir / TEXT("*.png");

	TArray<FString> Files;
	IFileManager::Get().FindFiles(Files, *find_mask, true, false);

#define ImageBrush(Name, Size) new FSlateImageBrush(dir / Name + ".png", FVector2D(Size, Size))

	StyleSet->Set("ClassIcon.DialogAsset", ImageBrush("DialogAsset_16", 16.0f));
	StyleSet->Set("ClassThumbnail.DialogAsset", ImageBrush("DialogAsset_128", 64.0f));

	StyleSet->Set("ClassIcon.DialogScript", ImageBrush("DialogScript_16", 16.0f));
	StyleSet->Set("ClassThumbnail.DialogScript", ImageBrush("DialogScript_128", 64.0f));

	StyleSet->Set("ClassIcon.QuestAsset", ImageBrush("QuestAsset_16", 16.0f));
	StyleSet->Set("ClassThumbnail.QuestAsset", ImageBrush("QuestAsset_128", 64.0f));

	StyleSet->Set("ClassIcon.QuestScript", ImageBrush("QuestScript_16", 16.0f));
	StyleSet->Set("ClassThumbnail.QuestScript", ImageBrush("QuestScript_128", 64.0f));

#undef ImageBrush

	for (auto file : Files)
	{
		auto name = "DialogSystem." + FPaths::GetBaseFilename(file);
		auto spectator = 0;
		auto size = FVector2D(24, 24);

		if (name.FindLastChar('_', spectator))
		{
			auto numberStr = name.Right(name.Len() - spectator - 1);
			auto number = FCString::Atoi(*numberStr);

			if (number > 2 && number < 256)
			{
				size.X = number;
				size.Y = number;
			}
		}

		StyleSet->Set(FName(*name), new FSlateImageBrush(FPaths::Combine(dir, file), size));
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
