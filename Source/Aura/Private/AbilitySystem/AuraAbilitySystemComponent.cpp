// zhuz1's learning project


#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	// GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Red, TEXT("Effect Applied"));
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
	//
	// for(const FGameplayTag& Tag : TagContainer)
	// {
	// 	//TODO: Broadcast the tag to the widget Controller
	// 	const FString Msg = FString::Printf(TEXT("GE Tag : %s"), *Tag.ToString());
	// 	GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Red, Msg);
	// }
}
