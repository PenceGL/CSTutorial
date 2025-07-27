// game
#include "CSTutorialCharacter.h"
#include "UserInterface/CSTutorialHUD.h"
#include "UserInterface/Interaction/InteractionWidget.h"
#include "Components/InventoryComponent.h"
#include "CSTutorialPlayerController.h"
#include "Interfaces/InteractionInterface.h"
#include "World/Container.h"
#include "World/Pickup.h"

// engine
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ACSTutorialCharacter::ACSTutorialCharacter() :
	InteractionCheckFrequency(0.1f),
	AimingInteractionDistance(275.0f),
	DefaultInteractionDistance(475.0f)
{
	// default/built-in UE game template construction code
	//-----------------------------------------------------------------------------------
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// CSTutorial game construction code
	//-----------------------------------------------------------------------------------
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.0f);

	AimingCameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimingCameraTimeline"));
	DefaultCameraLocation = FVector{0.0f, 0.0f, 65.0f};
	AimingCameraLocation = FVector{175.0f, 50.0f, 55.0f};
	CameraBoom->SocketOffset = DefaultCameraLocation;

	// capsule default dimensions = 34.0f, 88.0f
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	BaseEyeHeight = 76.0f;
}

void ACSTutorialCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// default/built-in UE game template input bindings
		//-----------------------------------------------------------------------------------
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving (also binds to controller left thumbstick)
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACSTutorialCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACSTutorialCharacter::Look);

		// Looking (Controller right thumbstick only)
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACSTutorialCharacter::Look);

		// CSTutorial game input bindings
		//-----------------------------------------------------------------------------------
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACSTutorialCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ACSTutorialCharacter::EndInteract);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACSTutorialCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACSTutorialCharacter::StopAiming);

		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &ACSTutorialCharacter::ToggleMenu);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ACSTutorialCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<ACSTutorialPlayerController>(GetController());
	HUD = Cast<ACSTutorialHUD>(MainPlayerController->GetHUD());

	FOnTimelineFloat AimLerpAlphaValue;
	FOnTimelineEvent TimelineFinishedEvent;
	AimLerpAlphaValue.BindUFunction(this, FName("UpdateCameraTimeline"));
	TimelineFinishedEvent.BindUFunction(this, FName("CameraTimelineEnd"));

	if (AimingCameraTimeline && AimingCameraCurve)
	{
		AimingCameraTimeline->AddInterpFloat(AimingCameraCurve, AimLerpAlphaValue);
		AimingCameraTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AimingCameraTimeline or AimingCameraCurve were null! Aiming functionality will not work until corrected."));
	}

	// start looping timer for interactable checking
	GetWorldTimerManager().SetTimer(
		TH_InteractionCheck,
		this,
		&ACSTutorialCharacter::PerformInteractionCheck,
		InteractionCheckFrequency,
		true);

	// initialize defaults for interaction system queries
	InteractionCollisionQueryParams.bTraceComplex = false;
	InteractionObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
}

void ACSTutorialCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GetWorldTimerManager().IsTimerActive(TH_TimedInteraction))
	{
		// percentage of completion = timer value / interaction duration
		const float InteractProgress =
			GetWorldTimerManager().GetTimerElapsed(TH_TimedInteraction) /
			InteractionTarget->InteractableData.InteractionDuration;
		HUD->GetInteractionWidget()->InteractionProgressBar->SetPercent(FMath::Clamp(InteractProgress, 0.0f, 1.0f));
	}
}

void ACSTutorialCharacter::PerformInteractionCheck()
{
	// Use dot product to only allow the line trace to happen if view is pointed
	// in the 180 degree arc in front of the character
	if (FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector()) > 0)
	{
		const FVector TraceStart{FollowCamera->GetComponentLocation()};
		const float InteractionCheckDistance = bAiming ? AimingInteractionDistance : DefaultInteractionDistance;
		const FVector TraceEnd{TraceStart + GetViewRotation().Vector() * InteractionCheckDistance};

		// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

		// draw debug sphere at the same location and size as the sweep sphere
		// DrawDebugSphere(GetWorld(),
		//                 TraceEnd,
		//                 70.0f,
		//                 8, FColor::Blue,
		//                 false,
		//                 5.0f);

		// create an array to store all sweep hit results
		if (GetWorld()->SweepMultiByObjectType(OutHits,
		                                       TraceStart,
		                                       TraceEnd,
		                                       FQuat::Identity,
		                                       InteractionObjectQueryParams,
		                                       FCollisionShape::MakeSphere(70.0f),
		                                       InteractionCollisionQueryParams))
		{
			FHitResult ClosestHit = OutHits[0];
			// if more than one interactable was detected, find the closest one by iterating through all hits to find the
			// one with the lowest distance value
			if (OutHits.Num() > 1)
			{
				for (const FHitResult& Hit : OutHits)
				{
					if (Hit.Distance < ClosestHit.Distance)
					{
						ClosestHit = Hit;
					}
				}
			}

			// check for interactable interface
			if (ClosestHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				// check if the new interactable is different from the previous one
				if (ClosestHit.GetActor() != InteractionTarget.GetObject())
				{
					// end focus on the previous interactable
					if (IsValid(InteractionTarget.GetObject()))
					{
						InteractionTarget->EndFocus();
					}

					// process new interactable
					FoundInteractable(ClosestHit.GetActor());
					return;
				}

				// in case we opened a container, the interaction widget was hidden
				// if looking at the same container, and the container interface has been closed
				// show the interaction widget again
				if (!HUD->bContainerInterfaceOpen && !HUD->bInteractionWidgetVisible)
				{
					HUD->ShowInteractionWidget();
				}

				return;
			}
		}
	}

	NoInteractableFound();
}

