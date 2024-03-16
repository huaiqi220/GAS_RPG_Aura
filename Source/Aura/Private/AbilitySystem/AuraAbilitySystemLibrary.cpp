// zhuz1's learning project


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}

/* 不能替换， 替换报错，搞了我一个小时 */
UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

/*
* 你的问题涉及到UE4/UE5的游戏编程，尤其是使用Gameplay Ability System (GAS)进行多人游戏开发时的一些特殊情况。从你提供的代码和问题描述来看，问题似乎出在`GetCharacterClassInfo`方法的使用上。你尝试通过封装逻辑到`GetCharacterClassInfo`来重用代码，但这导致了多人联机场景下的空指针错误。让我们来分析可能的原因：

### 1. 网络复制和时机问题

在多人游戏中，服务器和客户端之间的数据同步是非常重要的。当你调用`GetCharacterClassInfo`方法时，你依赖于`AAuraGameModeBase`对象的存在和其`CharacterClassInfo`成员的有效性。如果这个方法在游戏模式还没有完全初始化或在某些数据还没从服务器同步到客户端的时候被调用，就可能会导致空指针错误。

### 2. `GetGameMode`的行为

`UGameplayStatics::GetGameMode`只在服务器端返回有效的游戏模式对象。在客户端调用时，它会返回`nullptr`，因为客户端不直接访问游戏模式。如果你在客户端代码中调用了`GetCharacterClassInfo`，并期望它能正常工作，这就是问题所在。

### 3. 封装引入的逻辑更改

虽然你提到封装的逻辑和原始代码逻辑“一模一样”，但实际上，将代码移动到一个新的函数中可以改变其执行上下文（特别是关于何时何地执行这段逻辑）。如果封装后的函数在不同的时间点或在客户端代码路径中被调用，这可能是导致问题的根源。

### 解决建议

- **确保网络同步**：确保在调用`GetCharacterClassInfo`之前，相关的游戏模式和角色类信息已经从服务器同步并在客户端上可用。
- **检查执行上下文**：仔细检查`GetCharacterClassInfo`被调用的地点。确认这些调用只在服务器执行，或者你有条件地处理客户端逻辑。
- **调试和日志记录**：在`GetCharacterClassInfo`和调用它的函数中添加更多的日志记录，以确定何时以及在什么上下文中发生了空指针错误。UE4/UE5的日志系统可以帮助你识别问题发生的确切时间点和条件。

### 具体代码建议

- 修改`GetCharacterClassInfo`以检查它是否在客户端上被错误地调用，并添加适当的日志记录来警告开发者。
- 如果这个函数确实需要在客户端被调用，考虑引入一个安全的方式来处理客户端和服务器之间的数据同步。

理解多人游戏的网络行为和数据同步机制对于解决这类问题至关重要。希望这些建议能帮助你解决问题。
 */