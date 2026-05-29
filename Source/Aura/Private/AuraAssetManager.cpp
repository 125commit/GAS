// Lavid


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager::UAuraAssetManager()
{
	UE_LOG(LogTemp, Warning, TEXT(">>> UAuraAssetManager 构造函数被调用了！ <<<"));
}

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAsssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAsssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAuraGameplayTags::InitializeNativeGameplayTags();
	//UAbilitySystemGlobals::Get().InitGlobalData();
}


