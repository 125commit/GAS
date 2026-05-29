// Lavid


#include "AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* CooldownChange = NewObject<UWaitCooldownChange>();
	CooldownChange->ASC = AbilitySystemComponent;
	CooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		CooldownChange->EndTask();
		return nullptr;
	}

	//To know when a cooldown effect has ended (cooldown Tag has been removed)
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
		CooldownChange, 
		&UWaitCooldownChange::CooldownTagChange);

	//to know when a cooldown effect has been applied
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(CooldownChange, &UWaitCooldownChange::OnActivateEffectAdded);

	return CooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC)) return;
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChange(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActivateEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActivateEffectHandle)
{
	//获取 GE 的标签 (AssetTags和GrantedTags)
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	//如果GE有CooldownTag（这是一个变量，WaitForCooldownChange函数中设置）
	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		//获取Effect的剩余时间
		FGameplayEffectQuery EffectQueery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemainingArray = ASC->GetActiveEffectsTimeRemaining(EffectQueery);

		//计算数组中的最大时间（就是冷却的剩余时间）
		if (TimesRemainingArray.Num() > 0)
		{
			float TimeRemaining = TimesRemainingArray[0];
			for (int i = 0; i < TimesRemainingArray.Num(); i++)
			{
				if (TimesRemainingArray[i] > TimeRemaining)
				{
					TimeRemaining = TimesRemainingArray[i];
				}
			}
			//最后广播
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
