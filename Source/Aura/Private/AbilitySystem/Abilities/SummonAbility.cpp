// Lavid


#include "AbilitySystem/Abilities/SummonAbility.h"

TArray<FVector> USummonAbility::GetSpawnLocations()
{
	const FVector ForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const float DeltaSpread = SpawnSpread / NumMinions;

	//LeftOfSpread就是Shama召唤的左边界
	const FVector LeftOfSpread = ForwardVector.RotateAngleAxis(-SpawnSpread / 2, FVector().UpVector);
	//召唤点的坐标的数组
	TArray<FVector> SpawnLocations;
	for (int i = 0; i < NumMinions; i++) 
	{
		//把召唤的范围切成均匀的角度
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector().UpVector);
		//获取每一个均匀方向上的随机点
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility );
		if (HitResult.bBlockingHit)
		{
			ChosenSpawnLocation = HitResult.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);	
	}
	return SpawnLocations;
}

TSubclassOf<APawn> USummonAbility::GetRandomMinionClass()
{
	int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
