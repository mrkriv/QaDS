#pragma once

#include "QuestNode.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class DIALOGSYSTEMEDITOR_API FQuestStageEventCustomization : public IPropertyTypeCustomization
{

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FQuestStageEventCustomization();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	FText GetTitleText() const;
	EVisibility GetFingTagVisibility() const;
	EVisibility GetObjectClassVisibility() const;

private:
	FReply OnTitleClick();

	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	TSharedPtr<IPropertyHandle> PropertyHandle_ObjectClass;
	TSharedPtr<IPropertyHandle> PropertyHandle_EventName;
	TSharedPtr<IPropertyHandle> PropertyHandle_CallType;
	TSharedPtr<IPropertyHandle> PropertyHandle_FindTag;
	TSharedPtr<IPropertyHandle> PropertyHandle_OwnerNode;
	TSharedPtr<IPropertyHandle> PropertyHandle_Parameters;
	TSharedPtr<IPropertyHandle> PropertyHandle_Invert;
};