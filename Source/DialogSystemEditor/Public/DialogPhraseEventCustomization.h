#pragma once

#include "DialogPhrase.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class DIALOGSYSTEMEDITOR_API FDialogPhraseEventCustomization : public IPropertyTypeCustomization
{

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FDialogPhraseEventCustomization();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	FText GetTitleText() const;
	EVisibility GetFingTagVisibility() const;
	EVisibility GetLevelNameVisibility() const;
	EVisibility GetParametersVisibility() const;
	EVisibility GetObjectClassVisibility() const;

	FReply OnTitleClick();

private:
	TSharedPtr<IPropertyHandle> PropertyHandle_ObjectClass;
	TSharedPtr<IPropertyHandle> PropertyHandle_EventName;
	TSharedPtr<IPropertyHandle> PropertyHandle_CallType;
	TSharedPtr<IPropertyHandle> PropertyHandle_FindTag;
	TSharedPtr<IPropertyHandle> PropertyHandle_OwnerNode;
	TSharedPtr<IPropertyHandle> PropertyHandle_Parameters;
	TSharedPtr<IPropertyHandle> PropertyHandle_Invert;
};