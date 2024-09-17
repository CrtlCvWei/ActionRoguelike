// Fill out your copyright notice in the Description page of Project Settings.


#include "AwCharacter.h"

#include "AWInteractionComponent.h"
#include "MyGAS/AwActionComponent.h"
#include "MyMovementComp/AwCharacterMovementComponent.h"
#include "AWPlayerState.h"
#include "..\Public\MyGAS/AWAttributeComp.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyMovementComp/System/MoveSystem.h"
#include "UI/AwHUD.h"
#include "Utils/utils.h"


// Sets default values

# pragma region Init
AAwCharacter::AAwCharacter(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAwCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Init_Paramters();
}

void AAwCharacter::Init_Paramters()
{
	// My Own Movement Component
	AwCharacterMovementComp = Cast<UAwCharacterMovementComponent>(GetCharacterMovement());

	ArrowComp = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetVisibility(true);
	// 当控制器旋转时不旋转使其仅影响弹簧臂。
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("AwSpringArmComponent");
	SpringArmComp->SetupAttachment(RootComponent);
	// 弹簧臂跟随移动
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("AwCameraComponent");
	CameraComp->SetupAttachment(SpringArmComp);
	// 摄像机不跟随移动
	CameraComp->bUsePawnControlRotation = false;

	// INPUT 'F' to interact
	InteractionComp = CreateDefaultSubobject<UAWInteractionComponent>("InteractionComp");

	//GAS

	AwCharacterMovementComp->MaxWalkSpeed = this->NormalMoveSpeed;
	AwCharacterMovementComp->GravityScale = this->GravityScale;
	JumpMaxHoldTime = 1.0f;
	JumpMaxCount = 1;
	JumpKeyHoldTime = 0.5f;
	AwCharacterMovementComp->JumpZVelocity = this->JumpZVelocity;
	AwCharacterMovementComp->bOrientRotationToMovement = true;
	AwCharacterMovementComp->BrakingFriction = this->BrakingFriction;
	AwCharacterMovementComp->BrakingDecelerationWalking = this->BrakingDecelerationWalking;
	AwCharacterMovementComp->BrakingDecelerationFlying = 512.f;
	AwCharacterMovementComp->BrakingDecelerationFalling = this->BrakingDecelerationWalking;
	AwCharacterMovementComp->MaxFlySpeed = this->ClimbVectorZ;

	BaseEyeHeight = 103.;
}

void AAwCharacter::Init_GAS()
{
	UAWAttributeComp* AttributeComp = GetOwningAttribute();

	if (!AttributeComp)
		return;

	AttributeComp->AttributeChangeBindBase("Health", this, &AAwCharacter::OnHealthChange,
	                                       "&AAwCharacter::OnHealthChange");
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AAwHUD* HUD = Cast<AAwHUD>(PC->GetHUD()))
		{
			AAWPlayerState* PS = PC->GetPlayerState<AAWPlayerState>();
			if (!ensure(PS)) return;
			UAWAttributeComp* AC = PS->GetPlayerAttribute();
			if (ensure(AC))
			{
				HUD->InitOverlayWidget(PC, PS, AC, GetOwningAction());
			}
		}
	}

	if (HasAuthority())
	{
		if (const auto ActionComp = GetOwningAction())
		{
			ActionComp->SetOwningActor();
			for (TSubclassOf<UAwAction> ActionClass : ActionComp->GetDefaultActions())
			{
				ActionComp->AddAction(ActionClass);
			}
		}
	}
	// GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
}

# pragma endregion

void AAwCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

UAWAttributeComp* AAwCharacter::GetOwningAttribute() const
{
	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return nullptr;
	AAWPlayerState* PS = PC->GetPlayerState<AAWPlayerState>();
	if (!(PS))
		return nullptr;
	if (PS->GetPlayerAttribute()->GetOwningActor() != this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
		                                 "Player Attribute is not found And Try to set Owning Actor");
		PS->GetPlayerAttribute()->SetOwningActor();
		PS->GetPlayerAttribute()->GetAttributeSet()->SetOwningActor();
	}
	return PS->GetPlayerAttribute();
}

UAwActionComponent* AAwCharacter::GetOwningAction() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!ensure(PC))
		return nullptr;
	AAWPlayerState* PS = PC->GetPlayerState<AAWPlayerState>();
	if (!ensure(PS))
		return nullptr;
	if (PS->GetPlayerAction()->GetOwningActor() != this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
		                                 "Player Action is not found And Try to set Owning Actor");
		PS->GetPlayerAction()->SetOwningActor();
	}
	return PS->GetPlayerAction();;
}

FCollisionQueryParams AAwCharacter::GetIgnoreCollisionParams() const
{
	FCollisionQueryParams Parameters;
	TArray<AActor*> ActorChildrens;
	GetAllChildActors(ActorChildrens);
	Parameters.AddIgnoredActors(ActorChildrens);
	Parameters.AddIgnoredActor(this);
	return Parameters;
}

