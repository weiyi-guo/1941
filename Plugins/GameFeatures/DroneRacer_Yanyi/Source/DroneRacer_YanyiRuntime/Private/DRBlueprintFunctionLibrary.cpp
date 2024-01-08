// Fill out your copyright notice in the Description page of Project Settings.


#include "DRBlueprintFunctionLibrary.h"
#include "Components/DRHealthComponent.h"

bool UDRBlueprintFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	if(UDRHealthComponent* HealthComponent = TargetActor->FindComponentByClass<UDRHealthComponent>())
	{
		return HealthComponent->ApplyDamage(DamageCauser, DamageAmount);
	}
	return false;
}

FString UDRBlueprintFunctionLibrary::ConvertNetModeEnumToString(ENetMode NetMode)
{
	switch (NetMode)
	{
	case NM_Standalone:
		return "ENetMode::NM_Standalone";
		break;
	case NM_DedicatedServer:
		return "ENetMode::NM_DedicatedServer";
		break;
	case NM_ListenServer:
		return "ENetMode::NM_ListenServer";
		break;
	case NM_Client:
		return "ENetMode::NM_Client";
		break;
	case NM_MAX:
		return "ENetMode::NM_MAX";
		break;
	default:
		break;
	}
	return "ENetMode::Unknow";
}

FString UDRBlueprintFunctionLibrary::ConvertNetRoleEnumToString(ENetRole NetRole)
{
	switch (NetRole)
	{
	case ROLE_None:
		return "ENetRole::ROLE_None";
		break;
	case ROLE_SimulatedProxy:
		return "ENetRole::ROLE_SimulatedProxy";
		break;
	case ROLE_AutonomousProxy:
		return "ENetRole::ROLE_AutonomousProxy";
		break;
	case ROLE_Authority:
		return "ENetRole::ROLE_Authority";
		break;
	case ROLE_MAX:
		return "ENetRole::ROLE_MAX";
		break;
	default:
		break;
	}
	return "ENetRole::Unknow";
}