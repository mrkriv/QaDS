// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "BrushSet.h"
#include "Engine/Texture2D.h"
#include "CanvasTypes.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "SlateGameResources.h" 
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"

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

void UDefaultAssetThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas)
{
	UTexture2D* GridTexture = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *GetIconPath()))
		return;

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			GridTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
			if (GridTexture)
			{
				Width = ImageWrapper->GetWidth();
				Height = ImageWrapper->GetHeight();

				void* TextureData = GridTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
				GridTexture->PlatformData->Mips[0].BulkData.Unlock();

				GridTexture->UpdateResource();
			}
		}
	}

	if (GridTexture == NULL)
		return;

	Canvas->DrawTile(
		(float)X, (float)Y,
		(float)Width, (float)Height,
		0.0f, 0.0f,
		4.0f, 4.0f,
		FLinearColor::White,
		GridTexture->Resource,
		false);
}

FString UDialogAssetThumbnailRenderer::GetIconPath()
{
	auto brus = FBrushSet::Get().GetBrush("DialogSystem.DialogAsset");
	return brus->GetResourceName().ToString();
}

FString UDialogScriptThumbnailRenderer::GetIconPath()
{
	auto brus = FBrushSet::Get().GetBrush("DialogSystem.DialogScript");
	return brus->GetResourceName().ToString();
}