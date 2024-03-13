// zhuz1's learning project


#include "AuraAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager =  Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAuraGameplayTags::InitializeNativeGameplayTags();
	// 使用TargetData这个数据结构之前必须首先调用这个
	UAbilitySystemGlobals::Get().InitGlobalData();
}
