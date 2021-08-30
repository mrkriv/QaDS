#include "DialogAsset.h"
#include "UObject/UnrealType.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SHyperlink.h"
#include "Slate/SlateTextureAtlasInterface.h"
#include "DialogPhraseEventCustomization.h"

#include "DialogNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

#define GET_PROPERTY(Property) GET_PROPERTY_IN_TYPE(FDialogPhraseEvent, Property)
#define GET_PROPERTY_IN_TYPE(Type, Property) StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(Type, Property))

TSharedRef<IPropertyTypeCustomization> FDialogPhraseEventCustomization::MakeInstance()
{
	return MakeShareable(new FDialogPhraseEventCustomization());
}

void FDialogPhraseEventCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> structPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = structPropertyHandle;

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
	StructBuilder.AddProperty(GET_PROPERTY(CallType).ToSharedRef());
	StructBuilder.AddProperty(GET_PROPERTY(EventName).ToSharedRef());

	StructBuilder.AddProperty(GET_PROPERTY(ObjectClass).ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FDialogPhraseEventCustomization::GetObjectClassVisibility));

	StructBuilder.AddProperty(GET_PROPERTY(FindTag).ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FDialogPhraseEventCustomization::GetFingTagVisibility));

	if (GET_PROPERTY_IN_TYPE(FDialogPhraseCondition, InvertCondition).IsValid())
		StructBuilder.AddProperty(GET_PROPERTY_IN_TYPE(FDialogPhraseCondition, InvertCondition).ToSharedRef());

	UObject* Property_ObjectClass;
	FName Property_EventName;

	GET_PROPERTY(EventName)->GetValue(Property_EventName);
	GET_PROPERTY(ObjectClass)->GetValue(Property_ObjectClass);

	if (Property_ObjectClass == NULL)
		return;

	auto func = Cast<UClass>(Property_ObjectClass)->ClassDefaultObject->FindFunction(Property_EventName);
	auto array = GET_PROPERTY(Parameters)->AsArray();

	if (func == NULL || !array.IsValid())
		return;

	uint32 i = 0;
	for (TFieldIterator<UProperty> PropIt(func); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt, ++i)
	{
		UProperty *Prop = *PropIt;

		FText name = FText::FromString(Prop->GetName());
		FString value;

		uint32 arrayLenght;
		array->GetNumElements(arrayLenght);

		if (i >= arrayLenght)
			break;
			
		auto param = array->GetElement(i);
		param->GetValue(value);

		StructBuilder.AddCustomRow(name)
			.NameContent()
			[
				StructPropertyHandle->CreatePropertyNameWidget(name)
			]
			.ValueContent()
			.MinDesiredWidth(250)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.FillWidth(4)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(value))
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.OnTextCommitted_Lambda([param](const FText& Text, ETextCommit::Type CommitMethod)
					{
						param->SetValue(Text.ToString());
					})
				]
				+ SHorizontalBox::Slot()
				.Padding(8, 2, 0, 0)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.FillWidth(1)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Prop->GetCPPType()))
					.Font(IDetailLayoutBuilder::GetDetailFont())
				]
			];
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply FDialogPhraseEventCustomization::OnTitleClick()
{
	UObject* Property_ObjectClass;
	UObject* Property_OwnerNode;
	EDialogPhraseEventCallType Property_CallType;

	GET_PROPERTY(ObjectClass)->GetValue(Property_ObjectClass);
	GET_PROPERTY(CallType)->GetValue((uint8&)Property_CallType);
	GET_PROPERTY(OwnerNode)->GetValue(Property_OwnerNode);

	if (Property_CallType == EDialogPhraseEventCallType::DialogScript && Property_OwnerNode != NULL)
	{
		auto node = Cast<UDialogNode>(Property_OwnerNode);

		if (node == NULL || node->OwnerDialog == NULL || !node->OwnerDialog->DialogScriptClass.IsValid())
			return FReply::Handled();

		auto sc = Cast<UDialogNode>(Property_OwnerNode)->OwnerDialog->DialogScriptClass.Get();

		if (sc != NULL)
			FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *sc->GetPathName() }));
	}
	else if (Property_ObjectClass != NULL)
	{
		FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *Property_ObjectClass->GetPathName() }));
	}

	return FReply::Handled();
}

FText FDialogPhraseEventCustomization::GetTitleText() const
{
	FDialogPhraseEvent phraseEvent;
	StructPropertyHandle->GetValue((uint8&)phraseEvent);

	return FText::FromString(phraseEvent.ToString());
}

EVisibility FDialogPhraseEventCustomization::GetFingTagVisibility() const
{
	EDialogPhraseEventCallType Property_CallType;
	GET_PROPERTY(CallType)->GetValue((uint8&)Property_CallType);

	return Property_CallType == EDialogPhraseEventCallType::FindByTag ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FDialogPhraseEventCustomization::GetObjectClassVisibility() const
{
	EDialogPhraseEventCallType Property_CallType;
	GET_PROPERTY(CallType)->GetValue((uint8&)Property_CallType);

	return Property_CallType != EDialogPhraseEventCallType::DialogScript ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef GET_PROPERTY
#undef GET_PROPERTY_IN_TYPE