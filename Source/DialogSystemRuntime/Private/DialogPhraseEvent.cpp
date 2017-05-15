// Copyright 2017 Krivosheya Mikhail. All Rights Reserved.
#include "DialogSystemRuntime.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"
#include "DialogImplementer.h"
#include "DialogPhraseEvent.h"

#define ERROR(Message, ...) ErrorMessage =  FString::Printf(TEXT(Message), ##__VA_ARGS__); return false

bool FDialogPhraseEvent::Compile(FString& ErrorMessage, bool& needUpdate)
{
	if (EventName.IsNone())
	{
		ERROR("Event name is empty");
	}

	if (ObjectClass == NULL && CallType != EDialogPhraseEventCallType::DialogScript)
	{
		ERROR("Object class	is empty");
	}

	switch (CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		if (!OwnerNode || !OwnerNode->OwnerDialog)
			break;

		if (OwnerNode->OwnerDialog->DialogScriptClass == NULL)
		{
			ERROR("DialogScript not found, pleass select dialog script class in root node");
		}

		ObjectClass = OwnerNode->OwnerDialog->DialogScriptClass;
		break;
		
	case EDialogPhraseEventCallType::Player:
	case EDialogPhraseEventCallType::Interlocutor:
		break;

	case EDialogPhraseEventCallType::FindByTag:
		if (FindTag.IsEmpty())
		{
			ERROR("Find tag is empty");
		}

		if (!ObjectClass->IsChildOf(AActor::StaticClass()) && !ObjectClass->IsChildOf(UActorComponent::StaticClass()))
		{
			ERROR("Object class must be an actor or its component");
		}
		break;

	default:
		ERROR("Unknown call type");
	}

	auto func = ObjectClass->ClassDefaultObject->FindFunction(EventName);
	if (func == NULL)
	{
		ERROR("Function %s not found", *EventName.ToString());
	}

	TArray<UProperty*> params;
	for (TFieldIterator<UProperty> PropIt(func); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		params.Add(*PropIt);
	}

	if(params.Num() != Parameters.Num())
		needUpdate = true;

	while (params.Num() < Parameters.Num())
	{
		Parameters.RemoveAt(Parameters.Num() - 1);
	}
	while (params.Num() > Parameters.Num())
	{
		Parameters.Add("0");
	}

	Command = EventName.ToString();

	for (auto& p : Parameters)
	{
		Command.AppendChar(' ');
		Command.Append(p);
	}

	return true;
}

bool FDialogPhraseCondition::Compile(FString& ErrorMessage, bool& needUpdate)
{
	if (!Super::Compile(ErrorMessage, needUpdate))
		return false;

	auto func = ObjectClass->ClassDefaultObject->FindFunction(EventName);

	int found = 0;
	for (TFieldIterator<UProperty> It(func); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It)
	{
		if (It->IsA(UBoolProperty::StaticClass()))
			found++;
	}

	if (found == 0)
	{
		ERROR("Function %s does not return a boolean value", *EventName.ToString());
	}
	else if (found > 1)
	{
		ERROR("Function %s returns several Boolean values", *EventName.ToString());
	}

	return true;
}
#undef ERROR


UObject* FDialogPhraseEvent::GetObject(UDialogImplementer* Implementer) const
{
	UObject* obj = NULL;

	switch (CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		obj = Implementer->DialogScript;
		break;

	case EDialogPhraseEventCallType::Player:
		obj = UGameplayStatics::GetPlayerCharacter(Implementer->Interlocutor->GetWorld(), 0);
		break;

	case EDialogPhraseEventCallType::Interlocutor:
		obj = Implementer->Interlocutor;
		break;

	case EDialogPhraseEventCallType::FindByTag:
		for (FObjectIterator Itr(ObjectClass); Itr; ++Itr)
		{
			AActor* actor = Cast<AActor>(*Itr);
			if (actor != NULL)
			{
				if (actor->Tags.Contains(*FindTag))
				{
					obj = actor;
					break;
				}
			}
			UActorComponent* component = Cast<UActorComponent>(*Itr);
			if (component != NULL)
			{
				if (component->ComponentHasTag(*FindTag))
				{
					obj = component;
					break;
				}
			}
		}
		break;

	default:
		break;
	}

	if (obj == NULL)
		UE_LOG(DialogModuleLog, Warning, TEXT("Event terget not found in %s"), *Implementer->Asset->GetPathName());

	return obj;
}

void FDialogPhraseEvent::Invoke(UDialogImplementer* Implementer)
{
	auto obj = GetObject(Implementer);
	if (obj != NULL)
	{
		auto ar = FOutputDeviceRedirector::Get();
		obj->CallFunctionByNameWithArguments(*Command, *ar, obj, true);
	}
	else
		UE_LOG(DialogModuleLog, Error, TEXT("Object for function call not found"));
}

