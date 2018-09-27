#pragma once

#include "QuestNode.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class DIALOGSYSTEMEDITOR_API FQuestStageEventCustomization : public IPropertyTypeCustomization
{
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();


	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	FReply OnTitleClick(); 

	FText GetTitleText() const;
	EVisibility GetFingTagVisibility() const;
	EVisibility GetObjectClassVisibility() const;
};