// Lavid


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const int32 Damage = ScalableDamage.GetValueAtLevel(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			//Titile
			"<Title>FIRE BOLT</>\n\n"

			//Level
			"<Small>Level: </><Level>%d</>\n"
			//ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			//Coodown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			"<Default>Launches a bolt of fire, exploding on impact and dealing:</>\n"

			//Damage
			"<Damage>%d</>"
			"<Default>fire damage with a chance to burn</>"),

			//Values
			Level,
			ManaCost,
			Cooldown,
			Damage);
	}
	else
	{
		return FString::Printf(TEXT(
			//Titile
			"<Title>FIRE BOLT</>\n\n"

			//Level
			"<Small>Level: </><Level>%d</>\n"
			//ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			//Coodown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			//Number of Projectiles
			"<Default>Launches %d bolt of fire, exploding on impact and dealing:</>\n"

			//Damage
			"<Damage>%d</>"
			"<Default>fire damage with a chance to burn</>"),

			//Values

			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, ProjectilesNum),
			Damage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	const int32 Damage = ScalableDamage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		//Titile
		"<Title>NEXT LEVEL: </>\n\n"

		//Level
		"<Small>Level: </><Level>%d</>\n"
		//ManaCost
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		//Coodown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		//Number of Projectiles
		"<Default>Launches %d bolt of fire, exploding on impact and dealing: </>\n"

		//Damage
		"<Damage>%d</>"
		"<Default>fire damage with a chance to burn</>"),

		//Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, ProjectilesNum),
		Damage);
}