bool FDialogPhraseCondition::InvokeCheck(class UDialogImplementer* Implementer)
{
	auto obj = GetObject(Implementer);
	if (obj != NULL)
	{
		bool checkResult;

		if (!CallCheckFunction(obj, *Command, checkResult) || !checkResult)
		{
			return InvertCondition;
		}
	}
	else
		UE_LOG(DialogModuleLog, Error, TEXT("Object for function call not found"));

	return !InvertCondition;
}


// Copy from ScriptCore.cpp UObject::CallFunctionByNameWithArguments
bool FDialogPhraseCondition::CallCheckFunction(UObject* Executor, const TCHAR* Str, bool& checkResult)
{
	FString MsgStr;
	if (!FParse::Token(Str, MsgStr, true))
	{
		UE_LOG(DialogModuleLog, Error, TEXT("CallFunctionByNameWithArguments: Not Parsed '%s'"), Str);
		return false;
	}
	const FName Message = FName(*MsgStr, FNAME_Find);
	if (Message == NAME_None)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("CallFunctionByNameWithArguments: Name not found '%s'"), Str);
		return false;
	}
	UFunction* Function = Executor->FindFunction(Message);
	if (nullptr == Function)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("CallFunctionByNameWithArguments: Function not found '%s'"), Str);
		return false;
	}

	UProperty* LastParameter = nullptr;

	// find the last parameter
	for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags&(CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It)
	{
		LastParameter = *It;
	}

	// Parse all function parameters.
	uint8* Parms = (uint8*)FMemory_Alloca(Function->ParmsSize);
	FMemory::Memzero(Parms, Function->ParmsSize);

	const uint32 ExportFlags = PPF_Localized;
	bool Failed = 0;
	int32 NumParamsEvaluated = 0;
	for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It, NumParamsEvaluated++)
	{
		UProperty* PropertyParam = *It;
		checkSlow(PropertyParam); // Fix static analysis warning
		if (NumParamsEvaluated == 0 && Executor)
		{
			UObjectPropertyBase* Op = dynamic_cast<UObjectPropertyBase*>(*It);
			if (Op && Executor->IsA(Op->PropertyClass))
			{
				// First parameter is implicit reference to object executing the command.
				Op->SetObjectPropertyValue(Op->ContainerPtrToValuePtr<uint8>(Parms), Executor);
				continue;
			}
		}

		// Keep old string around in case we need to pass the whole remaining string
		const TCHAR* RemainingStr = Str;

		// Parse a new argument out of Str
		FString ArgStr;
		FParse::Token(Str, ArgStr, true);

		// if ArgStr is empty but we have more params to read parse the function to see if these have defaults, if so set them
		bool bFoundDefault = false;
		bool bFailedImport = true;
		if (!FCString::Strcmp(*ArgStr, TEXT("")))
		{
			const FName DefaultPropertyKey(*(FString(TEXT("CPP_Default_")) + PropertyParam->GetName()));
#if WITH_EDITOR
			const FString PropertyDefaultValue = Function->GetMetaData(DefaultPropertyKey);
#else
			const FString PropertyDefaultValue = TEXT("");
#endif
			if (!PropertyDefaultValue.IsEmpty())
			{
				bFoundDefault = true;

				const TCHAR* Result = It->ImportText(*PropertyDefaultValue, It->ContainerPtrToValuePtr<uint8>(Parms), ExportFlags, NULL);
				bFailedImport = (Result == nullptr);
			}
		}

		if (!bFoundDefault)
		{
			// if this is the last string property and we have remaining arguments to process, we have to assume that this
			// is a sub-command that will be passed to another exec (like "cheat giveall weapons", for example). Therefore
			// we need to use the whole remaining string as an argument, regardless of quotes, spaces etc.
			if (PropertyParam == LastParameter && PropertyParam->IsA<UStrProperty>() && FCString::Strcmp(Str, TEXT("")) != 0)
			{
				ArgStr = FString(RemainingStr).Trim();
			}

			const TCHAR* Result = It->ImportText(*ArgStr, It->ContainerPtrToValuePtr<uint8>(Parms), ExportFlags, NULL);
			bFailedImport = (Result == nullptr);
		}

		if (bFailedImport)
		{
			UE_LOG(DialogModuleLog, Error, TEXT("'%s': Bad or missing property '%s'"), *Message.ToString(), *It->GetName());
			Failed = true;

			break;
		}

	}

	if (!Failed)
	{
		Executor->ProcessEvent(Function, Parms);
	}

	//!!destructframe see also UObject::ProcessEvent
	for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It)
	{
		// Copy result in params
		if (UBoolProperty* BoolProperty = Cast<UBoolProperty>(*It))
		{
			checkResult = BoolProperty->GetPropertyValue(It->ContainerPtrToValuePtr<uint8>(Parms));
		}

		It->DestroyValue_InContainer(Parms);
	}

	// Success.
	return true;
}