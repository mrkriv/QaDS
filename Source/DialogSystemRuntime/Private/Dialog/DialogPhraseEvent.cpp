#include "DialogSystemRuntime.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "DialogProcessor.h"
#include "DialogNodes.h"
#include "DialogAsset.h"
#include "DialogPhraseEvent.h"

bool FDialogPhraseEvent::Compile(FString& ErrorMessage)
{
	if (EventName.IsNone())
	{
		ErrorMessage = FString::Printf(TEXT("Event name is empty"));
		return false;
	}

	if (ObjectClass == NULL && CallType != EDialogPhraseEventCallType::DialogScript)
	{
		ErrorMessage = FString::Printf(TEXT("Object classis empty"));
		return false;
	}

	switch (CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		if (!OwnerNode || !OwnerNode->OwnerDialog)
			break;

		if (!OwnerNode->OwnerDialog->DialogScriptClass.IsValid())
		{
			ErrorMessage = FString::Printf(TEXT("DialogScript not found, pleass select dialog script class in root node"));
			return false;
		}

		ObjectClass = OwnerNode->OwnerDialog->DialogScriptClass.Get();
		break;
		
	case EDialogPhraseEventCallType::Player:
	case EDialogPhraseEventCallType::NPC:
		break;

	case EDialogPhraseEventCallType::FindByTag:
		if (FindTag.IsEmpty())
		{
			ErrorMessage = FString::Printf(TEXT("Find tag is empty"));
			return false;
		}

		if (!ObjectClass->IsChildOf(AActor::StaticClass()) && !ObjectClass->IsChildOf(UActorComponent::StaticClass()))
		{
			ErrorMessage = FString::Printf(TEXT("Object class must be an actor or its component"));
			return false;
		}
		break;

	default:
		ErrorMessage = FString::Printf(TEXT("Unknown call type"));
		return false;
	}

	auto func = ObjectClass->ClassDefaultObject->FindFunction(EventName);
	if (func == NULL)
	{
		ErrorMessage = FString::Printf(TEXT("Function %s not found"), *EventName.ToString());
		return false;
	}

	TArray<UProperty*> params;
	for (TFieldIterator<UProperty> PropIt(func); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		params.Add(*PropIt);
	}

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

bool FDialogPhraseCondition::Compile(FString& ErrorMessage)
{
	if (!Super::Compile(ErrorMessage))
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
		ErrorMessage = FString::Printf(TEXT("Function %s does not return a boolean value"), *EventName.ToString());
		return false;
	}
	else if (found > 1)
	{
		ErrorMessage = FString::Printf(TEXT("Function %s returns several Boolean values"), *EventName.ToString());
		return false;
	}

	return true;
}


UObject* FDialogPhraseEvent::GetObject(UDialogProcessor* DialogProcessor) const
{
	UObject* obj = NULL;

	switch (CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		obj = Cast<UObject>(DialogProcessor->DialogScript);
		break;

	case EDialogPhraseEventCallType::Player:
		obj = UGameplayStatics::GetPlayerCharacter(DialogProcessor->NPC->GetWorld(), 0);
		break;

	case EDialogPhraseEventCallType::NPC:
		obj = DialogProcessor->NPC;
		break;

	case EDialogPhraseEventCallType::FindByTag:
		for (FObjectIterator Itr(ObjectClass); Itr; ++Itr)
		{
			auto actor = Cast<AActor>(*Itr);
			if (actor != NULL)
			{
				if (actor->Tags.Contains(*FindTag))
				{
					obj = actor;
					break;
				}
			}

			auto component = Cast<UActorComponent>(*Itr);
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
		UE_LOG(DialogModuleLog, Warning, TEXT("Event terget not found in %s"), *DialogProcessor->Asset->GetPathName());

	return obj;
}

void FDialogPhraseEvent::Invoke(UDialogProcessor* DialogProcessor)
{
	auto obj = GetObject(DialogProcessor);
	if (obj != NULL)
	{ 
		auto ar = FOutputDeviceRedirector::Get();
		obj->CallFunctionByNameWithArguments(*Command, *ar, obj, true);
	}
	else
		UE_LOG(DialogModuleLog, Error, TEXT("Object for function call not found"));
}

FString FDialogPhraseEvent::ToString() const
{
	auto funcName = EventName.ToString() + "(";

	for (auto i = 0; i < Parameters.Num(); i++)
	{
		if (i != 0)
			funcName.Append(", ");

		funcName.Append(Parameters[i]);
	}

	funcName += ")";

	switch (CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		return funcName;

	case EDialogPhraseEventCallType::Player:
		return TEXT("Player.") + funcName;

	case EDialogPhraseEventCallType::NPC:
		return TEXT("NPC.") + funcName;

	case EDialogPhraseEventCallType::FindByTag:
		if (ObjectClass)
			return ObjectClass->GetName() + TEXT("[") + FindTag + TEXT("].") + funcName;
		break;
	}

	return TEXT("None");
}

bool FDialogPhraseCondition::InvokeCheck(class UDialogProcessor* DialogProcessor) const
{
	auto obj = GetObject(DialogProcessor);

	if (obj == NULL)
	{
		UE_LOG(DialogModuleLog, Error, TEXT("Object for function call not found"));
		return false;
	}

	bool checkResult;
	if (!CallCheckFunction(obj, *Command, checkResult) || !checkResult)
	{
		return InvertCondition;
	}

	return !InvertCondition;
}

FString FDialogPhraseCondition::ToString() const
{
	auto baseText = FDialogPhraseEvent::ToString();

	if (InvertCondition)
		return "NOT(" + baseText + ")";

	return baseText;
}

// Copy from ScriptCore.cpp UObject::CallFunctionByNameWithArguments
bool FDialogPhraseCondition::CallCheckFunction(UObject* Executor, const TCHAR* Str, bool& checkResult) const
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

	bool Failed = 0;
	int32 NumParamsEvaluated = 0;
	for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It, NumParamsEvaluated++)
	{
		UProperty* PropertyParam = *It;
		checkSlow(PropertyParam); // Fix static analysis warning
		if (NumParamsEvaluated == 0 && Executor)
		{
			FObjectPropertyBase* Op = CastField<FObjectPropertyBase>(*It);
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

				const TCHAR* Result = It->ImportText(*PropertyDefaultValue, It->ContainerPtrToValuePtr<uint8>(Parms), 0, NULL);
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
				ArgStr = FString(RemainingStr).TrimStart();
			}

			const TCHAR* Result = It->ImportText(*ArgStr, It->ContainerPtrToValuePtr<uint8>(Parms), 0, NULL);
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
