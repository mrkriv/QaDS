#pragma once

#include "DialogPhrase.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class DIALOGSYSTEMEDITOR_API FDialogPhraseEventCustomization : public IPropertyTypeCustomization
{
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	FReply OnTitleClick();

	EVisibility GetFingTagVisibility() const;
	EVisibility GetObjectClassVisibility() const;
	FText GetTitleText() const;
};