void ACSTutorialCharacter::FoundInteractable(AActor* NewInteractable)
{
	InteractionTarget = NewInteractable;

	InteractionTarget->BeginFocus();

	// send the new InteractionTarget data to the InteractionWidget
	HUD->UpdateInteractionWidget(&InteractionTarget->InteractableData);
}

void ACSTutorialCharacter::NoInteractableFound()
{
	// clear timer for any ongoing timed interaction
	GetWorldTimerManager().ClearTimer(TH_TimedInteraction);

	// call end focus on current interaction target
	if (IsValid(InteractionTarget.GetObject()))
	{
		InteractionTarget->EndInteract();
		InteractionTarget->EndFocus();
		InteractionTarget = nullptr;
	}

	HUD->GetInteractionWidget()->InteractionProgressBar->SetPercent(0.0f);
	HUD->HideInteractionWidget();
}

void ACSTutorialCharacter::BeginInteract()
{
	if (IsValid(InteractionTarget.GetObject()))
	{
		InteractionTarget->BeginInteract();

		// if there no time requirement on the interaction, simply call Interact
		if (FMath::IsNearlyZero(InteractionTarget->InteractableData.InteractionDuration, 0.1f))
		{
			Interact();
		}
		else
		{
			// otherwise, start the timed interaction timer
			// after the duration of InteractionDuration has elapsed, Interact is called
			GetWorldTimerManager().SetTimer(TH_TimedInteraction,
			                                this,
			                                &ACSTutorialCharacter::Interact,
			                                InteractionTarget->InteractableData.InteractionDuration,
			                                false);
		}
	}
}

void ACSTutorialCharacter::EndInteract()
{
	// clear timer in case the interact button was released prior to reaching the timed interaction duration
	GetWorldTimerManager().ClearTimer(TH_TimedInteraction);

	if (IsValid(InteractionTarget.GetObject()))
	{
		InteractionTarget->EndInteract();
		// can reset progress bar percent here if needed, or possibly could use this as a feature
	}
}

void ACSTutorialCharacter::Interact()
{
	// clear timer in case the interaction had a time requirement
	GetWorldTimerManager().ClearTimer(TH_TimedInteraction);

	if (IsValid(InteractionTarget.GetObject()))
	{
		// special actions that need to occur for each interactable type
		switch (InteractionTarget->InteractableData.InteractableType)
		{
		case EInteractableType::Pickup:
			{
			}
			break;
		case EInteractableType::NonPlayerCharacter:
			{
			}
			break;
		case EInteractableType::Device:
			{
			}
			break;
		case EInteractableType::Toggle:
			{
			}
			break;
		case EInteractableType::Container:
			{
				// link the container inventory to the player container interface 
				HUD->SetTargetContainer(Cast<AContainer>(InteractionTarget.GetObject()), this);
			}
			break;
		default:
			{
			}
		}

		InteractionTarget->Interact(this);
	}
}

void ACSTutorialCharacter::UpdateInteractionWidget() const
{
	if (IsValid(InteractionTarget.GetObject()))
	{
		HUD->UpdateInteractionWidget(&InteractionTarget->InteractableData);
	}
}

void ACSTutorialCharacter::ToggleMenu()
{
	HUD->ToggleMenu();

	if (HUD->bMainMenuOpen)
	{
		StopAiming();
	}
}

void ACSTutorialCharacter::Aim()
{
	if (!HUD->HasAnyMenuOpen())
	{
		bAiming = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = 200.0f;

		if (AimingCameraTimeline)
		{
			AimingCameraTimeline->PlayFromStart();
		}
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
		{
			AimingCameraTimeline->Reverse();
		}
	}
}

void ACSTutorialCharacter::UpdateCameraTimeline(const float TimelineValue) const
{
	const FVector CameraLocation = FMath::Lerp(DefaultCameraLocation, AimingCameraLocation, TimelineValue);
	CameraBoom->SocketOffset = CameraLocation;
}

void ACSTutorialCharacter::CameraTimelineEnd() const
{
	if (AimingCameraTimeline)
	{
		if (AimingCameraTimeline->GetPlaybackPosition() != 0.0f)
		{
			HUD->ShowCrosshair();
		}
	}
}

void ACSTutorialCharacter::DropItem(UItemBase* ItemToDrop)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector SpawnLocation{GetActorLocation() + GetActorForwardVector() * 50.0f};
	const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

	APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);
	Pickup->InitializeDrop(ItemToDrop);
	PlayerInventory->HandleRemoveItem(ItemToDrop);
}

void ACSTutorialCharacter::ExitContainerRadius() const
{
	HUD->ClearTargetContainer();
}

void ACSTutorialCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ACSTutorialCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ACSTutorialCharacter::DoMove(const float Right, const float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ACSTutorialCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ACSTutorialCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ACSTutorialCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
