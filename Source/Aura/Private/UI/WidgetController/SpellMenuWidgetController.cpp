// Lavid


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				//Set Boolean that determine Button Enabled
				SelectedAbility.Status = StatusTag;

				bool bEnableSpendPoints = false;
				bool bEnableEquip = false;
				SetButtonEnable(CurrentSpellPoints, StatusTag, bEnableSpendPoints, bEnableEquip);

				//Set Description
				FString Description;
				FString NextLevelDexcription;
				GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDexcription);

				SpellGobeSelecteDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDexcription);
			}

			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});

	GetAuraPS()->OnSpellPointsChangeDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(SpellPoints);

			//Set Boolean that determine Button Enabled
			CurrentSpellPoints = SpellPoints;

			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			SetButtonEnable(CurrentSpellPoints, SelectedAbility.Status, bEnableSpendPoints, bEnableEquip);

			//Set Description
			FString Description;
			FString NextLevelDexcription;
			GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDexcription);

			SpellGobeSelecteDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDexcription);

		});

	//bind to AbilityEquippedDelegate
	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquip);

}

void USpellMenuWidgetController::SpellGobeSelected(const FGameplayTag& AbilityTag)
{
	
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(AbilityType);
		bWaitingForEquipSelection = false;
	}
	

	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();

	//Set some boolean to ensure Tag is useful
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);

	FGameplayAbilitySpec* AbilitySpec =  GetAuraASC()->GetSpecFromTag(AbilityTag);
	const bool SpecValid = (AbilitySpec != nullptr);
	const bool bTagValid = AbilityTag.IsValid();

	//Set Status(Locally)
	FGameplayTag AbilityStatus;
	if (!bTagValid || bTagNone || !SpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;

	//Set Boolean that determine Button Enabled
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	SetButtonEnable(SpellPoints, AbilityStatus, bEnableSpendPoints, bEnableEquip);

	//Set Description
	FString Description;
	FString NextLevelDexcription;
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDexcription);

	SpellGobeSelecteDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDexcription);
}

void USpellMenuWidgetController::SpendSpellPoint()
{
	GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipDelegate.Broadcast(AbilityType);
		bWaitingForEquipSelection = false;
	}

	SelectedAbility.Ability = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status = FAuraGameplayTags::Get().Abilities_Status_Locked;

	SpellGobeSelecteDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	WaitForEquipDelegate.Broadcast(AbilityType);

	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedStatus =  GetAuraASC()->GetStatusFromAbiltyTag(SelectedAbility.Ability);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Eligible))
	{
		SelectedEquippedSlot = GetAuraASC()->GetInputTagFromAbiltyTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::EquippedRowGlobePressed(const FGameplayTag& AbilityType, const FGameplayTag& EquippedSlotTag)
{
	//错误的情况，直接return
		//保证按下了 Equip 按钮
	if (!bWaitingForEquipSelection) return;

		//防止把OffensiveSpell放到Passive的槽中
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.Ability).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	/* assign or reassign spell to slot */
	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, EquippedSlotTag);

}

void USpellMenuWidgetController::OnAbilityEquip(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& PreviousSlot, const FGameplayTag& Slot)
{
	bWaitingForEquipSelection = false;

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	//处理“旧槽位”的状态广播（清空旧槽位信息）
	/* Only if equipped an already_equipped spell */
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	//处理“新槽位”的状态广播（设置新技能信息）
	FAuraAbilityInfo Info  = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	//停止播放Selection动画
	StopWaitingForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType);

	//取消Tree中的SpellGlobe的选中
	SpellGlobeReassignDelegate.Broadcast(AbilityTag);
	GlobeDeselect();

}

void USpellMenuWidgetController::SetButtonEnable(int32 SpellPoints, const FGameplayTag& AbilityStatus, bool& bEnableSpendPoints, bool& bEnableEquip)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bEnableEquip  = false;
		if (SpellPoints > 0)
		{
			bEnableSpendPoints = true;
		}
	}

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bEnableEquip = true;
		if (SpellPoints > 0)
		{
			bEnableSpendPoints = true;
		}
	}

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bEnableEquip = true;
		if (SpellPoints > 0)
		{
			bEnableSpendPoints = true;
		}
	}

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
		bEnableEquip = false;
	}

}



