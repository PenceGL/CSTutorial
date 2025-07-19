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
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// CSTutorial game functions
	//---------------------------------------------------------
	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TH_TimedInteraction); }

	FORCEINLINE TObjectPtr<UInventoryComponent> GetInventory() const { return PlayerInventory; }
	FORCEINLINE ACSTutorialHUD* GetHUD() const { return HUD; }

	void UpdateInteractionWidget() const;

	void DropItem(const TObjectPtr<UItemBase>& ItemToDrop);


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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerCharacter | Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerCharacter | Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerCharacter | Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PlayerCharacter | Input")
	UInputAction* MouseLookAction;
	
	// CSTutorial miscellaneous game properties
	//---------------------------------------------------------
	UPROPERTY()
	TObjectPtr<ACSTutorialHUD> HUD;
	
	UPROPERTY()
	TObjectPtr<ACSTutorialPlayerController> MainPlayerController;

	UPROPERTY(VisibleAnywhere, Category= "PlayerCharacter | Inventory")
	TObjectPtr<UInventoryComponent> PlayerInventory;

	// input mapping properties
	//---------------------------------------------------------
	UPROPERTY(EditAnywhere, Category="PlayerCharacter | Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category="PlayerCharacter | Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category="PlayerCharacter | Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter | Input")
	TObjectPtr<UInputAction> ToggleMenuAction;
	
	// interaction properties
	//---------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "PlayerCharacter | Interaction")
	float InteractionCheckFrequency;

	/** @brief Used to set the maximum distance at which actors can be interacted with while aiming.*/
	UPROPERTY(EditAnywhere, Category = "PlayerCharacter | Interaction")
	float AimingInteractionDistance;
	
	/** @brief Used to set the maximum distance at which actors can be interacted with while not aiming.*/
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter | Interaction")
	float DefaultInteractionDistance;

	UPROPERTY(VisibleAnywhere, Category = "PlayerCharacter | Interaction")
	TScriptInterface<IInteractionInterface> InteractionTarget;

	FCollisionQueryParams InteractionCollisionQueryParams;
	FCollisionObjectQueryParams InteractionObjectQueryParams;
	// array that will be reused for storing any detected interactables
	TArray<FHitResult> OutHits;
	
	/** @brief Timer handle used for a timed interaction (ex: hold button to turn valve, etc.).*/
	FTimerHandle TH_TimedInteraction;
	
	/** @brief Timer handle used to control firing the line trace that checks for interactables.*/
	FTimerHandle TH_InteractionCheck;
	
	// properties related to camera aiming transition
	//---------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category="PlayerCharacter | Aiming")
	FVector DefaultCameraLocation;
	
	UPROPERTY(VisibleAnywhere, Category="PlayerCharacter | Aiming")
	FVector AimingCameraLocation;

	UPROPERTY(VisibleAnywhere, Category="PlayerCharacter | Aiming")
	TObjectPtr<UTimelineComponent> AimingCameraTimeline;

	UPROPERTY(EditDefaultsOnly, Category="PlayerCharacter | Aiming")
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
	
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="PlayerCharacter | Input")
	virtual void DoMove(float Right, float Forward);
	
	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="PlayerCharacter | Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="PlayerCharacter | Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="PlayerCharacter | Input")
	virtual void DoJumpEnd();
	
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
