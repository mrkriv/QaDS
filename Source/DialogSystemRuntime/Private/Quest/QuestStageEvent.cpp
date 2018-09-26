#include "DialogSystemRuntime.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"
#include "QuestProcessor.h"
#include "QuestNode.h"
#include "QuestAsset.h"
#include "QuestScript.h"
#include "QuestStageEvent.h"

bool FQuestStageEvent::Compile(FString& ErrorMessage, bool& needUpdate)
{
	if (EventName.IsNone())
	{
		ErrorMessage = FString::Printf(TEXT("Event name is empty"));
		return false;
	}

	if (ObjectClass == NULL && CallType != EQuestStageEventCallType::QuestScript)
	{
		ErrorMessage = FString::Printf(TEXT("Object classis empty"));
		return false;
	}

	switch (CallType)
	{
	case EQuestStageEventCallType::QuestScript:
		if (!OwnerNode || !OwnerNode->OwnerQuest)
			break;

		if (!OwnerNode->OwnerQuest->QuestScriptClass.IsValid())
		{
			ErrorMessage = FString::Printf(TEXT("QuestScript not found, pleass select Quest script class in root node"));
			return false;
		}

		ObjectClass = OwnerNode->OwnerQuest->QuestScriptClass.Get();
		break;
		
	case EQuestStageEventCallType::Player:
		break;

	case EQuestStageEventCallType::FindByTag:
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

bool FQuestStageCondition::Compile(FString& ErrorMessage, bool& needUpdate)
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

UObject* FQuestStageEvent::GetObject(UQuestNode* QuestNode) const
{
	UObject* obj = NULL;
	auto quest = QuestNode->OwnerQuest;

	switch (CallType)
	{
	case EQuestStageEventCallType::QuestScript:
		obj = Cast<UObject>(quest->QuestScript);
		break;

	case EQuestStageEventCallType::Player:
		if (quest->QuestScript == NULL)
		{
			UE_LOG(DialogModuleLog, Warning, TEXT("Need QuestScript for EQuestStageEventCallType::Player in %s"), *quest->GetPathName());
			return NULL;
		}

		obj = UGameplayStatics::GetPlayerCharacter(quest->QuestScript->GetWorld(), 0);
		break;

	case EQuestStageEventCallType::FindByTag:
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
		UE_LOG(DialogModuleLog, Warning, TEXT("Event terget not found in %s"), *quest->GetPathName());

	return obj;
}

void FQuestStageEvent::Invoke(UQuestNode* QuestNode)
{
	auto obj = GetObject(QuestNode);
	if (obj != NULL)
	{ 
		auto ar = FOutputDeviceRedirector::Get();
		obj->CallFunctionByNameWithArguments(*Command, *ar, obj, true);
	}
	else
		UE_LOG(DialogModuleLog, Error, TEXT("Object for function call not found"));
}

FString FQuestStageEvent::ToString() const
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
	case EQuestStageEventCallType::QuestScript:
		return funcName;

	case EQuestStageEventCallType::Player:
		return TEXT("Player.") + funcName;

	case EQuestStageEventCallType::FindByTag:
		if (ObjectClass)
			return ObjectClass->GetName() + TEXT("[") + FindTag + TEXT("].") + funcName;
		break;
	}

	return TEXT("None");
}

bool FQuestStageCondition::InvokeCheck(UQuestNode* QuestNode) const
{
	auto obj = GetObject(QuestNode);

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

FString FQuestStageCondition::ToString() const
{
	auto baseText = FQuestStageEvent::ToString();

	if (InvertCondition)
		return "NOT(" + baseText + ")";

	return baseText;
}

// Copy from ScriptCore.cpp UObject::CallFunctionByNameWithArguments
bool FQuestStageCondition::CallCheckFunction(UObject* Executor, const TCHAR* Str, bool& checkResult) const
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
