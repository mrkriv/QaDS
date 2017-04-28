// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemEditor.h"
#include "DialogAsset.h"
#include "DialogScript.h"
#include "UObject/UnrealType.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SComboBox.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SHyperlink.h"
#include "ScopedTransaction.h"
#include "Slate/SlateTextureAtlasInterface.h"
#include "DialogPhraseEventParamCustomization.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

TSharedRef<IPropertyTypeCustomization> FDialogPhraseEventParamCustomization::MakeInstance()
{
	return MakeShareable(new FDialogPhraseEventParamCustomization());
}

FDialogPhraseEventParamCustomization::FDialogPhraseEventParamCustomization()
{
}

void FDialogPhraseEventParamCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName PropertyName_Name = GET_MEMBER_NAME_CHECKED(FDialogPhraseEventParam, Name);
	static const FName PropertyName_Value = GET_MEMBER_NAME_CHECKED(FDialogPhraseEventParam, Value);


	PropertyHandle_Name = StructPropertyHandle->GetChildHandle(PropertyName_Name);
	PropertyHandle_Value = StructPropertyHandle->GetChildHandle(PropertyName_Value);

	check(PropertyHandle_Name.IsValid());
	check(PropertyHandle_Value.IsValid());

	FString Name;
	FString Value;

	PropertyHandle_Name->GetValue(Name);
	PropertyHandle_Value->GetValue(Value);

	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(FText::FromString(Name))
		]
		.ValueContent()
		.MinDesiredWidth(250)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(Value))
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void FDialogPhraseEventParamCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}