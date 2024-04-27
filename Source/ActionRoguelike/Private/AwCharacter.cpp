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
#include "UI/AwHUD.h"


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

	AttributeComp->AttributeChangeBind("Health", this, &AAwCharacter::OnHealthChange,
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
	GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
}

# pragma endregion

void AAwCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

UAWAttributeComp* AAwCharacter::GetOwningAttribute() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	// if(!ensure(PC))
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

	GetWorldTimerManager().SetTimer(InitTimerHandle, this, &AAwCharacter::Init_GAS, 0.2f, true);
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
	// GetCharacterMovement()->MaxWalkSpeed = this->MaxMoveSpeed;
	GetOwningAction()->StartActionByName(this, "Sprint");
}

void AAwCharacter::EndSprint()
{
	// GetCharacterMovement()->MaxWalkSpeed = this->NormalMoveSpeed;
	GetOwningAction()->StopActionByName(this, "Sprint");
}

void AAwCharacter::MoveForward(float Values)
{
	if ((Controller != nullptr) && (Values != 0.0f) && AwCharacterMovementComp)
	{
		if (AwCharacterMovementComp->IsClimbing())
		{
			MoveForwardWhenClimbing(Values);
			return;
		}
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Values);
	}
}

void AAwCharacter::MoveRight(float Values)
{
	if ((Controller != nullptr) && (Values != 0.0f) && AwCharacterMovementComp)
	{
		if (AwCharacterMovementComp->IsClimbing())
		{
			MoveRightWhenClimbing(Values);
			return;
		}
		// rotate the character to the right direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

		// Add movement in that direction
		AddMovementInput(Direction, Values);
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

void AAwCharacter::MoveForwardWhenClimbing(float Values)
{
	FHitResult HitResult;
	const FVector Direction = FVector::CrossProduct(-AwCharacterMovementComp->GetClimbSurfaceNormal(),
	                                                GetActorRightVector()).GetSafeNormal();

	const FVector StartLocation = GetActorLocation() + Direction * 100 * Values;
	const FVector EndLocation = StartLocation + (-AwCharacterMovementComp->GetClimbSurfaceNormal()) * 100;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
	                                                     10, 35, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	                                                     false,
	                                                     TArray<AActor*>({this,}), EDrawDebugTrace::ForOneFrame,
	                                                     HitResult,
	                                                     true);
	if (!bHit)
	{
		// Detect and Climb Up Ledge
		DetectAndClimbUp();
		return;
	}
	AddMovementInput(Direction, Values);
}

void AAwCharacter::MoveRightWhenClimbing(float Values)
{
	FHitResult ClimbHitResult;

	const FVector Direction = FVector::CrossProduct(-AwCharacterMovementComp->GetClimbSurfaceNormal(),
	                                                -GetActorUpVector()).GetSafeNormal();
	const FVector StartLocation = GetMesh()->GetSocketLocation("Chest") + Direction * 50 * Values;
	const FVector EndLocation = StartLocation + (-AwCharacterMovementComp->GetClimbSurfaceNormal()) * 100;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
	                                                     10, 40, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	                                                     false,
	                                                     TArray<AActor*>({this}), EDrawDebugTrace::ForOneFrame,
	                                                     ClimbHitResult,
	                                                     true);

	AddMovementInput(Direction, Values);
	// if (!bHit)
	// {
	// 	return;
	// }
	// FVector OL_N_NEW = ClimbHitResult.Normal;
	//
	// FVector ActorLocation = GetMesh()->GetSocketLocation("Chest");
	// FVector OL_N_OLD = -GetActorForwardVector();
	// bHit = GetWorld()->LineTraceSingleByObjectType(ClimbHitResult, ActorLocation,
	//                                                ActorLocation + GetActorForwardVector() * 200,
	//                                                ECC_WorldStatic, GetIgnoreCollisionParams()); // 忽略当前角色自身，避免与自身发生碰撞
	//
	// ActorLocation = GetMesh()->GetSocketLocation("Chest") + GetActorRightVector() * Values;
	//
	// float Distance_pre = FVector::Distance(ActorLocation, ClimbHitResult.ImpactPoint);
	// bHit = GetWorld()->LineTraceSingleByObjectType(ClimbHitResult, ActorLocation,
	//                                                ActorLocation + GetActorForwardVector() * 200,
	//                                                ECC_WorldStatic, GetIgnoreCollisionParams());
	//
	// if (OL_N_OLD == OL_N_NEW)
	// {
	// 	AddMovementInput(Direction, Values);
	// 	return;
	// }
	// float Deta_Distance = FVector::Distance(ActorLocation, ClimbHitResult.ImpactPoint) - Distance_pre;
	// if (Deta_Distance > 0)
	// 	Values += tan(FMath::Acos(FVector::DotProduct(OL_N_OLD, OL_N_NEW))) * Deta_Distance * (Values > 0 ? 1 : -1);
	// AddMovementInput(Direction, Values);
	// FRotator NewRotator = FRotationMatrix::MakeFromX(-OL_N_NEW).Rotator();
	// NewRotator = FMath::RInterpTo(GetActorRotation(), NewRotator, GetWorld()->GetDeltaSeconds(), 2.f);
	// if(!NewRotator.IsNearlyZero())
	// 	SetActorRotation(NewRotator);
}

bool AAwCharacter::DetectAndClimbUp()
{
	// Detect any obstacle on the edge
	FHitResult HitResult;
	const FVector UpVector = FVector::CrossProduct(-AwCharacterMovementComp->GetClimbSurfaceNormal(),
	                                               GetActorRightVector()).GetSafeNormal();
	const FVector StartLocation = GetActorLocation() + UpVector * 150;
	const FVector EndLocation = StartLocation + -AwCharacterMovementComp->GetClimbSurfaceNormal() * 50;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
	                                                     30, 90, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	                                                     false,
	                                                     TArray<AActor*>({this}), EDrawDebugTrace::ForOneFrame,
	                                                     HitResult,
	                                                     true);
	if (bHit) // if there is any obstacles
		return false;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 忽略当前角色自身，避免与自身发生碰撞

	if (GetWorld()->LineTraceSingleByChannel(HitResult, EndLocation, EndLocation + FVector(0, 0, -200), ECC_Visibility,
	                                         CollisionParams))
	{
		// can land
		ClimbingUp.Broadcast(HitResult.ImpactPoint);
		return true;
	}
	return false;
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
	if (AwCharacterMovementComp && AwCharacterMovementComp->TraceClimbaleSurface() && !AwCharacterMovementComp->IsClimbing())
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
