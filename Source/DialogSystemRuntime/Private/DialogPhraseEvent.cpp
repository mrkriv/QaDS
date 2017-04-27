#include "DialogSystemRuntime.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"
#include "DialogImplementer.h"
#include "DialogPhraseEvent.h"

//#define ERROR(Message) ErrorMessage = TEXT(Message); return false
#define ERROR(Message, ...) ErrorMessage =  FString::Printf(TEXT(Message), ##__VA_ARGS__); return false

bool FDialogPhraseEvent::Check(FString& ErrorMessage)
{
	if (EventName.IsNone())
	{
		ERROR("Event name is empty");
	}

	switch (CallType)
	{
	case EDialogPhraseEventCallType::Player:
		break;
	case EDialogPhraseEventCallType::Interlocutor:
		break;

	case EDialogPhraseEventCallType::DialogScript:
		if (!OwnerNode || !OwnerNode->OwnerDialog)
			break;

		if (OwnerNode->OwnerDialog->DialogScriptClass == NULL)
		{
			ERROR("DialogScript not found, pleass select dialog script class in root node");
		}
		
		return CompileParametrs(OwnerNode->OwnerDialog->DialogScriptClass->GetDefaultObject(), ErrorMessage);
		
	case EDialogPhraseEventCallType::CreateNew:
		if (ObjectClass == NULL)
		{
			ERROR("Object class	is empty");
		}
		if (ObjectClass->IsChildOf(AActor::StaticClass()))
		{
			ERROR("Object class can not be an actor");
		}
		break;

	case EDialogPhraseEventCallType::FindByTag:

		if (ObjectClass == NULL)
		{
			ERROR("Object class	is empty");
		}

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

	return true;
}

bool FDialogPhraseEvent::CompileParametrs(UObject* Object, FString& ErrorMessage)
{
	ParameterData.Reset();

	if (Object == NULL)
	{
		ERROR("Target object not found");
	}

	auto func = Object->FindFunction(EventName);
	if (func == NULL)
	{
		ERROR("%s have not function %s", *Object->GetName(), *EventName.ToString());
	}

	TMap<UProperty*, FDialogPhraseEventParam> params;

	for (auto& param : Parameters)
	{
		auto prop = func->PropertyLink;
		while (prop != NULL)
		{
			if (prop->ArrayDim != 1)
			{
				ERROR("Array argument not supported. (%s.%s in %s)", *EventName.ToString(), *prop->GetName(), *Object->GetName());
			}

			if (param.Name == prop->GetName())
				params.Add(prop, param);

			prop = prop->PropertyLinkNext;
		}
	}

	if (params.Num() != func->NumParms || Parameters.Num() != func->NumParms)
	{
		ERROR("Invalid function signature %s in %s", *EventName.ToString(), *Object->GetName());
	}

	auto prop = func->PropertyLink;
	while (prop != NULL)
	{
		if (!params[prop].Compile(prop, ParameterData, ErrorMessage))
			return false;

		prop = prop->PropertyLinkNext;
	}

	return true;
}

bool FDialogPhraseEventParam::Compile(const UProperty* Property, TArray<uint8>& ParameterData, FString& ErrorMessage) const
{
	return true;
}

#undef ERROR



UObject* FDialogPhraseEvent::GetObject(UDialogImplementer* Implementer) const
{
	UObject* obj = NULL;

	switch (CallType)
	{
	case EDialogPhraseEventCallType::DialogScript:
		obj = Implementer->Asset->DialogScript;
		break;

	case EDialogPhraseEventCallType::Player:
		obj = UGameplayStatics::GetPlayerCharacter(Implementer->Interlocutor->GetWorld(), 0);
		break;

	case EDialogPhraseEventCallType::Interlocutor:
		obj = Implementer->Interlocutor;
		break;

	case EDialogPhraseEventCallType::CreateNew:
		obj = NewObject<UObject>(Implementer, ObjectClass);
		obj->PostLoad();
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

void FDialogPhraseEvent::Invoke(UDialogImplementer* Implementer) const
{
	auto obj = GetObject(Implementer);
	if (obj != NULL)
	{
		auto func = obj->FindFunction(EventName);

		if (func != NULL)
		{
			if (func->ParmsSize == ParameterData.Num())
			{
				Implementer->ProcessEvent(func, const_cast<uint8*>(ParameterData.GetData()));

				//FTimerDynamicDelegate Delegate;
				//Delegate.BindUFunction(obj, EventName);
				//Delegate.Execute();
			}
			else
			{
				UE_LOG(DialogModuleLog, Error, TEXT("Invalid function signature %s.%s. Please recompile '%s' dialog"), *obj->GetName(), *EventName.ToString(), *Implementer->Asset->GetName())
				UE_LOG(DialogModuleLog, Error, TEXT("Expected signature:"))

				if (Parameters.Num() != 0)
				{
					for (auto param : Parameters)
						UE_LOG(DialogModuleLog, Error, TEXT("%s = '%s'"), *param.Name, *param.Value)
				}
				else
					UE_LOG(DialogModuleLog, Error, TEXT("Not have argument"))
			}
		}
		else
			UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s not found"), *obj->GetName(), *EventName.ToString())
	}
}


bool FDialogPhraseCondition::Check(FString& ErrorMessage)
{
	return Super::Check(ErrorMessage);
}

bool FDialogPhraseCondition::InvokeCheck(class UDialogImplementer* Implementer) const
{
	auto obj = GetObject(Implementer);
	if (obj != NULL)
	{
		auto func = obj->FindFunction(EventName);
		if (func != NULL)
		{
			if (func->NumParms == 1 && func->Children[0].IsA(UBoolProperty::StaticClass()))
			{
				FDialogConditionDelegate Delegate;
				Delegate.BindUFunction(obj, EventName);

				if (!Delegate.Execute())
					return InvertCondition;
			}
			else
				UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s must return only a Boolean value and do not have input parameters"), *obj->GetName(), *EventName.ToString())
		}
		else
			UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s not found"), *obj->GetName(), *EventName.ToString())
	}

	return !InvertCondition;
}