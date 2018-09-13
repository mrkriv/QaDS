// Copyright 2017-2018 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "UObject/UnrealType.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SViewport.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "Viewports.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "IDetailsView.h"
#include "Slate/SceneViewport.h"
#include "DialogEditorNodes.h"
#include "PhraseNodeCustomization.h"
#include "DetailLayoutBuilder.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

#define LOCTEXT_NAMESPACE "DialogPhraseEventCustomization"

TSharedRef<IDetailCustomization> FPhraseNodeDetails::MakeInstance()
{
	return MakeShareable(new FPhraseNodeDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FPhraseNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	auto DataProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UPhraseNode, Data));

	uint32 DataPropertyCount;
	DataProperty->GetNumChildren(DataPropertyCount);

	DetailLayout.HideProperty(DataProperty);

	auto AutoTimeProperty = DataProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDialogPhraseInfo, AutoTime));
	bool isAutoTime;
	AutoTimeProperty->GetValue(isAutoTime);
	AutoTimeProperty->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([&DetailLayout]()
	{
		DetailLayout.ForceRefreshDetails();
	}));
	
	for (uint32 i = 0; i < DataPropertyCount; i++)
	{
		auto prop = DataProperty->GetChildHandle(i);
		auto name = prop->GetProperty()->GetName();

		auto& categoty = DetailLayout.EditCategory(FName(*prop->GetMetaData("Category")));

		/*if (name == "Text")
		{
			FText Text;
			prop->GetValue(Text);

			categoty.AddCustomRow(prop->GetPropertyDisplayName()).NameContent()
			[
				prop->CreatePropertyNameWidget()
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.MinDesiredHeight(150)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SMultiLineEditableTextBox)
						.Text(Text)
						.SelectAllTextWhenFocused(false)
						.ClearKeyboardFocusOnCommit(true)
						.OnTextCommitted_Lambda([prop](const FText& NewText, ETextCommit::Type Type) 
						{
							prop->SetValue(NewText);
						})
						.SelectAllTextOnCommit(false)
						.AutoWrapText(true)
						.ModiferKeyForNewLine(EModifierKey::Shift)
					]
				]
			];
		}
		else*/ if (name == "PhraseManualTime" && isAutoTime)
		{
			continue;
		}
		else
		{
			categoty.AddProperty(prop);
		}
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE