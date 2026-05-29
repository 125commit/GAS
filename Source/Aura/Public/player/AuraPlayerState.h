// Lavid

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStatChange, int32)

class UAbilitySystemComponent;
class UAttributeSet;
class ULevelUpInfo;

/**
 * 
 */

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();

	FPlayerStatChange OnXPChangeDelegate;
	FPlayerStatChange OnLevelChangeDelegate;
	FPlayerStatChange OnAttributePointsChangeDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }

	UFUNCTION(BlueprintCallable)
	void SetXP(int32 InXP);

	UFUNCTION(BlueprintCallable)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintCallable)
	void SetLevel(int32 InLevel);

	UFUNCTION(BlueprintCallable)
	void AddToLevel(int32 InLevel);

	UFUNCTION(BlueprintCallable)
	void SetAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintCallable)
	void AddToAttributePoints(int32 InAttributePoints);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;


protected:
	

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 1;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttribuetPoints)
	int32 AttributePoints = 1;

	UFUNCTION()
	void OnRep_AttribuetPoints();

};
