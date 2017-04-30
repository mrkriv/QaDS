// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#pragma once


#include "DialogPhrase.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class DIALOGSYSTEMEDITOR_API FDialogPhraseEventParamCustomization : public IPropertyTypeCustomization
{

public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FDialogPhraseEventParamCustomization();
	
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
private:
	TSharedPtr<IPropertyHandle> PropertyHandle_Name;
	TSharedPtr<IPropertyHandle> PropertyHandle_Type;
	TSharedPtr<IPropertyHandle> PropertyHandle_Value;
};