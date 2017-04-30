// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ThumbnailRendering/DefaultSizedThumbnailRenderer.h"
#include "BrushSet.generated.h"

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


UCLASS()
class UDefaultAssetThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
	GENERATED_BODY()

	// UThumbnailRenderer interface
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas) override;
	// End of UThumbnailRenderer interface

protected:
	virtual FString GetIconPath() { return TEXT(""); }
};

UCLASS()
class UDialogAssetThumbnailRenderer : public UDefaultAssetThumbnailRenderer
{
	GENERATED_BODY()

protected:
	virtual FString GetIconPath() override;
};

UCLASS()
class UDialogScriptThumbnailRenderer : public UDefaultAssetThumbnailRenderer
{
	GENERATED_BODY()

protected:
	virtual FString GetIconPath() override;
};
