#include "UObject/UnrealType.h"
#include "Widgets/SViewport.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "DialogEditorNodes.h"
#include "PhraseNodeCustomization.h"

#define LOCTEXT_NAMESPACE "DialogPhraseEventCustomization"

TSharedRef<IDetailCustomization> FPhraseNodeDetails::MakeInstance()
{
	return MakeShareable(new FPhraseNodeDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FPhraseNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	auto DataProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UDialogPhraseEdGraphNode, Data));

	uint32 DataPropertyCount;
	DataProperty->GetNumChildren(DataPropertyCount);

	DetailLayout.HideProperty(DataProperty);

	auto AutoTimeProperty = DataProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDialogPhraseInfo, AutoTime));
	bool isAutoTime;
	AutoTimeProperty->GetValue(isAutoTime);
	AutoTimeProperty->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([&DetailLayout]()
	{
		DetailLayout.ForceRefreshDetails();
	}));
	
	for (uint32 i = 0; i < DataPropertyCount; i++)
	{
		auto prop = DataProperty->GetChildHandle(i);
		auto name = prop->GetProperty()->GetName();

		auto& categoty = DetailLayout.EditCategory(FName(*prop->GetMetaData("Category")));

		if (name == "Text")
		{
			FText Text;
			prop->GetValue(Text);

			categoty.AddCustomRow(prop->GetPropertyDisplayName()).NameContent()
			[
				prop->CreatePropertyNameWidget()
			]
			.ValueContent()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.MinDesiredHeight(150)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SMultiLineEditableTextBox)
						.Text(Text)
						.AutoWrapText(true)
						.SelectAllTextOnCommit(false)
						.SelectAllTextWhenFocused(false)
						.ClearKeyboardFocusOnCommit(true)
						.OnTextCommitted_Lambda([prop](const FText& NewText, ETextCommit::Type Type) 
						{
							prop->SetValue(NewText);
						})
						.ModiferKeyForNewLine(EModifierKey::None)
					]
				]
			];
		}
		else if (name == "PhraseManualTime" && isAutoTime)
		{
			continue;
		}
		else
		{
			categoty.AddProperty(prop);
		}
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE