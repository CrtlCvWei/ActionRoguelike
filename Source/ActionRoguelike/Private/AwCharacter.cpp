// Fill out your copyright notice in the Description page of Project Settings.


#include "AwCharacter.h"


#include "MyGAS/AwActionComponent.h"
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
AAwCharacter::AAwCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Init_Paramters(); 
}

void AAwCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

UAWAttributeComp* AAwCharacter::GetOwningAttribute() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	// if(!ensure(PC))
	if(!PC)
		return nullptr;
	AAWPlayerState* PS = PC->GetPlayerState<AAWPlayerState>();
	if(!(PS))
		return  nullptr;
	if(PS->GetPlayerAttribute()->GetOwningActor() != this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Player Attribute is not found And Try to set Owning Actor");
		PS->GetPlayerAttribute()->SetOwningActor();
		PS->GetPlayerAttribute()->GetAttributeSet()->SetOwningActor();
	}
	return PS->GetPlayerAttribute();
}

UAwActionComponent* AAwCharacter::GetOwningAction() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if(!ensure(PC))
		return nullptr;
	AAWPlayerState* PS = PC->GetPlayerState<AAWPlayerState>();
	if(!ensure(PS))
		return  nullptr;
	if(PS->GetPlayerAction()->GetOwningActor() != this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Player Action is not found And Try to set Owning Actor");
		PS->GetPlayerAction()->SetOwningActor();
	}
	return  PS->GetPlayerAction();;
}

void AAwCharacter::Init_Paramters()
{
	bIsClimbing = false;
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
	
	GetCharacterMovement()->MaxWalkSpeed = this->NormalMoveSpeed;
	GetCharacterMovement()->GravityScale = this->GravityScale;
	JumpMaxHoldTime = 1.0f;
	JumpMaxCount = 1;
	JumpKeyHoldTime = 0.5f;
	GetCharacterMovement()->JumpZVelocity = this->JumpZVelocity;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->BrakingFriction = this->BrakingFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = this->BrakingDecelerationWalking;
	GetCharacterMovement()->BrakingDecelerationFlying = 512.f;
	GetCharacterMovement()->BrakingDecelerationFalling = this->BrakingDecelerationWalking;
	GetCharacterMovement()->MaxFlySpeed = this->ClimbVectorZ;

	BaseEyeHeight = 103.;
}

// Called when the game starts or when spawned
void AAwCharacter::BeginPlay()
{
	Super::BeginPlay();
	//init
	UAWAttributeComp* AttributeComp =GetOwningAttribute();
	if (ensure(AttributeComp))
	{
		AttributeComp->AttributeChangeBind("Health", this, &AAwCharacter::OnHealthChange,"&AAwCharacter::OnHealthChange");
	}
	
	if(APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if(AAwHUD* HUD = Cast<AAwHUD>(PC->GetHUD()))
		{
			AAWPlayerState* PS =  PC->GetPlayerState<AAWPlayerState>();
			if(!ensure(PS)) return;
			UAWAttributeComp* AC =  PS->GetPlayerAttribute();
			if(ensure(AC))
			{
				HUD->InitOverlayWidget(PC,PS,AC,GetOwningAction());
			}
		}
	}

}

