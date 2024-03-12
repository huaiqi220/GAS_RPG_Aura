// zhuz1's learning project


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// Super::BindCallbacksToDependencies();
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for(auto& Pair : AS->TagsToAttributes)
	{
		/*遍历这个属性集合中所有的属性，并为每个属性的变化绑定一个Lambda表达式。这个Lambda表达式会在属性值改变时被调用。*/
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
        	[this, Pair](const FOnAttributeChangeData& Data)
			{
        		BroadcastAttributeInfo(Pair.Key, Pair.Value());
        	}
        );
	}

}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	// Super::BroadcastInitialValues();
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	/*FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);*/
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
