// zhuz1's learning project


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);
	
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

/*
*在UE5（Unreal Engine 5）中，GameAbilitySystem是一个用于处理游戏中角色技能和状态的复杂系统，
*其中包含了预测机制来优化多人游戏中的网络同步。预测机制、预测键、回滚等概念是为了解决网络延迟带来的不一致性和同步问题。
*理解这些内容需要对网络编程和UE5的游戏编程模型有一定的了解。下面我会简要介绍这些概念以及它们是如何工作的。
### 预测机制
UE5的GameAbilitySystem利用预测机制来优化网络游戏中的响应性。当玩家执行一个动作，比如发动一个技能时，系统不会立即等待服务器的确认，
而是在本地先行预测执行结果并立即反馈给玩家，从而减少感知到的延迟。服务器在收到动作请求后，会执行相同的逻辑，并将最终结果同步给所有客户端。
### 预测键（Prediction Key）
预测键是预测机制中的一个核心概念。它是一个标识符，用于将一系列的预测操作和服务器确认后的结果进行匹配。
当客户端发送一个带有预测键的动作给服务器时，服务器执行该动作并返回结果时也会包含这个预测键，以便客户端确认哪些预测动作已经被服务器验证。
### 回滚（Reconciliation）
回滚是当客户端的预测与服务器的验证结果不一致时采取的措施。这时，客户端会根据服务器的最终结果调整本地状态，如
果有必要，还会重新执行一些动作来确保客户端和服务器的状态一致。这个过程是自动的，对玩家而言是透明的。
### GameplayPrediction.h
`GameplayPrediction.h`是UE5 GameAbilitySystem中一个关键的头文件，定义了与预测相关的类、
结构和枚举。它包含了实现预测逻辑所需的基本构建块，比如预测键的生成和管理、预测系统的注册机制等。
理解`GameplayPrediction.h`中的内容通常涉及以下几个方面：
- **预测键的生成和使用**：如何生成预测键，以及如何在发起和处理预测请求时使用它。
- **预测模型的注册和解析**：如何在系统中注册可以被预测的动作，以及如何解析这些动作的预测结果。
- **状态同步和回滚处理**：包含状态同步的机制，以及当预测失败时如何回滚到一个一致的状态。
理解这些概念和机制需要时间和实践。通常，熟悉UE5文档中关于网络编程和GameAbilitySystem的指南，以及分析示例项目中的实现，
都是很好的学习方法。此外，社区论坛和教程也是宝贵的资源，可以帮助你深入理解这一复杂但强大的系统。
 *
 * 
 */
