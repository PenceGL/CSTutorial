// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// #include "Logging/LogMacros.h"
#include "CSTutorialCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class ACSTutorialPlayerController;
class UTimelineComponent;
class UInventoryComponent;
class ACSTutorialHUD;
class UItemBase;
class IInteractionInterface;
class UInputMappingContext;
struct FInputActionValue;

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() :
		CurrentInteractable(nullptr),
		LastInteractionCheckTime(0.0f)
	{
	};

	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ACSTutorialCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
//======================================================================================
// PROPERTIES & VARIABLES
//======================================================================================
	bool bAiming;

//======================================================================================
// FUNCTIONS
//======================================================================================
	// default/built-in UE game template functions
	//---------------------------------------------------------
	ACSTutorialCharacter();
	
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);
	
	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// CSTutorial game functions
	//---------------------------------------------------------
	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; };

	void UpdateInteractionWidget() const;

	void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);


protected:
//======================================================================================
// PROPERTIES & VARIABLES
//======================================================================================
	// default/built-in UE game template properties
	//---------------------------------------------------------
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MouseLookAction;
	
	// CSTutorial game properties
	//---------------------------------------------------------
	UPROPERTY()
	TObjectPtr<ACSTutorialHUD> HUD;
	
	UPROPERTY()
	TObjectPtr<ACSTutorialPlayerController> MainPlayerController;
	
	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	UPROPERTY(VisibleAnywhere, Category= "Character | Inventory")
	TObjectPtr<UInventoryComponent> PlayerInventory;

	// input mapping properties
	//---------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleMenuAction;
	
	// interaction properties
	//---------------------------------------------------------
	float InteractionCheckFrequency;
	float InteractionCheckDistance;
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;
	
	// timeline properties used for camera aiming transition
	//---------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category="Character | Camera")
	FVector DefaultCameraLocation;
	
	UPROPERTY(VisibleAnywhere, Category="Character | Camera")
	FVector AimingCameraLocation;

	UPROPERTY(VisibleAnywhere, Category="Character | Camera")
	TObjectPtr<UTimelineComponent> AimingCameraTimeline;

	UPROPERTY(EditDefaultsOnly, Category="Character | Aim Timeline")
	TObjectPtr<UCurveFloat> AimingCameraCurve;
	
//======================================================================================
// FUNCTIONS
//======================================================================================
	// default/built-in UE game template functions
	//---------------------------------------------------------
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	// CSTutorial game functions
	//---------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void ToggleMenu();

	void Aim();
	void StopAiming();

	UFUNCTION()
	void UpdateCameraTimeline(const float TimelineValue) const;

	UFUNCTION()
	void CameraTimelineEnd() const;

	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();
};
