// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatDamageable.h"
#include "CombatDamageableBox.generated.h"

/**
 *  A simple physics box that reacts to damage through the ICombatDamageable interface
 */
UCLASS(abstract)
class ACombatDamageableBox : public AActor, public ICombatDamageable
{
	GENERATED_BODY()
	
	/** Damageable box mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

public:	

	/** Constructor */
	ACombatDamageableBox();

protected:

	/** Amount of HP this box starts with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float CurrentHP = 3.0f;

	/** Time to wait before we remove this box from the level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float DeathDelayTime = 6.0f;

	FTimerHandle DeathTimer;

	/** Blueprint damage handler for effect playback */
	UFUNCTION(BlueprintImplementableEvent, Category="Damage")
	void OnBoxDamaged(const FVector& DamageLocation, const FVector& DamageImpulse);

	/** Blueprint destruction handler for effect playback */
	UFUNCTION(BlueprintImplementableEvent, Category="Damage")
	void OnBoxDestroyed();

	/** Timer callback to remove the box from the level after it dies */
	void RemoveFromLevel();

public:

	/** EndPlay cleanup */
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// ~Begin CombatDamageable interface

	/** Handles damage and knockback events */
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;

	/** Handles death events */
	virtual void HandleDeath() override;

	/** Handles healing events */
	virtual void ApplyHealing(float Healing, AActor* Healer) override;

	// ~End CombatDamageable interface
};
