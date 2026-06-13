// Lavid

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCal_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCal_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCal_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput)const override;

	void DetermineDebuff(const FGameplayEffectSpec& GESpec, 
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FAggregatorEvaluateParameters& EvaluationParamters,
		const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDef) const;
};
