#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemyController.h"

AEnemyController::AEnemyController()
{
	Helpers::CreateComponent<UBlackboardComponent>(this, &BlackboardComponent, "BlackBoardComponent");
	check(BlackboardComponent);
	Helpers::CreateComponent<UBehaviorTreeComponent>(this, &BehaviorTreeComponent, "BehaviorTreeComponent");
	check(BehaviorTreeComponent);
	
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn == nullptr) return;

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(InPawn);
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}
}
