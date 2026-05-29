// Lavid


#include "AbilitySystem/Data/AbilityInfo.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFound)
{
    for (const FAuraAbilityInfo& Info : AbilityInfomation)
    {
        if (Info.AbilityTag == AbilityTag)
        {
            return Info;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't find Info for AbilityTag[%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
    }

    return FAuraAbilityInfo();
}
