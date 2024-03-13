// zhuz1's learning project

#include "Player/AuraPlayerController.h"

// 因为少写一个s，搞了半天
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	// 本质 实体在服务器上发生变化时，将在服务器上的更改复制到所有客户端

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	
	
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 断言AuraContext已经正确设置
	check(AuraContext);

	// 单例，持续时间内只有一个
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext,0);
	}
	
	// 展示鼠标，选择光标
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	/*UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);*/
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig,this,  &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();

	
	
}

/*TODO: 不在样条上先走上样条，走上样条之后走向目的地*/
void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}


void AAuraPlayerController::CursorTrace()
{
	
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	/*调用后处理边缘加红*/
	if(LastActor != ThisActor)
	{
		if(LastActor)LastActor->UnHighlightActor();
		if(ThisActor)ThisActor->HighlightActor();
	}

}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if(FollowTime <= ShortPressThreshold && ControlledPawn)	
		{
			UNavigationPath* NavPath =  UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination);
			if(NavPath)
			{
				Spline->ClearSplinePoints();
				for(const FVector& PointLoc : NavPath->PathPoints)
				{
					/* 在样条曲线上加点 */
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					/*DrawDebugSphere(GetWorld(),PointLoc,8.f,8,FColor::Green,false,5.f);*/
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num()-1];
				bAutoRunning = true;
				
			}
		}
			FollowTime = 0.f;
			bTargeting = false;
	}


}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	if (bTargeting  || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;
		
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}


UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}


/**
 * How it convert the rotation
* 在解释这个函数在RPG游戏中的作用之前，让我们先理解 `InputActionValue.Get<FVector2D>()` 和 `AddMovementInput(ForwardDirection, InputAxisVector.Y)` 这两个调用的含义：
1. **`InputActionValue.Get<FVector2D>()`**:
   - `InputActionValue` 是一个封装了玩家输入的对象。当玩家通过输入设备（如键盘、鼠标、游戏手柄）进行操作时，这些操作被转换为 `InputActionValue`。
   - `.Get<FVector2D>()` 是从这个输入值中获取一个二维向量（`FVector2D`）的方法。在RPG游戏的上下文中，这个二维向量通常代表方向控制 — 比如，使用键盘的WASD键或游戏手柄的摇杆来控制角色的移动方向。
	 - `X` 分量通常代表水平方向（左/右）。
	 - `Y` 分量通常代表垂直方向（前/后）。
2. **`AddMovementInput(ForwardDirection, InputAxisVector.Y)`**:
   - `AddMovementInput` 是一个向控制的 `Pawn`（例如，游戏中的角色）添加移动指令的方法。
   - `ForwardDirection` 是一个 `FVector` 类型，代表角色面向的前方方向。这是基于角色当前的朝向（通常是相机或控制器的朝向）计算出来的。
   - `InputAxisVector.Y` 是从玩家输入中获取的垂直方向的分量（如前进或后退的指令）。
	 - 将这个值作为第二个参数传递给 `AddMovementInput` 方法意味着你正在告诉游戏：“按照 `ForwardDirection` 方向移动角色，移动的强度或速度与 `InputAxisVector.Y` 的值成比例。”
### 在RPG游戏中的应用
在一个典型的RPG游戏中，玩家使用键盘、鼠标或游戏手柄来控制角色的移动。例如，按下键盘上的 'W' 键可能意味着向前移动，'S' 键意味着向后移动。这个 `Move` 函数就是用来处理这些输入并将它们转换为游戏世界中的角色移动。
- 当玩家按下 'W' 键（假设这是向前移动），`InputAxisVector.Y` 会得到一个正值。
- 当玩家按下 'S' 键（向后移动），`InputAxisVector.Y` 会得到一个负值。
- `ForwardDirection` 确定了角色当前面向的方向。
- `AddMovementInput(ForwardDirection, InputAxisVector.Y)` 根据这些信息计算并应用实际的移动。


因此，这个函数允许角色根据玩家的输入（移动方向和强度）在游戏世界中进行相应的移动。
 * @param InputActionValue 
 */
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	} 
}

