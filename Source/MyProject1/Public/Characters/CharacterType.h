#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped 	UMETA(DisplayName="Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName="EquippedOneHandedWeapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName="EquippedTwoHandedWeapon")
};
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied	UMETA(DisplayName="Unoccupied"),
	EAS_Attacking	UMETA(DisplayName="Attacking"),
	EAS_Equiping	UMETA(DisplayName="Equiping")
};
UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EAS_Alive	UMETA(DisplayName="Alive"),
	EAS_Death1	UMETA(DisplayName="Death1"),
	EAS_Death2	UMETA(DisplayName="Death2")
};

enum class EEnemtState : uint8
{
	EES_Patrolling	UMETA(DisplayName="Patrolling"),
	EES_Chasing	UMETA(DisplayName="Chasing"),
	EES_Attacking	UMETA(DisplayName="Attacking"),
	EES_Dead	UMETA(DisplayName="Dead")
};