// Called every frame
void AAwCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// DrawDebugLine(GetWorld(), GetMesh()->GetSocketLocation("Head"),
	//               GetMesh()->GetSocketLocation("Head") + GetActorForwardVector() * 100, FColor::Red, false, 0.1f, 0,
	//               1.f);


	// if (!bWasJumping && !GetCharacterMovement()->IsFalling())
	// 	DetectWall();
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
	if ((Controller != nullptr) && (Values != 0.0f))
	{
		if (bIsClimbing)
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
	if ((Controller != nullptr) && (Values != 0.0f))
	{
		if (bIsClimbing)
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

bool AAwCharacter::DetectWall()
{
	FVector StartLocation = GetMesh()->GetSocketLocation(RightKneeSocketName);
	FVector EndLocation = StartLocation + GetActorForwardVector() * 150;
	FVector BoxExtend = FVector(50, 50, 25);

	FCollisionShape CollisionBox = FCollisionShape::MakeBox(BoxExtend);
	// 创建射线参数
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 忽略当前角色自身，避免与自身发生碰撞
	// 执行射线检测
	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity,
	                                             ECC_WorldStatic, CollisionBox, CollisionParams);

	if (!bHit)
		return false;

	FVector Hit_normal = HitResult.ImpactNormal;
	FVector Hit_location = HitResult.ImpactPoint;

	StartLocation = Hit_location + Hit_normal * 100;
	EndLocation = Hit_location - Hit_normal * 200;
	// Draw
	// DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxExtend, FQuat::Identity, FColor::Red, false, 2.f);

	// 计算 法向量 和角色朝向夹角
	float Angle = FMath::Acos(FVector::DotProduct(Hit_normal, -1 * GetActorForwardVector()));
	if (abs(Angle) > 30)
		return false;

	const float Dis_thred = 100.f;
	FVector Hit_location_pre = Hit_location;
	FVector ZOffest = FVector(0, 0, 25);

	while (bHit && Hit_location.Z < BaseEyeHeight + 100.)
	{
		bHit = GetWorld()->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity,
		                                           ECC_WorldStatic,
		                                           CollisionBox, CollisionParams);
		if (!bHit)
			break;
		
		Hit_normal = HitResult.ImpactNormal;
		Hit_location = HitResult.ImpactPoint;

		float Dis = FMath::Abs((Hit_location - Hit_location_pre).Dot(GetActorForwardVector()));
		//
		

		if (Dis > Dis_thred ||
			Hit_normal.Z < -0.5 || Hit_normal.Z > 0.86)
			return false;
		//
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, BoxExtend, FQuat::Identity, FColor::Red, false, 2.f);
		
		Hit_location_pre = Hit_location;
		StartLocation += ZOffest;
		EndLocation += ZOffest;
	}
	
	if (Hit_location.Z > BaseEyeHeight + 50.)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, Hit_normal.ToString());
		DrawDebugBox(GetWorld(), Hit_location, BoxExtend, FQuat::Identity, FColor::Green, false, 2.f);
		return true;
	}

	return false;
}

FRotator AAwCharacter::FindClimbRotation(const FVector StartLoca, const FVector EndLoca, FVector ObstacleNormalVec)
{
	FVector ClimbUpVec = EndLoca - StartLoca;
	ClimbUpVec.Normalize(); // 单位向量
	FVector ClimbRightVec = ObstacleNormalVec.Cross(ClimbUpVec); // ue 是左手系好像
	ClimbRightVec.Normalize();
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ClimbRightVec * 100, 100,
	                          FColor::Green, false,
	                          0.f);
	FVector Up = ClimbRightVec.Cross(ObstacleNormalVec);
	Up.Normalize();
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + Up * 100, 100,
	                          FColor::Red, false,
	                          0.f);

	return FMatrix(-1 * ObstacleNormalVec, ClimbRightVec, Up, FVector::ZeroVector).Rotator();
}

