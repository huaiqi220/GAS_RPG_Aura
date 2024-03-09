// zhuz1's learning project

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	/** Enemy Interface */
	void HighlightActor() override;
	void UnHighlightActor() override;
	/** end Enemy Interface */

protected:
	
	// UPROPERTY(BlueprintReadOnly)
	// bool bHighlighted = false;
	// UPROPERTY(EditDefaultsOnly)
	// int customDepthValue = 250;
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;
	
	
};
