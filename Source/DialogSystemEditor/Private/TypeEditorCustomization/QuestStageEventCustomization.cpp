#include "DialogSystemEditor.h"
#include "QuestAsset.h"
#include "QuestScript.h"
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
#include "DetailCategoryBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SHyperlink.h"
#include "ScopedTransaction.h"
#include "Slate/SlateTextureAtlasInterface.h"
#include "QuestStageEventCustomization.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

TSharedRef<IPropertyTypeCustomization> FQuestStageEventCustomization::MakeInstance()
{
	return MakeShareable(new FQuestStageEventCustomization());
}

FQuestStageEventCustomization::FQuestStageEventCustomization()
{
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FQuestStageEventCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	static const FName PropertyName_ObjectClass = GET_MEMBER_NAME_CHECKED(FQuestStageEvent, ObjectClass);
	static const FName PropertyName_Parameters = GET_MEMBER_NAME_CHECKED(FQuestStageEvent, Parameters);
	static const FName PropertyName_EventName = GET_MEMBER_NAME_CHECKED(FQuestStageEvent, EventName);
	static const FName PropertyName_OwnerNode = GET_MEMBER_NAME_CHECKED(FQuestStageEvent, OwnerNode);
	static const FName PropertyName_CallType = GET_MEMBER_NAME_CHECKED(FQuestStageEvent, CallType);
	static const FName PropertyName_FindTag = GET_MEMBER_NAME_CHECKED(FQuestStageEvent, FindTag);
	static const FName PropertyName_Invert = GET_MEMBER_NAME_CHECKED(FQuestStageCondition, InvertCondition);

	PropertyHandle_ObjectClass = StructPropertyHandle->GetChildHandle(PropertyName_ObjectClass);
	PropertyHandle_Parameters = StructPropertyHandle->GetChildHandle(PropertyName_Parameters);
	PropertyHandle_EventName = StructPropertyHandle->GetChildHandle(PropertyName_EventName);
	PropertyHandle_OwnerNode = StructPropertyHandle->GetChildHandle(PropertyName_OwnerNode);
	PropertyHandle_CallType = StructPropertyHandle->GetChildHandle(PropertyName_CallType);
	PropertyHandle_FindTag = StructPropertyHandle->GetChildHandle(PropertyName_FindTag);
	PropertyHandle_Invert = StructPropertyHandle->GetChildHandle(PropertyName_Invert);
	PropertyHandle_PhraseEvent = StructPropertyHandle;

	check(PropertyHandle_ObjectClass.IsValid());
	check(PropertyHandle_Parameters.IsValid());
	check(PropertyHandle_OwnerNode.IsValid());
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
			.OnClicked(this, &FQuestStageEventCustomization::OnTitleClick)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(this, &FQuestStageEventCustomization::GetTitleText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			]
		];
}

void FQuestStageEventCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructBuilder.AddProperty(PropertyHandle_CallType.ToSharedRef());
	StructBuilder.AddProperty(PropertyHandle_EventName.ToSharedRef());

	StructBuilder.AddProperty(PropertyHandle_ObjectClass.ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FQuestStageEventCustomization::GetObjectClassVisibility));

	StructBuilder.AddProperty(PropertyHandle_FindTag.ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FQuestStageEventCustomization::GetFingTagVisibility));

	if (PropertyHandle_Invert.IsValid())
		StructBuilder.AddProperty(PropertyHandle_Invert.ToSharedRef());

	UObject* Property_ObjectClass;
	FName Property_EventName;

	PropertyHandle_EventName->GetValue(Property_EventName);
	PropertyHandle_ObjectClass->GetValue(Property_ObjectClass);

	if (Property_ObjectClass == NULL)
		return;

	auto func = Cast<UClass>(Property_ObjectClass)->ClassDefaultObject->FindFunction(Property_EventName);
	auto array = PropertyHandle_Parameters->AsArray();

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

FReply FQuestStageEventCustomization::OnTitleClick()
{
	UObject* Property_ObjectClass;
	UObject* Property_OwnerNode;
	EQuestStageEventCallType Property_CallType;

	PropertyHandle_ObjectClass->GetValue(Property_ObjectClass);
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);
	PropertyHandle_OwnerNode->GetValue(Property_OwnerNode);

	if (Property_CallType == EQuestStageEventCallType::QuestScript && Property_OwnerNode != NULL)
	{
		auto node = Cast<UQuestNode>(Property_OwnerNode);

		if (node == NULL || node->OwnerQuest == NULL || !node->OwnerQuest->QuestScriptClass.IsValid())
			return FReply::Handled();

		auto sc = Cast<UQuestNode>(Property_OwnerNode)->OwnerQuest->QuestScriptClass.Get();

		if (sc != NULL)
			FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *sc->GetPathName() }));
	}
	else if (Property_ObjectClass != NULL)
	{
		FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *Property_ObjectClass->GetPathName() }));
	}

	return FReply::Handled();
}

FText FQuestStageEventCustomization::GetTitleText() const
{
	FQuestStageEvent phraseEvent;
	PropertyHandle_PhraseEvent->GetValue((uint8&)phraseEvent);

	return FText::FromString(phraseEvent.ToString());
}

EVisibility FQuestStageEventCustomization::GetFingTagVisibility() const
{
	EQuestStageEventCallType Property_CallType;
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);

	return Property_CallType == EQuestStageEventCallType::FindByTag ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FQuestStageEventCustomization::GetObjectClassVisibility() const
{
	EQuestStageEventCallType Property_CallType;
	PropertyHandle_CallType->GetValue((uint8&)Property_CallType);

	return Property_CallType != EQuestStageEventCallType::QuestScript ? EVisibility::Visible : EVisibility::Collapsed;
}