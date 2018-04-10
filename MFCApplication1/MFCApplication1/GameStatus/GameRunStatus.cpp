#include "stdafx.h"
#include "GameRunStatus.h"
#include "GameSystem.h"

GameRunStatus::GameRunStatus(StatusDrive * pStatusDrive)
	:GameStatus(pStatusDrive)
{
	memset(m_pArrTexture, 0, sizeof(m_pArrTexture));
	memset(m_pArrNumTexture, 0, sizeof(m_pArrNumTexture));
	// 当前角度
	m_fCurrentAngle = 0.0f;
	// 角速度
	m_fAngleSpeed = 360;
	// 绘制当前帧
	m_nCurFrame = 0;
}

void GameRunStatus::OnEnter()
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	Direct9Render* m_pDirect9Render = pGameSystem->GetRender();
	if (m_pDirect9Render == NULL)
		return;

	TCHAR szTempPath[MAX_PATH] = {};
	for (int i = 0; i < TEXTURE_MAX; ++i)
	{
		int index = i * (80 / TEXTURE_MAX);
		memset(szTempPath, 0, sizeof(szTempPath));
		if (index < 10)
			_stprintf_s(szTempPath, _T("%s\\骑自行车000%d.%s"), RES_DIR, index, IMAGE);
		else
			_stprintf_s(szTempPath, _T("%s\\\\骑自行车00%d.%s"), RES_DIR, index, IMAGE);
		m_pArrTexture[i] = m_pDirect9Render->LoadTextureFromFile(szTempPath);
	}

	for (int i = 0; i < NUM_TEXTURE_MAX - 1; ++i)
	{
		memset(szTempPath, 0, sizeof(szTempPath));
		_stprintf_s(szTempPath, _T(".\\resource\\倒计时\\%d.png"), i);
		m_pArrNumTexture[i] = m_pDirect9Render->LoadTextureFromFile(szTempPath);
	}

	memset(szTempPath, 0, sizeof(szTempPath));
	_stprintf_s(szTempPath, _T(".\\resource\\倒计时\\-.png"));
	m_pArrNumTexture[NUM_TEXTURE_MAX - 1] = m_pDirect9Render->LoadTextureFromFile(szTempPath);
}

void GameRunStatus::OnUpdate(float deltaTime)
{
	GameSystem* pGameSystem = (GameSystem*)m_pStatusDrive;
	if (pGameSystem == NULL)
		return;

	Direct9Render* m_pDirect9Render = pGameSystem->GetRender();
	if (m_pDirect9Render == NULL)
		return;

	m_fCurrentAngle += m_fAngleSpeed*deltaTime;
	while (m_fCurrentAngle > 360.0f)
	{
		m_fCurrentAngle -= 360.0f;
	}

	while (m_fCurrentAngle < 0.0f)
	{
		m_fCurrentAngle += 360.0f;
	}

	m_nCurFrame = (int((m_fCurrentAngle / 360.0f) * TEXTURE_MAX + 0.5f)) % TEXTURE_MAX;

	RectF pTextureRect(0, 0, 1080, 1920);
	m_pDirect9Render->AddQuad(pTextureRect, m_pArrTexture[int(m_nCurFrame)]);

	int nWidth = 56 * 2, nHeight = 58 * 2;
	std::vector<int> vecNum;
	int nTempNum = abs((int)(m_fAngleSpeed + 0.5f));
	while (nTempNum > 0)
	{
		int nNum = nTempNum % 10;
		nTempNum = (nTempNum - nNum) / 10;
		vecNum.push_back(nNum);
	}

	if ((int)(m_fAngleSpeed + 0.5f) < 0)
		vecNum.push_back(10);
	if (vecNum.size() == 0)
		vecNum.push_back(0);
	int offset = 0;
	for (std::vector<int>::reverse_iterator it = vecNum.rbegin(); it != vecNum.rend(); ++it)
	{
		offset += m_pArrNumTexture[*it]->GetWidth();
	}
	offset = (1080 - offset) * 0.5f - 50;
	for (std::vector<int>::reverse_iterator it = vecNum.rbegin(); it != vecNum.rend(); ++it)
	{
		LPDIRECT9TEXTURE lpDirect9Texture = m_pArrNumTexture[*it];
		RectF rect(0, 0, lpDirect9Texture->GetWidth(), lpDirect9Texture->GetHeight());
		rect.OffsetSelf(offset, 150 - lpDirect9Texture->GetHeight());
		offset += lpDirect9Texture->GetWidth();
		m_pDirect9Render->AddQuad(rect, lpDirect9Texture);
	}
}

void GameRunStatus::OnLeave()
{
	memset(m_pArrNumTexture, 0, sizeof(m_pArrNumTexture));
	memset(m_pArrNumTexture, 0, sizeof(m_pArrNumTexture));
}

bool GameRunStatus::InputProcess(int message, int wParam, int lParam)
{
	if (message == WM_KEYDOWN)
	{
		if (wParam == VK_UP || wParam == VK_DOWN)
		{
			if (wParam == VK_UP)
				m_fAngleSpeed += 5.0f;
			else
				m_fAngleSpeed -= 5.0f;

			return true;
		}
	}

	return false;
}

