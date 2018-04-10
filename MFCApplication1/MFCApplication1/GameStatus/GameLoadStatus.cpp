#include "stdafx.h"
#include "GameLoadStatus.h"
#include "GameSystem.h"

GameLoadStatus::GameLoadStatus(StatusDrive* pStatusDrive)
	:GameStatus(pStatusDrive)
{
	memset(m_pArrLoadTexture, 0, sizeof(m_pArrLoadTexture));
	memset(m_pArrNumTexture, 0, sizeof(m_pArrNumTexture));
	m_fLoadFrame = 0.0f;
	m_fLoadfps = 1.0f;
	m_nLoadCompete = 0.0f;

	m_hRcvEvent = INVALID_HANDLE_VALUE;
	m_hThread = INVALID_HANDLE_VALUE;
	// 是否加载完成
	m_bIsLoaded = false;
}

GameLoadStatus::~GameLoadStatus()
{
	OnLeave();
}

void GameLoadStatus::OnEnter()
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	Direct9Render* m_pDirect9Render = pGameSystem->GetRender();
	if (m_pDirect9Render == NULL)
		return;

	TCHAR szTempPath[MAX_PATH] = {};
	for (int i = 0; i < LOAD_TEXTURE_MAX; ++i)
	{
		_stprintf_s(szTempPath, _T(".\\resource\\load\\资源正在加载中%d.png"), i);
		m_pArrLoadTexture[i] = m_pDirect9Render->LoadTextureFromFile(szTempPath);
	}

	for (int i = 0; i < NUM_TEXTURE_MAX - 1; ++i)
	{
		_stprintf_s(szTempPath, _T(".\\resource\\倒计时\\%d.png"), i);
		m_pArrNumTexture[i] = m_pDirect9Render->LoadTextureFromFile(szTempPath);
	}

	if(m_hRcvEvent == INVALID_HANDLE_VALUE)
		m_hRcvEvent = CreateEvent(0, false, false, _T("LoadThread"));

	if (m_hThread == INVALID_HANDLE_VALUE)
	{
		DWORD dwThreadID = 0;
		m_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LoadResourceThread, this, 0, &dwThreadID);
	}
}

void GameLoadStatus::OnUpdate(float deltaTime)
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	Direct9Render* m_pDirect9Render = pGameSystem->GetRender();
	if (m_pDirect9Render == NULL)
		return;

	//// 加载完成 切换状态
	if (m_bIsLoaded)
	{
		pGameSystem->SwitchStatus(GAME_IDLE_STATUS);
		return;
	}

	m_fLoadFrame = m_fLoadFrame + m_fLoadfps * deltaTime;
	while (m_fLoadFrame >= LOAD_TEXTURE_MAX)
	{
		m_fLoadFrame -= LOAD_TEXTURE_MAX;
	}
	LPDIRECT9TEXTURE lpDirect9Texture = m_pArrLoadTexture[int(m_fLoadFrame)];
	RectF pTextureRect(0, 0, lpDirect9Texture->GetWidth(), lpDirect9Texture->GetHeight());
	pTextureRect.OffsetSelf((1080 - pTextureRect.GetWidth())*0.5f, (1920 - pTextureRect.GetHeight())*0.5f);
	m_pDirect9Render->AddQuad(pTextureRect, lpDirect9Texture);

	std::vector<int> vecNum;
	int nTempNum = m_nLoadCompete;
	while (nTempNum > 0)
	{
		int nNum = nTempNum % 10;
		nTempNum = (nTempNum - nNum) / 10;
		vecNum.push_back(nNum);
	}
	if (vecNum.size() == 0)
		vecNum.push_back(0);

	int offset = 0;
	for (std::vector<int>::reverse_iterator it = vecNum.rbegin(); it != vecNum.rend(); ++it)
	{
		offset += m_pArrNumTexture[*it]->GetWidth();
	}
	offset = (1080 - offset) * 0.5f;
	for (std::vector<int>::reverse_iterator it = vecNum.rbegin(); it != vecNum.rend(); ++it)
	{
		LPDIRECT9TEXTURE lpDirect9Texture = m_pArrNumTexture[*it];
		RectF rect(0, 0, lpDirect9Texture->GetWidth(), lpDirect9Texture->GetHeight());
		rect.OffsetSelf(offset, 100 - lpDirect9Texture->GetHeight());
		offset += lpDirect9Texture->GetWidth();
		m_pDirect9Render->AddQuad(rect, lpDirect9Texture);
	}
}

void GameLoadStatus::OnLeave()
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	Direct9Render* m_pDirect9Render = pGameSystem->GetRender();
	if (m_pDirect9Render == NULL)
		return;

	TCHAR szTempPath[MAX_PATH] = {};
	for (int i = 0; i < LOAD_TEXTURE_MAX; ++i)
	{
		_stprintf_s(szTempPath, _T(".\\resource\\load\\资源正在加载中%d.png"), i);
		m_pDirect9Render->DestroyTextureByName(szTempPath);
		m_pArrLoadTexture[i] = NULL;
	}

	if (m_hRcvEvent != INVALID_HANDLE_VALUE)
	{
		SetEvent(m_hRcvEvent);
		if (m_hThread != INVALID_HANDLE_VALUE)
		{
			DWORD dwExitCode = STILL_ACTIVE;
			do
			{
				GetExitCodeThread(m_hThread, &dwExitCode);
			} while (dwExitCode == STILL_ACTIVE);
			m_hThread = INVALID_HANDLE_VALUE;
			OutputDebugString(_T("GetExitCodeThread Is Exit\n"));
		}
		CloseHandle(m_hRcvEvent);
		m_hRcvEvent = INVALID_HANDLE_VALUE;
		OutputDebugString(_T("Event Is Close\n"));
	}
}

DWORD GameLoadStatus::LoadResourceThread(LPVOID lParam)
{
	GameLoadStatus* pGameLoadStatus = (GameLoadStatus*)lParam;
	if (pGameLoadStatus == NULL)
		return 0;

	GameSystem* pGameSystem = (GameSystem*)pGameLoadStatus->m_pStatusDrive;

	float fps = 60;
	TCHAR szTempPath[MAX_PATH] = {};
	for (int i = 0; i < TEXTURE_MAX; ++i)
	{
		DWORD result = WaitForSingleObject(pGameLoadStatus->m_hRcvEvent, int(1000 / fps));
		if (WAIT_OBJECT_0 == result)
		{
			OutputDebugString(_T("WaitForSingleObject WAIT_OBJECT_0\n"));
		}
		if (WAIT_TIMEOUT != result)
			break;

		int index = i * (80 / TEXTURE_MAX);
		memset(szTempPath, 0, sizeof(szTempPath));
		if (index < 10)
			_stprintf_s(szTempPath, _T("%s\\骑自行车000%d.%s"), RES_DIR, index, IMAGE);
		else
			_stprintf_s(szTempPath, _T("%s\\\\骑自行车00%d.%s"), RES_DIR, index, IMAGE);

		OutputDebugString(szTempPath);
		OutputDebugString(_T("正在加载......\n"));
		pGameSystem->GetRender()->LoadTextureFromFileEx(szTempPath);
		OutputDebugString(_T("加载完成\n"));
		pGameLoadStatus->m_nLoadCompete = i;
	}

	pGameLoadStatus->m_bIsLoaded = true;
	return 0;
}

