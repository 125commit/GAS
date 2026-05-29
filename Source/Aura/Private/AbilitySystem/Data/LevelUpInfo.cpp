// Lavid


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(float XP) const
{
    int32 Level = 1;
    bool bSearching = true;
    while (bSearching)
    {
        //LevelUpInformation[0] = Nothing
        //LevelUpInformation[1] = Level 1 information
        if (LevelUpInformation.Num()-1 <= Level) return Level;

        if (XP >= LevelUpInformation[Level].LevelUpRequirement)
        {
            ++Level;
        }
        else
        {
            bSearching = false;
        }
    }
    return Level;
}
