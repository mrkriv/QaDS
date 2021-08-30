#include "StoryKeyWindow.h"
#include "StoryInformationManager.h"

#include "FileHelper.h"
#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Views/SListView.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SStoryKeyWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.Padding(4.0f, 2.0f, 4.0f, 2.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
					  	SAssignNew(searchBox, SSearchBox)
						.OnTextChanged(this, &SStoryKeyWindow::HandleSearch)
					]
					+ SHorizontalBox::Slot()
					.Padding(4.0f, 0.0f, 0.0f, 0.0f)
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Import"))
						.OnClicked(this, &SStoryKeyWindow::HandleImportButton)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Export"))
						.OnClicked(this, &SStoryKeyWindow::HandleExportButton)
					]
				]
				+ SVerticalBox::Slot()
				.Padding(0.0f, 4.0f, 0.0f, 4.0f)
				[
					SAssignNew(keyListView, SListView<TSharedPtr<FString>>)
					.ItemHeight(16.0f)
					.ListItemsSource(&keys)
					.OnGenerateRow(this, &SStoryKeyWindow::HandleGenerateRow)
					.OnSelectionChanged(this, &SStoryKeyWindow::HandleSelectKey)
					.SelectionMode(ESelectionMode::Single)					
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
					  	SAssignNew(editKeyTextBox, SEditableTextBox)
						.HintText(FText::FromString("Enter key name"))
					]
					+ SHorizontalBox::Slot()
					.Padding(4.0f, 0.0f, 0.0f, 0.0f)
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Add"))
						.OnClicked(this, &SStoryKeyWindow::HandleAddKeyButton)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Remove"))
						.OnClicked(this, &SStoryKeyWindow::HandleRemoveKeyButton)
					]
				]
			]
		]
	];
}

TSharedRef<ITableRow> SStoryKeyWindow::HandleGenerateRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(STextBlock).Text(FText::FromString(*Item))
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SStoryKeyWindow::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, DeltaTime);

	auto newKeyManager = UStoryKeyManager::GetStoryKeyManager(NULL);
	if (newKeyManager != NULL && newKeyManager != keyManager)
	{
		keyManager = newKeyManager;
		keyManager->OnKeyAdd.AddSP(this, &SStoryKeyWindow::OnStorageKeyAdd);
		keyManager->OnKeyRemove.AddSP(this, &SStoryKeyWindow::OnStorageKeyRemove);
		keyManager->OnKeysLoaded.AddSP(this, &SStoryKeyWindow::OnStorageKeysLoaded);

		UpdateKeys();
	}
}

void SStoryKeyWindow::HandleSelectKey(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid())
	{
		editKeyTextBox->SetText(FText::FromString(*NewSelection));
	}
}

void SStoryKeyWindow::UpdateKeys()
{
	keys.Reset();

	auto filter = searchBox->GetText().ToString();
	auto keysRaw = keyManager->GetKeys();

	keysRaw.Sort([](const FName& a, const FName& b)
	{
		return a.Compare(b) < 0;
	});

	for (auto key : keysRaw)
	{
		auto keyStr = key.ToString();

		if (filter.IsEmpty() || keyStr.Contains(filter))
		{
			keys.Add(MakeShareable(new FString(keyStr)));
		}
	}
	
	keyListView->RebuildList();
}

FReply SStoryKeyWindow::HandleExportButton()
{
	TArray<FString> Filenames;
	bool isSaved = FDesktopPlatformModule::Get()->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared()),
		"Choose save location",
		FPaths::ProjectUserDir(),
		TEXT(""),
		TEXT("Text|*.txt|All|*.*"),
		EFileDialogFlags::None,
		Filenames
	);

	if (isSaved)
	{
		FString text;
		for (auto key : keyManager->GetKeys())
		{
			text += key.ToString() + "\n";
		}
		
		text.RemoveFromEnd("\n");

		FFileHelper::SaveStringToFile(text, *Filenames[0]);

		LogInfo("Export successful");
	}

	return FReply::Handled();
}

FReply SStoryKeyWindow::HandleImportButton()
{
	TArray<FString> Filenames;
	bool isOpen = FDesktopPlatformModule::Get()->OpenFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared()),
		"Choose file",
		FPaths::ProjectUserDir(),
		TEXT("Text|*.txt|All|*.*"),
		TEXT(""),
		EFileDialogFlags::None,
		Filenames
	);

	if (isOpen)
	{
		TArray<FString> newKeys;
		TSet<FName> keysSet;
		FFileHelper::LoadFileToStringArray(newKeys, *Filenames[0]);

		for (auto key : newKeys)
		{
			keysSet.Add(*key);
		}

		if (keysSet.Num() > 0)
		{
			keyManager->SetKeys(keysSet);
			LogInfo("Import successful");
		}
		else
		{
			LogInfo("Storage is empty");
		}
	}

	return FReply::Handled();
}

FReply SStoryKeyWindow::HandleAddKeyButton()
{
	auto key = editKeyTextBox->GetText().ToString();
	if (keyManager->AddKey(*key))
	{
		editKeyTextBox->SetText(FText());
	}
	else
	{
		LogInfo("Already contains");
	}

	return FReply::Handled();
}

FReply SStoryKeyWindow::HandleRemoveKeyButton()
{
	auto key = editKeyTextBox->GetText().ToString();
	if (keyManager->RemoveKey(*key))
	{
		editKeyTextBox->SetText(FText());
	}
	else
	{
		LogInfo("Key not found");
	}

	return FReply::Handled();
}

void SStoryKeyWindow::HandleSearch(const FText& Text)
{
	UpdateKeys();
}

void SStoryKeyWindow::OnStorageKeyAdd(const FName& StoreKey)
{
	LogInfo("Add key: " + StoreKey.ToString());
	UpdateKeys();
}

void SStoryKeyWindow::OnStorageKeyRemove(const FName& StoreKey)
{
	LogInfo("Remove key: " + StoreKey.ToString());
	UpdateKeys();
}

void SStoryKeyWindow::OnStorageKeysLoaded(const TArray<FName>& StoreKeys)
{
	LogInfo("Loaded keys");
	UpdateKeys();
}

void SStoryKeyWindow::LogInfo(const FString& message)
{
	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(FText::FromString(message)));
}