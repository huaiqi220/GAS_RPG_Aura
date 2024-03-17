// zhuz1's learning project


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	/* 返回的是我们自定义的Aura上下文 */
	return new FAuraGameplayEffectContext();
}
