// game
#include "Player/CSTutorialCharacter.h"
#include "UserInterface/CSTutorialHUD.h"
#include "Components/InventoryComponent.h"
#include "Player/CSTutorialPlayerController.h"
#include "Interfaces/InteractionInterface.h"
#include "World/Pickup.h"

// engine
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "DrawDebugHelpers.h"

ACSTutorialCharacter::ACSTutorialCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(L"CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.0f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(L"FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AimingCameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimingCameraTimeline"));
	DefaultCameraLocation = FVector{0.0f, 0.0f, 65.0f};
	AimingCameraLocation = FVector{175.0f, 50.0f, 55.0f};
	CameraBoom->SocketOffset = DefaultCameraLocation;

	InteractionCheckFrequency = 0.1;
	InteractionCheckDistance = 200.0f;

	// capsule default dimensions = 34.0f, 88.0f
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	BaseEyeHeight = 76.0f;
}

void ACSTutorialCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACSTutorialCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACSTutorialCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void ACSTutorialCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MainPlayerController = Cast<ACSTutorialPlayerController>(GetController());
	HUD = Cast<ACSTutorialHUD>(MainPlayerController->GetHUD());

	if (MainPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MainPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	FOnTimelineFloat AimLerpAlphaValue;
	FOnTimelineEvent TimelineFinishedEvent;
	AimLerpAlphaValue.BindUFunction(this, FName("UpdateCameraTimeline"));
	TimelineFinishedEvent.BindUFunction(this, FName("CameraTimelineEnd"));

	if (AimingCameraTimeline && AimingCameraCurve)
	{
		AimingCameraTimeline->AddInterpFloat(AimingCameraCurve, AimLerpAlphaValue);
		AimingCameraTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent);
	}
}


void ACSTutorialCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void ACSTutorialCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{FVector::ZeroVector};

	if (!bAiming)
	{
		InteractionCheckDistance = 200.0f;
		TraceStart = GetPawnViewLocation();
	}
	else
	{
		InteractionCheckDistance = 250.0f;
		TraceStart = FollowCamera->GetComponentLocation();
	}

	FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}

	NoInteractableFound();
}

void ACSTutorialCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void ACSTutorialCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
			EndInteract();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void ACSTutorialCharacter::BeginInteract()
{
	// verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
				                                this,
				                                &ACSTutorialCharacter::Interact,
				                                TargetInteractable->InteractableData.InteractionDuration,
				                                false);
			}
		}
	}
}

void ACSTutorialCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void ACSTutorialCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void ACSTutorialCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void ACSTutorialCharacter::ToggleMenu()
{
	HUD->ToggleMenu();

	if (HUD->bIsMenuVisible)
	{
		StopAiming();
	}
}

void ACSTutorialCharacter::Aim()
{
	if (!HUD->bIsMenuVisible)
	{
		bAiming = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->PlayFromStart();
	}
}

void ACSTutorialCharacter::StopAiming()
{
	if (bAiming)
	{
		bAiming = false;
		bUseControllerRotationYaw = false;
		HUD->HideCrosshair();
		GetCharacterMovement()->MaxWalkSpeed = 500.f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->Reverse();
	}
}

void ACSTutorialCharacter::UpdateCameraTimeline(const float TimelineValue) const
{
	const FVector CameraLocation = FMath::Lerp(DefaultCameraLocation, AimingCameraLocation, TimelineValue);
	CameraBoom->SocketOffset = CameraLocation;
}

void ACSTutorialCharacter::CameraTimelineEnd()
{
	if (AimingCameraTimeline)
	{
		if (AimingCameraTimeline->GetPlaybackPosition() != 0.0f)
		{
			HUD->ShowCrosshair();
		}
	}
}

void ACSTutorialCharacter::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{GetActorLocation() + (GetActorForwardVector() * 50.0f)};
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
	}
}

void ACSTutorialCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2d>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation{0, Rotation.Yaw, 0};

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ACSTutorialCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2d>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}