void AAwCharacter::ClimbingTick()
{
	// 当玩家处于攀爬状态时，时刻检测墙面

	static FHitResult ClimbHitResult;
	static FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FVector ActorLocation = GetMesh()->GetSocketLocation("Chest");

	bool bHit = GetWorld()->LineTraceSingleByObjectType(ClimbHitResult, ActorLocation,
	                                                    ActorLocation + GetActorForwardVector() * 200,
	                                                    ECC_WorldStatic, CollisionParams); // 忽略当前角色自身，避免与自身发生碰撞
	if (!bHit)
	{
		RidOffClimbingMode();
		return;
	}


	FVector OL = ClimbHitResult.ImpactPoint; // would be the start location of FindClimbRotation Call
	FVector OL_Norm = ClimbHitResult.Normal;
	double DisRecord = FVector::Distance(OL, ActorLocation);

	bHit = GetWorld()->LineTraceSingleByObjectType(ClimbHitResult, ActorLocation,
	                                               ActorLocation + (GetActorForwardVector() + FVector(0, 0, 0.5)) * 200,
	                                               ECC_WorldStatic, CollisionParams);
	if (!bHit)
	{
		// Detect and Climb Up Ledge
		return;
	}
	FVector OLE = ClimbHitResult.ImpactPoint; // would be the end location of FindClimbRotation Call
	FVector OLE_Norm = ClimbHitResult.Normal;

	OL_Norm = FMath::VInterpTo(OL_Norm, OLE_Norm, GetWorld()->GetDeltaSeconds(), 5.f);

	DrawDebugPoint(GetWorld(), OLE, 10.f, FColor::Blue, false, 0.f);
	DrawDebugPoint(GetWorld(), OL, 10.f, FColor::Yellow, false, 0.f);

	FRotator NewRotator = FindClimbRotation(OL, OLE, OL_Norm);
	FRotator DeltaRotator = NewRotator - GetActorRotation();


	DeltaRotator.Normalize(); // Normalize the rotator to ensure it is within the range [-180, 180]
	DeltaRotator.Yaw = FMath::Abs(DeltaRotator.Yaw);
	DeltaRotator.Pitch = FMath::Abs(DeltaRotator.Pitch);
	DeltaRotator.Roll = FMath::Abs(DeltaRotator.Roll);

	if (FMath::Fmod(DeltaRotator.Yaw, 180.f) > 8.f
		|| FMath::Fmod(DeltaRotator.Pitch, 180.f) > 8.f
		|| FMath::Fmod(DeltaRotator.Roll, 180.f) > 8.f
		|| DisRecord >= 75.f)
	{
		/* debug*/
		FString DeBugMeg2 = FString::Printf(TEXT("Rotator.Yaw: %s"), *FString::SanitizeFloat(abs(DeltaRotator.Yaw)));
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, DeBugMeg2);
		FString DeBugMeg = FString::Printf(
			TEXT("RotatorDifference: %s"), *FString::SanitizeFloat(FMath::Fmod(abs(DeltaRotator.Yaw), 180.f)));
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, DeBugMeg);
		/* debug*/
		NewRotator = FMath::RInterpTo(GetActorRotation(), NewRotator, GetWorld()->GetDeltaSeconds(), 2.f);
		SetActorRotation(NewRotator);
		GetCharacterMovement()->AddForce(GetActorForwardVector() * 150000);
	}
	ClimbRotator = NewRotator;
}

void AAwCharacter::SwitchToClimbingMode()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	bIsClimbing = true;
	GetMesh()->SetRelativeLocation(FVector(-20, 0, -120));
	GetCapsuleComponent()->SetCapsuleHalfHeight(60);
}

void AAwCharacter::RidOffClimbingMode()
{
	if (bIsClimbing)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		bIsClimbing = false;
		SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
		GetMesh()->SetRelativeLocation(FVector(0, 0, -95));
		GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	}
}

