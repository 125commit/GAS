// Lavid


#include "AI/MyBTService_FindNearestPlayer.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BTFunctionLibrary.h"

void UMyBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> TargetWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, TargetWithTag);

	//获取并设置 最近距离、最近玩家 的黑板键
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	for (AActor* Target : TargetWithTag)
	{
		if (IsValid(Target) && IsValid(OwningPawn))
		{
			float Distance = OwningPawn->GetDistanceTo(Target);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Target;
			}
		}
	}
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);


}
