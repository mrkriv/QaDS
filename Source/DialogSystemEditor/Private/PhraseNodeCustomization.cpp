#include "DialogSystemEditor.h"
#include "UObject/UnrealType.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SViewport.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "Viewports.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "IDetailsView.h"
#include "Slate/SceneViewport.h"
#include "DialogEditorNodes.h"
#include "PhraseNodeCustomization.h"
#include "DetailLayoutBuilder.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

#define LOCTEXT_NAMESPACE "DialogPhraseEventCustomization"

#define PROPERTY(NAME) Layout->GetProperty(GET_MEMBER_NAME_CHECKED(UPhraseNode, NAME))

TSharedRef<IDetailCustomization> FPhraseNodeDetails::MakeInstance()
{
	return MakeShareable(new FPhraseNodeDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FPhraseNodeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	Layout = &DetailLayout;

	TextProperty = TSharedPtr<IPropertyHandle>(PROPERTY(Text));
	Layout->HideProperty(TextProperty);

	FText Text;
	TextProperty->GetValue(Text);

	IDetailCategoryBuilder& mainCategoty = Layout->EditCategory("PhraseNode");

	mainCategoty.AddCustomRow(TextProperty->GetPropertyDisplayName()).NameContent()
	[
		TextProperty->CreatePropertyNameWidget()
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
				//.Font(InArgs._Font)
				.SelectAllTextWhenFocused(false)
				.ClearKeyboardFocusOnCommit(true)
				.OnTextCommitted(this, &FPhraseNodeDetails::OnTextCommitted)
				.OnTextChanged(this, &FPhraseNodeDetails::OnTextChanged)
				.SelectAllTextOnCommit(false)
				.AutoWrapText(true)
				.ModiferKeyForNewLine(EModifierKey::Shift)
			]
		]
	];

	auto AutoTimeProperty = PROPERTY(AutoTime);
	bool isAutoTime;
	AutoTimeProperty->GetValue(isAutoTime);
	AutoTimeProperty->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FPhraseNodeDetails::OnForceRefreshDetails));

	if (isAutoTime)
		Layout->HideProperty(PROPERTY(PhraseManualTime));
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FPhraseNodeDetails::OnTextCommitted(const FText& NewText, ETextCommit::Type Type)
{
	TextProperty->SetValue(NewText);
}

void FPhraseNodeDetails::OnTextChanged(const FText& NewText)
{
	TextProperty->SetValue(NewText);
}

void FPhraseNodeDetails::OnForceRefreshDetails()
{
	if (Layout != NULL)
		Layout->ForceRefreshDetails();
}

#undef LOCTEXT_NAMESPACE