void AAwCharacter::MoveForwardWhenClimbing(float Values)
{
	FHitResult HitResult;
	const FRotator Rotation = FRotator(0, ClimbRotator.Yaw, 0);
	const FVector Direction = FRotationMatrix(ClimbRotator).GetScaledAxis(EAxis::Z);
	const FVector StartLocation = GetActorLocation() + FRotationMatrix(ClimbRotator).GetScaledAxis(EAxis::Z) * 100 *
		Values;
	const FVector EndLocation = StartLocation + FRotationMatrix(Rotation).GetScaledAxis(EAxis::X) * 100;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
	                                                     10, 35, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	                                                     false,
	                                                     TArray<AActor*>({this}), EDrawDebugTrace::ForOneFrame,
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
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);


	const FRotator Rotation = FRotator(0, ClimbRotator.Yaw, 0);
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
	const FVector StartLocation = GetMesh()->GetSocketLocation("Chest") + FRotationMatrix(ClimbRotator).
		GetScaledAxis(EAxis::Y) * 50 * Values;
	const FVector EndLocation = StartLocation + FRotationMatrix(Rotation).GetScaledAxis(EAxis::X) * 100;

	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation,
	                                                     10, 40, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
	                                                     false,
	                                                     TArray<AActor*>({this}), EDrawDebugTrace::ForOneFrame,
	                                                     ClimbHitResult,
	                                                     true);

	if (!bHit)
	{
		return;
	}
	FVector OL_N_NEW = ClimbHitResult.Normal;

	FVector ActorLocation = GetMesh()->GetSocketLocation("Chest");
	FVector OL_N_OLD = -GetActorForwardVector();
	bHit = GetWorld()->LineTraceSingleByObjectType(ClimbHitResult, ActorLocation,
	                                               ActorLocation + GetActorForwardVector() * 200,
	                                               ECC_WorldStatic, CollisionParams); // 忽略当前角色自身，避免与自身发生碰撞

	ActorLocation = GetMesh()->GetSocketLocation("Chest") + GetActorRightVector() * Values;
	float Distance_pre = FVector::Distance(ActorLocation, ClimbHitResult.ImpactPoint);
	bHit = GetWorld()->LineTraceSingleByObjectType(ClimbHitResult, ActorLocation,
	                                               ActorLocation + GetActorForwardVector() * 200,
	                                               ECC_WorldStatic, CollisionParams);

	if (OL_N_OLD == OL_N_NEW)
	{
		AddMovementInput(Direction, Values);
		return;
	}
	float Deta_Distance = FVector::Distance(ActorLocation, ClimbHitResult.ImpactPoint) - Distance_pre;
	if (Deta_Distance > 0)
		Values += tan(FMath::Acos(FVector::DotProduct(OL_N_OLD, OL_N_NEW))) * Deta_Distance * (Values > 0 ? 1 : -1);
	AddMovementInput(Direction, Values);
	FRotator NewRotator = FRotationMatrix::MakeFromX(-OL_N_NEW).Rotator();
	NewRotator = FMath::RInterpTo(GetActorRotation(), NewRotator, GetWorld()->GetDeltaSeconds(), 2.f);
	SetActorRotation(NewRotator);
}

bool AAwCharacter::DetectAndClimbUp()
{
	// Detect any obstacle on the edge
	FHitResult HitResult;
	const FRotator Rotation = FRotator(0, ClimbRotator.Yaw, 0);
	const FVector StartLocation = GetActorLocation() + FRotationMatrix(ClimbRotator).GetScaledAxis(EAxis::Z) * 150;
	const FVector EndLocation = StartLocation + FRotationMatrix(Rotation).GetScaledAxis(EAxis::X) * 50;

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

	if (GetWorld()->LineTraceSingleByChannel(HitResult, EndLocation, EndLocation + FVector(0, 0, -100), ECC_Visibility,
	                                         CollisionParams))
	{
		// can land
		ClimbingUp.Broadcast(HitResult.ImpactPoint);
		return true;
	}
	return false;
}

void AAwCharacter::OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth,
                                  float Change)
{
	if (UAWAttributeComp* PlayerAttribute =  GetOwningAttribute())
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
	if (UAWAttributeComp* PlayerAttribute =  GetOwningAttribute())
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
	// SetActorRelativeRotation(FRotator(0, 90.f + GetActorRotation().Yaw,0));
	Super::Jump();
	if (DetectWall())
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // 忽略当前角色自身，避免与自身发生碰撞
		bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, GetMesh()->GetSocketLocation("Hips"),
		                                                    GetMesh()->GetSocketLocation("Hips") +
		                                                    GetActorForwardVector() * 100, ECC_WorldStatic,
		                                                    CollisionParams);

		if (bHit)
		{
			SwitchToClimbingMode();
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			                                 "Actor Forward  Before: " + GetActorForwardVector().ToString());

			FRotator NewRotator = FRotationMatrix::MakeFromX(-HitResult.ImpactNormal).Rotator();
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black,
			                                 "Hit normal: " + (HitResult.ImpactNormal * -1).ToString());
			SetActorLocation(HitResult.ImpactPoint + HitResult.Normal * 45);
			SetActorRotation(NewRotator);
			ClimbRotator = NewRotator;
		}
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
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AAwCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &AAwCharacter::PrimaryInterat);
	PlayerInputComponent->BindAction("BlackHoleAbility", IE_Pressed, this, &AAwCharacter::BlackHoleAbility);
	PlayerInputComponent->BindAction("CancelClimbing", IE_Pressed, this, &AAwCharacter::RidOffClimbingMode);
}