// Called when the game starts or when spawned
void AAwCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(InitTimerHandle, this, &AAwCharacter::Init_GAS, 1.f, false);
}

// Called every frame
void AAwCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation("Head"),
	//               GetMesh()->GetSocketLocation("Head") + GetActorForwardVector() * 100, FColor::Red, false, 0.1f, 0,
	//               1.f);
}

bool AAwCharacter::GetIsClimbing() const
{
	return AwCharacterMovementComp->IsClimbing();
}

void AAwCharacter::BeginSprint()
{
	GetOwningAction()->StartActionByName(this, "Sprint");
}

void AAwCharacter::EndSprint()
{
	GetOwningAction()->StopActionByName(this, "Sprint");
}

void AAwCharacter::MoveForward(float Values)
{
	if ((Controller != nullptr) && (Values != 0.0f) && AwCharacterMovementComp)
	{
		AwCharacterMovementComp->HandleMove(Values, EMoveDirection::X);
	}
}

void AAwCharacter::MoveRight(float Values)
{
	if ((Controller != nullptr) && (Values != 0.0f) && AwCharacterMovementComp)
	{
		AwCharacterMovementComp->HandleMove(Values, EMoveDirection::Y);
	}
}

void AAwCharacter::CrouchPressed()
{
	AwCharacterMovementComp->CrouchPressed();
}

#pragma region Climb

void AAwCharacter::RidOffClimbingMode()
{
	AwCharacterMovementComp->StopClimb();
}

void AAwCharacter::EdgeClimbing()
{
	if (AwCharacterMovementComp && AwCharacterMovementComp->CanClimbDownLedge())
		AwCharacterMovementComp->EdgeClimb();
}

#pragma endregion

void AAwCharacter::OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth,
                                  float Change)
{
	if (UAWAttributeComp* PlayerAttribute = GetOwningAttribute())
	{
		if (!PlayerAttribute->isAlive())
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			// ragdoll
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
			FTimerHandle DeadDelay;
			GetWorldTimerManager().SetTimer(DeadDelay, this, &AAwCharacter::Deadth, 2.f);
			DisableInput(PC);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		                                 "Player Attribute is not found\n In FUN AAwCharacter::OnHealthChange");
	}
}

void AAwCharacter::HealSelf(float v)
{
	if (UAWAttributeComp* PlayerAttribute = GetOwningAttribute())
	{
		PlayerAttribute->SetHealth(v, this);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Player Attribute is not found");
	}
}

void AAwCharacter::Jump()
{
	Super::Jump();
	if (AwCharacterMovementComp && TraceClimbaleSur(AwCharacterMovementComp) && !AwCharacterMovementComp->IsClimbing())
	{
		AwCharacterMovementComp->Climb();
	}
}

void AAwCharacter::ClearJumpInput(float DeltaTime)
{
	if (bPressedJump)
	{
		JumpKeyHoldTime += DeltaTime;

		// Some Changes
		if (JumpKeyHoldTime <= JumpHoldTimeThreshold)
		{
			// jump higher
			GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
		}
		else
		{
			//normal jump
			GetCharacterMovement()->JumpZVelocity = JumpZVelocity * 0.5;
		}

		//

		// Don't disable bPressedJump right away if it's still held.
		// Don't modify JumpForceTimeRemaining because a frame of update may be remaining.
		if (JumpKeyHoldTime >= GetJumpMaxHoldTime())
		{
			bPressedJump = false;
		}
	}
	else
	{
		JumpForceTimeRemaining = 0.0f;
		bWasJumping = false;
	}
}

void AAwCharacter::StopJumping()
{
	Super::StopJumping();
	GetWorldTimerManager().ClearTimer(JumpTimerHandle);
}

void AAwCharacter::PrimaryAttack()
{
	GetOwningAction()->StartActionByName(this, "PrimaryAttack");
}

void AAwCharacter::BlackHoleAbility()
{
	GetOwningAction()->StartActionByName(this, "BlackHoleAbility");
}

void AAwCharacter::PrimaryInterat()
{
	if (this->InteractionComp)
	{
		InteractionComp->PrimaryIntract();
	}
}

FVector AAwCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void AAwCharacter::Deadth()
{
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");
}

// Called to bind functionality to input
void AAwCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Control Movement
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AAwCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAwCharacter::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAwCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AAwCharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AAwCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AAwCharacter::EndSprint);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AAwCharacter::CrouchPressed);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AAwCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &AAwCharacter::PrimaryInterat);
	PlayerInputComponent->BindAction("BlackHoleAbility", IE_Pressed, this, &AAwCharacter::BlackHoleAbility);
	PlayerInputComponent->BindAction("CancelClimbing", IE_Pressed, this, &AAwCharacter::RidOffClimbingMode);
	PlayerInputComponent->BindAction("EdgeClimbing", IE_Pressed, this, &AAwCharacter::EdgeClimbing);
}
