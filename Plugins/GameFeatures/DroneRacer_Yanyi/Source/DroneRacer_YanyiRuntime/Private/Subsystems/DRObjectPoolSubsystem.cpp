// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DRObjectPoolSubsystem.h"
#include "DRPoolableInterface.h"

void UDRObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// UE_LOG(LogTemp, Warning, TEXT("Initialize DRObjectPoolSubsystem."));
}

void UDRObjectPoolSubsystem::RegisterPoolableClass(UClass* ObjectClass, int InitialNum)
{
	if (!ObjectClass->ImplementsInterface(UDRPoolableInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Object Type %s hasn't Implement Poolable Interface!"), *ObjectClass->GetName());
		return;
	}

	if (ObjectPool.Contains(ObjectClass->GetName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Poolable Object Class %s has registed!"), *ObjectClass->GetName());
		return;
	}

	FPoolableObjectList ObjectList;

	for (int i = 0; i < InitialNum; ++i)
	{
		AActor* SpawnedActor = SpawnNewObjectForClass(ObjectClass);

		if (IsValid(SpawnedActor))
		{
			IDRPoolableInterface* PoolableInterface = Cast<IDRPoolableInterface>(SpawnedActor);
			PoolableInterface->Execute_OnDeActive(SpawnedActor);

			ObjectList.PoolableObjects.Push(SpawnedActor);
		}
	}

	ObjectPool.Add(ObjectClass->GetName(), ObjectList);

	UE_LOG(LogTemp, Warning, TEXT("Register Poolable Class %s."), *ObjectClass->GetName());
}

AActor* UDRObjectPoolSubsystem::GetObjectOfType(UClass* ObjectClass)
{
	AActor* PoolableObject = nullptr;

	// Find the Object List of the Target Object Class
	if (ObjectPool.Contains(ObjectClass->GetName()))
	{
		for (auto IdleObject : ObjectPool[ObjectClass->GetName()].PoolableObjects)
		{
			if (IdleObject == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Object of type %s in object pool is invalid"), *ObjectClass->GetName());
			}

			IDRPoolableInterface* PoolableInterface = Cast<IDRPoolableInterface>(IdleObject);
			// Find a Idle Object
			if (IdleObject && !PoolableInterface->Execute_IsBussy(IdleObject))
			{
				PoolableObject = IdleObject;
				break;
			}
		}

		if (!PoolableObject) // Try to create an new Idle Object
		{
			PoolableObject = SpawnNewObjectForClass(ObjectClass);

			IDRPoolableInterface* PoolableInterface = Cast<IDRPoolableInterface>(PoolableObject);
			ObjectPool[ObjectClass->GetName()].PoolableObjects.Push(PoolableObject);
		}
	}
	else if(ObjectClass->ImplementsInterface(UDRPoolableInterface::StaticClass()))
	{
		PoolableObject = SpawnNewObjectForClass(ObjectClass);
		IDRPoolableInterface* PoolableInterface = Cast<IDRPoolableInterface>(PoolableObject);

		FPoolableObjectList NewPoolableObjectList;
		NewPoolableObjectList.PoolableObjects.Add(PoolableObject);
		ObjectPool.Add(ObjectClass->GetName(), NewPoolableObjectList);
	}

	if (!PoolableObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get Idle Object of Type %s Failed!"), *ObjectClass->GetName());
	}

	return PoolableObject;
}

bool UDRObjectPoolSubsystem::ReturnToPool(AActor* PoolableObject)
{
	if (!PoolableObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Do not return Invalid Pointer to the Object Pool."));
		return false;
	}

	UClass* ObjectClass = PoolableObject->GetClass();

	IDRPoolableInterface* PoolableInterface = Cast<IDRPoolableInterface>(PoolableObject);
	if (!PoolableInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("Return Un-Poolable Object to Object Pool!"));
		return false;
	}

	PoolableInterface->Execute_OnDeActive(PoolableObject);

	if (ObjectPool.Contains(ObjectClass->GetName()))
	{
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Return Poolable Object %s to Object Pool Failed!"), *PoolableObject->GetName());
	return false;
}

AActor* UDRObjectPoolSubsystem::SpawnNewObjectForClass(UClass* ObjectClass)
{
	FActorSpawnParameters* SpawnParameters = new FActorSpawnParameters;
	SpawnParameters->SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FString PoolableObjectName = ObjectClass->GetName() + "_" + FString::FromInt(ObjectPool.Contains(ObjectClass->GetName()) ? ObjectPool[ObjectClass->GetName()].PoolableObjects.Num() : 0);
	AActor* PoolableObject = GetWorld()->SpawnActor<AActor>(ObjectClass, FVector::ZeroVector, FRotator::ZeroRotator, *SpawnParameters);
	if (PoolableObject)
	{
		PoolableObject->Rename(*PoolableObjectName);
		// PoolableObject->SetActorLabel(*PoolableObjectName);
		UE_LOG(LogTemp, Warning, TEXT("Spawn New Object %s."), *PoolableObjectName);

		IDRPoolableInterface* PoolableInterface = Cast<IDRPoolableInterface>(PoolableObject);
		PoolableInterface->Execute_OnDeActive(PoolableObject);
	}

	return PoolableObject;
}
