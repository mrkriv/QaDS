#include "DialogSystemRuntime.h"
#include "EdGraph/EdGraphPin.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "StoryInformationManager.h"
#include "DialogImplementer.h"
#include "DialogPhrase.h"


void UDialogNode::Invoke(class UDialogImplementer* Implementer)
{
}

bool UDialogNode::Check(UDialogImplementer* Implementer) const
{
	return false;
}

TArray<UDialogNode*> UDialogNode::GetChilds()
{
	return Childs;
}

void UDialogNode::AddEvent(TArray<FDialogPhraseEvent>* Array, const FDialogPhraseEvent& Event)
{
	auto ev = Event;
	ev.OwnerNode = this;
	Array->Add(ev);
}

void UDialogNode::AddCondition(TArray<FDialogPhraseCondition>* Array, const FDialogPhraseCondition& Event)
{
	auto cond = Event;
	cond.OwnerNode = this;
	Array->Add(cond);
}

bool FDialogPhraseEvent::Check(FString& ErrorMessage) const
{
#define ERROR(Message) ErrorMessage = TEXT(Message); return false

	if (EventName.IsNone())
		ERROR("Event name is empty");

	switch (CallType)
	{
	case EDialogPhraseEventCallType::Player:
		break;
	case EDialogPhraseEventCallType::Interlocutor:
		break;
	case EDialogPhraseEventCallType::DialogScript:
		break;
		
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
#undef ERROR
}

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

bool FDialogPhraseCondition::Check(FString& ErrorMessage) const
{
	return Super::Check(ErrorMessage);
}


void UDialogPhrase::Invoke(UDialogImplementer* Implementer)
{
	for (auto key : GiveKeys)
		UStoryKeyManager::AddKey(key);

	for (auto key : RemoveKeys)
		UStoryKeyManager::RemoveKey(key);
	
	if (UID.IsValid())
		UStoryKeyManager::AddKey(*(Implementer->Asset->Name.ToString() + UID.ToString()), EStoryKeyTypes::DialogPhrases);

	for (auto& Event : CustomEvents)
	{
		auto obj = Event.GetObject(Implementer);
		if (obj != NULL)
		{
			auto func = obj->FindFunction(Event.EventName);
			if (func != NULL)
			{
				if (func->NumParms != 0)
				{
					FTimerDynamicDelegate Delegate;
					Delegate.BindUFunction(obj, Event.EventName);
					Delegate.Execute();
				}
				else
					UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s do not have input parameters and not return value"), *obj->GetName(), *Event.EventName.ToString())
			}
			else
				UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s not found"), *obj->GetName(), *Event.EventName.ToString())
		}
	}
}

bool UDialogPhrase::Check(UDialogImplementer* Implementer) const
{
	for (auto key : CheckHasKeys)
	{
		if (UStoryKeyManager::DontHasKey(key))
			return false;
	}

	for (auto key : CheckDontHasKeys)
	{
		if (UStoryKeyManager::HasKey(key))
			return false;
	}

	for (auto& Conditions : CustomConditions)
	{
		auto obj = Conditions.GetObject(Implementer);
		if (obj != NULL)
		{
			auto func = obj->FindFunction(Conditions.EventName);
			if (func != NULL)
			{
				if (func->NumParms == 1 && func->Children[0].IsA(UBoolProperty::StaticClass()))
				{
					FDialogConditionDelegate Delegate;
					Delegate.BindUFunction(obj, Conditions.EventName);

					if (!Delegate.Execute())
						return false;
				}
				else
					UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s must return only a Boolean value and do not have input parameters"), *obj->GetName(), *Conditions.EventName.ToString())
			}
			else
				UE_LOG(DialogModuleLog, Error, TEXT("Function %s.%s not found"), *obj->GetName(), *Conditions.EventName.ToString())
		}
	}

	return true;
}