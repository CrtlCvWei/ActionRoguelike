// Fill out your copyright notice in the Description page of Project Settings.


#include "AwCharacter.h"

#include "AwActionComponent.h"
#include "AWAttributeComp.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
	if (ensure(this->AttributeComp))
	{
		AttributeComp->OnHealthChange.AddDynamic(this,&AAwCharacter::OnHealthChange);
	}
}

void AAwCharacter::Init_Paramters()
{
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
	// Attributes
	AttributeComp = CreateDefaultSubobject<UAWAttributeComp>("AttributeComp");

	// Actions
	ActionComp = CreateDefaultSubobject<UAwActionComponent>("ActionComp");
	
	GetCharacterMovement()->MaxWalkSpeed = this->NormalMoveSpeed;
	GetCharacterMovement()->GravityScale = this->GravityScale;
	JumpMaxHoldTime = 1.0f;
	GetCharacterMovement()->JumpZVelocity = this->JumpZVelocity;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->BrakingFriction = this->BrakingFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = this->BrakingDecelerationWalking;


	Init_BeginPlay();
}

void AAwCharacter::Init_BeginPlay()
{
	BaseEyeHeight = 103.;

	//
	AttributeComp->SetHealth(100,this);
}

// Called when the game starts or when spawned
void AAwCharacter::BeginPlay()
{
	Super::BeginPlay();
	//init
	Init_BeginPlay();
}

// Called every frame
void AAwCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAwCharacter::BeginSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = this->MaxMoveSpeed;
	// ActionComp->StartActionByName(this,"Sprint");
}

void AAwCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = this->NormalMoveSpeed;
	// ActionComp->StopActionByName(this,"Sprint");
}

void AAwCharacter::MoveForward(float Values)
{
	if ((Controller != nullptr) && (Values != 0.0f))
	{
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
		// Find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		//UE_LOG(LogTemp, Warning, TEXT("Yaw Value: %f"), Rotation.Yaw);
		// Get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
		// Add movement in that direction
		AddMovementInput(Direction, Values);
	}
}

void AAwCharacter::UpdateJumpKeyHoldTime()
{
	JumpKeyHoldTime += 0.1f;
}

void AAwCharacter::OnHealthChange(AActor* InstigatorActor, UAWAttributeComp* AttributeComponent, float NewHealth,
                                  float Change)
{
	if (this->AttributeComp)
	{
		if (!this->AttributeComp->isAlive())
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			// ragdoll
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Ignore);
			FTimerHandle DeadDelay;
			GetWorldTimerManager().SetTimer(DeadDelay, this, &AAwCharacter::Deadth, 2.f);
			DisableInput(PC);
			
		}
	}
}

void AAwCharacter::HealSelf(float v)
{
	AttributeComp->SetHealth(v,this);
}

void AAwCharacter::Jump()
{
	Super::Jump();
	// 启动定时器，每帧调用UpdateJumpKeyHoldTime函数
	GetWorldTimerManager().SetTimer(JumpTimerHandle, this, &AAwCharacter::UpdateJumpKeyHoldTime, 0.1f, true);
	if (JumpKeyHoldTime >= JumpHoldTimeThreshold)
	{
		// jump higher
		GetCharacterMovement()->JumpZVelocity = 1000.f;
	}
	else
	{
		//normal jump
		GetCharacterMovement()->JumpZVelocity = 900.f;
	}
}

void AAwCharacter::StopJumping()
{
	Super::StopJumping();
	GetWorldTimerManager().ClearTimer(JumpTimerHandle);
}

FRotator AAwCharacter::GetProjectileRotation(FVector HandLocation)
{
	/* 从摄像机中心进行射线检测，检测视线处是否有可见物体碰撞，根据碰撞情况修正projectile运动姿态、方向*/
	FVector CameraLocation;
	FVector EyeEndLocation;
	FRotator CameraRotation;
	FRotator ProjectileRotation;
	float longest_dis = 10000.f;

	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
	EyeEndLocation = CameraLocation + longest_dis * CameraRotation.Vector();

	// 创建射线参数
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 忽略当前角色自身，避免与自身发生碰撞

	// 执行射线检测
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EyeEndLocation, ECC_Visibility,
	                                                 CollisionParams);
	if (bHit)
	{
		// get the hit point
		EyeEndLocation = HitResult.ImpactPoint;
	}
	// fix the projectile rotation
	ProjectileRotation = FRotationMatrix::MakeFromX(EyeEndLocation - HandLocation).Rotator();
	return ProjectileRotation;
}

void AAwCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if (ensureAlways(ClassToSpawn))
	{
		const FVector HandLocation = this->GetMesh()->GetSocketLocation("Right-wrist");
		const FRotator ProjectileRotation = this->GetProjectileRotation(HandLocation);
		FTransform LocaTM = FTransform((FRotator)ProjectileRotation,
		                               (FVector)HandLocation);
		FActorSpawnParameters SpawnParams;
		/** Actor will spawn in desired location, regardless of collisions. */
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
		// Spawn the projectile (Magic balls or any other things...)
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, LocaTM, SpawnParams);
	}
}

void AAwCharacter::PrimaryAttack()
{
	this->ActionComp->StartActionByName(this,"PrimaryAttack");
}

// void AAwCharacter::PrimaryAttack_TimeElasped()
// {
// 	this->SpawnProjectile(ProjuctileClass);
// }

void AAwCharacter::BlackHoleAbility_TimeElasped()
{
	this->SpawnProjectile(BlackHoleClass);
}

void AAwCharacter::BlackHoleAbility()
{
	if (ensure(this->BlackHoleClass) && ensure(AttackAni))
	{
		// Attac Animation
		PlayAnimMontage(AttackAni, 1.);
		GetWorldTimerManager().SetTimer(ProjectileSpawnHandle, this, &AAwCharacter::BlackHoleAbility_TimeElasped,
		                                0.25f);
	}
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
}


