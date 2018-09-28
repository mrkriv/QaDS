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

#define GET_PROPERTY(Property) GET_PROPERTY_IN_TYPE(FQuestStageEvent, Property)
#define GET_PROPERTY_IN_TYPE(Type, Property) StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(Type, Property))

TSharedRef<IPropertyTypeCustomization> FQuestStageEventCustomization::MakeInstance()
{
	return MakeShareable(new FQuestStageEventCustomization());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FQuestStageEventCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> structPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
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
	StructBuilder.AddProperty(GET_PROPERTY(CallType).ToSharedRef());
	StructBuilder.AddProperty(GET_PROPERTY(EventName).ToSharedRef());

	StructBuilder.AddProperty(GET_PROPERTY(ObjectClass).ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FQuestStageEventCustomization::GetObjectClassVisibility));

	StructBuilder.AddProperty(GET_PROPERTY(FindTag).ToSharedRef())
		.Visibility(TAttribute<EVisibility>(this, &FQuestStageEventCustomization::GetFingTagVisibility));

	if (GET_PROPERTY_IN_TYPE(FQuestStageCondition, InvertCondition).IsValid())
		StructBuilder.AddProperty(GET_PROPERTY_IN_TYPE(FQuestStageCondition, InvertCondition).ToSharedRef());

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

FReply FQuestStageEventCustomization::OnTitleClick()
{
	UObject* Property_ObjectClass;
	GET_PROPERTY(ObjectClass)->GetValue(Property_ObjectClass);
	
	if (Property_ObjectClass != NULL)
	{
		FAssetEditorManager::Get().OpenEditorsForAssets(TArray<FName>({ *Property_ObjectClass->GetPathName() }));
	}

	return FReply::Handled();
}

FText FQuestStageEventCustomization::GetTitleText() const
{
	FQuestStageEvent stageEvent;
	StructPropertyHandle->GetValue((uint8&)stageEvent);

	return FText::FromString(stageEvent.ToString());
}

EVisibility FQuestStageEventCustomization::GetFingTagVisibility() const
{
	EQuestStageEventCallType Property_CallType;
	GET_PROPERTY(CallType)->GetValue((uint8&)Property_CallType);

	return Property_CallType == EQuestStageEventCallType::FindByTag ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility FQuestStageEventCustomization::GetObjectClassVisibility() const
{
	EQuestStageEventCallType Property_CallType;
	GET_PROPERTY(CallType)->GetValue((uint8&)Property_CallType);

	return Property_CallType != EQuestStageEventCallType::QuestScript ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef GET_PROPERTY
#undef GET_PROPERTY_IN_TYPE