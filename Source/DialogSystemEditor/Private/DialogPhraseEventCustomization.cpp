#include "DialogSystemEditor.h"
#include "DialogAsset.h"
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
#include "DialogPhraseEventCustomization.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

#define LOCTEXT_NAMESPACE "DialogPhraseEventCustomization"

TSharedRef<IPropertyTypeCustomization> FDialogPhraseEventCustomization::MakeInstance()
{
	return MakeShareable(new FDialogPhraseEventCustomization());
}

FDialogPhraseEventCustomization::FDialogPhraseEventCustomization()
{
}

void FDialogPhraseEventCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName PropertyName_ObjectClass = GET_MEMBER_NAME_CHECKED(FDialogPhraseEvent, ObjectClass);
	static const FName PropertyName_EventName = GET_MEMBER_NAME_CHECKED(FDialogPhraseEvent, EventName);
	static const FName PropertyName_OwnerNode = GET_MEMBER_NAME_CHECKED(FDialogPhraseEvent, OwnerNode);
	static const FName PropertyName_CallType = GET_MEMBER_NAME_CHECKED(FDialogPhraseEvent, CallType);
	static const FName PropertyName_FindTag = GET_MEMBER_NAME_CHECKED(FDialogPhraseEvent, FindTag);

	PropertyHandle_ObjectClass = StructPropertyHandle->GetChildHandle(PropertyName_ObjectClass);
	PropertyHandle_EventName = StructPropertyHandle->GetChildHandle(PropertyName_EventName);
	PropertyHandle_OwnerNode = StructPropertyHandle->GetChildHandle(PropertyName_OwnerNode);
	PropertyHandle_CallType = StructPropertyHandle->GetChildHandle(PropertyName_CallType);
	PropertyHandle_FindTag = StructPropertyHandle->GetChildHandle(PropertyName_FindTag);

	check(PropertyHandle_ObjectClass.IsValid());
	//check(PropertyHandle_OwnerNode.IsValid());
	check(PropertyHandle_EventName.IsValid());
	check(PropertyHandle_CallType.IsValid());
	check(PropertyHandle_FindTag.IsValid());

	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(StructPropertyHandle->GetPropertyDisplayName())
		]
		.ValueContent()
		.MinDesiredWidth(250)
		[
			SNew(SButton)
			.OnClicked(this, &FDialogPhraseEventCustomization::OnTitleClick)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(this, &FDialogPhraseEventCustomization::GetTitleText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void FDialogPhraseEventCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructBuilder.AddChildProperty(PropertyHandle_CallType.ToSharedRef());

	StructBuilder.AddChildProperty(PropertyHandle_ObjectClass.ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FDialogPhraseEventCustomization::GetObjectClassVisibility));

	StructBuilder.AddChildProperty(PropertyHandle_EventName.ToSharedRef());

	StructBuilder.AddChildProperty(PropertyHandle_FindTag.ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FDialogPhraseEventCustomization::GetFingTagVisibility));
}

FReply FDialogPhraseEventCustomization::OnTitleClick()
{
	UObject* Property_ObjectClass;
	UDialogNode* Property_OwnerNode;
	EDialogPhraseEventCallType Property_CallType;

	PropertyHandle_ObjectClass->GetValue(Property_ObjectClass);
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);
	PropertyHandle_OwnerNode->GetValue((uint8&)Property_OwnerNode);

	if (Property_ObjectClass != NULL)
	{
		FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *Property_ObjectClass->GetPathName() }));
	}
	else if (Property_CallType == EDialogPhraseEventCallType::DialogScript)
	{
		auto sc = Property_OwnerNode->OwnerDialog->DialogScript;

		if(sc != NULL)
			FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *sc->GetPathName() }));
	}

	return FReply::Handled();
}

FText FDialogPhraseEventCustomization::GetTitleText() const
{
	UObject* Property_ObjectClass;
	EDialogPhraseEventCallType Property_CallType;
	FString Property_FindTag;
	FName Property_EventName;

	PropertyHandle_ObjectClass->GetValue(Property_ObjectClass);
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);
	PropertyHandle_EventName->GetValue(Property_EventName);
	PropertyHandle_FindTag->GetValue(Property_FindTag);

	FString title = "None";

	switch (Property_CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		title = Property_EventName.ToString();
		break;

	case EDialogPhraseEventCallType::Player:
		title = TEXT("Player.") + Property_EventName.ToString();
		break;

	case EDialogPhraseEventCallType::Interlocutor:
		title = TEXT("Interlocutor.") + Property_EventName.ToString();
		break;

	case EDialogPhraseEventCallType::CreateNew:
		if (Property_ObjectClass)
			title = TEXT("new ") + Property_ObjectClass->GetName() + TEXT(".") + Property_EventName.ToString();
		break;

	case EDialogPhraseEventCallType::FindByTag:
		if (Property_ObjectClass)
			title = Property_ObjectClass->GetName() + TEXT("(") + Property_FindTag + TEXT(").") + Property_EventName.ToString();
		break;

	default:
		break;
	}

	return FText::FromString(title);
}

EVisibility FDialogPhraseEventCustomization::GetFingTagVisibility() const
{
	EDialogPhraseEventCallType Property_CallType;
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);

	return Property_CallType == EDialogPhraseEventCallType::FindByTag ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FDialogPhraseEventCustomization::GetObjectClassVisibility() const
{
	EDialogPhraseEventCallType Property_CallType;
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);

	return
		(Property_CallType == EDialogPhraseEventCallType::CreateNew ||
	 	 Property_CallType == EDialogPhraseEventCallType::FindByTag) ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE