#include "Network/DroneMovementReplication.h"
#include "Components/DroneMovementComponent.h"
#include <GameFramework/CharacterMovementComponent.h>

void FDroneNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	FCharacterNetworkMoveData::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FSavedMove_Drone* SavedMove_Drone = static_cast<const FSavedMove_Drone*>(&ClientMove);
	if (SavedMove_Drone)
	{

		EngineForce = SavedMove_Drone->EngineForce;

		// UE_LOG(LogTemp, Warning, TEXT("ClientFillNetworkMoveData %f %f %f"), EngineForce.X, EngineForce.Y, EngineForce.Z);
	}
}

bool FDroneNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	bool Result = FCharacterNetworkMoveData::Serialize(CharacterMovement, Ar, PackageMap, MoveType);
	bool bLocalSuccess = true;
	EngineForce.NetSerialize(Ar, PackageMap, bLocalSuccess);

	return !Ar.IsError();;
}

