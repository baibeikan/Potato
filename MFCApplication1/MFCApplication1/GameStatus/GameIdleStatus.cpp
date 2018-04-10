#include "stdafx.h"
#include "GameIdleStatus.h"
#include "GameSystem.h"

void GameIdleStatus::OnEnter()
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	pGameSystem->EnterIdle();

	//pGameSystem->DestroyStatus(GAME_LOAD_STATUS);
}

void GameIdleStatus::OnUpdate(float deltaTime)
{
}

void GameIdleStatus::OnLeave()
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	pGameSystem->LeaveIdle();
}

