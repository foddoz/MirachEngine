#pragma once
#include "IEnemyStateLogic.h"
#include "enemy.h"
#include "CommonStates.h"
#include <iostream>
#include "../Graphics/Animation/MD2.hpp"
#include "../Scripts/health.hpp"

class EnemyState : public IEnemyStateLogic 
{
public:

    void OnAttackEnter(Enemy* e) override;
    void OnAttackExecute(Enemy* e) override;
    void OnAttackExit(Enemy* e) override;

    void OnFleeEnter(Enemy* e) override;
    void OnFleeExecute(Enemy* e) override;
    void OnFleeExit(Enemy* e) override;

    void OnPatrolEnter(Enemy* e) override;
    void OnPatrolExecute(Enemy* e) override;
    void OnPatrolExit(Enemy* e) override;

    void OnChaseEnter(Enemy* e) override;
    void OnChaseExecute(Enemy* e) override;
    void OnChaseExit(Enemy* e) override;

    void OnDeathEnter(Enemy* e) override;
    void OnDeathExecute(Enemy* e) override;
    void OnDeathExit(Enemy* e) override;

    void OnPainEnter(Enemy* e) override;
    void OnPainExecute(Enemy* e) override;
    void OnPainExit(Enemy* e) override;

};