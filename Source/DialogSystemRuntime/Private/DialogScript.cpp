// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "DialogImplementer.h"
#include "DialogScript.h"

ADialogScript::ADialogScript()
{
}

AActor* ADialogScript::GetPlayer()
{
	return UGameplayStatics::GetPlayerCharacter(GetInterlocutor()->GetWorld(), 0);
}

AActor* ADialogScript::GetInterlocutor()
{
	return Implementer->Interlocutor;
}

void ADialogScript::SetTrigger()
{
	if (Trigger == true)
		return;

	Trigger = true;
	OnTriggerChange(Trigger);
}

void ADialogScript::ResetTrigger()
{
	if (Trigger == false)
		return;

	Trigger = false;
	OnTriggerChange(Trigger);
}

void ADialogScript::SetTriggerValue(bool IsSetNew)
{
	if (IsSetNew)
		SetTrigger();
	else
		ResetTrigger();
}

bool ADialogScript::GetTrigger()
{
	return Trigger;
}
