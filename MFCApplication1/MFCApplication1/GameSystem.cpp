#include "stdafx.h"
#include "GameSystem.h"
#include "GameLoadStatus.h"
#include "GameIdleStatus.h"
#include "GameRunStatus.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = NULL; }}
#define SAFE_DELETE(ptr)  { if(ptr) { delete ptr; ptr = NULL; }}

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position; // The position
	D3DXVECTOR2 tuv;
};
// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


GameSystem::GameSystem()
{
	m_pDirect9Render = NULL;
}

GameSystem::~GameSystem()
{
	Release();
}

void GameSystem::Inital(HWND hWnd)
{
	m_pDirect9Render = new Direct9Render(GetWindowHandle());
	m_pDirect9Render->Inital();

	for (int i = 0; i < GAME_MAX_STATUS; ++i)
	{
		AddStatus(i);
	}
	SwitchStatus(GAME_LOAD_STATUS);
	// 加载界面
}

void GameSystem::Update(float deltaTime)
{
	__super::Update(deltaTime);
}

void GameSystem::Render()
{
	if (GetCurrentStatus() == GAME_IDLE_STATUS)
		return;

	m_pDirect9Render->Render();
}

void GameSystem::Release()
{
	__super::Release();

	SAFE_DELETE(m_pDirect9Render);
}

StatusBase* GameSystem::CreateStatus(int nStatusID)
{
	StatusBase* pNewStatus = NULL;
	switch (nStatusID)
	{
	case GAME_LOAD_STATUS:
		pNewStatus = new GameLoadStatus(this);
		break;
	case GAME_RUN_STATUS:
		pNewStatus = new GameRunStatus(this);
		break;
	case GAME_IDLE_STATUS:
		pNewStatus = new GameIdleStatus(this);
		break;
	default:
		break;
	}
	return pNewStatus;
}

BOOL GameSystem::PreTranslateMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (__super::InputProcess(message, wParam, lParam))
		return TRUE;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == 'A')
		{
			int nCurStatus = GetCurrentStatus();
			if (nCurStatus == GAME_IDLE)
				SwitchStatus(GAME_RUN);
			else if (nCurStatus == GAME_RUN)
				SwitchStatus(GAME_IDLE);
			return TRUE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void GameSystem::EnterIdle()
{

}

void GameSystem::LeaveIdle()
{

}


