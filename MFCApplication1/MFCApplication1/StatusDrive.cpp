#include "stdafx.h"
#include "StatusDrive.h"
#include "GameStatus.h"

StatusDrive::StatusDrive()
{
	m_pCurStatus = nullptr;
}

StatusDrive::~StatusDrive()
{
	Release();
}

void StatusDrive::Inital()
{
	
}

void StatusDrive::Update(float deltaTime)
{
	while (m_queSwitchStatusID.size() > 0)
	{
		int nStatusID = m_queSwitchStatusID.front();
		m_queSwitchStatusID.pop();

		StatusBase* pOldStatus = m_pCurStatus;
		StatusMap::iterator it = m_mapStatus.find(nStatusID);
		if (it != m_mapStatus.end())
		{
			m_pCurStatus = it->second;
			if (pOldStatus)
			{
				pOldStatus->OnLeave();
			}
			if (m_pCurStatus)
			{
				m_pCurStatus->OnEnter();
			}
		}
	}
	if(m_pCurStatus)
		m_pCurStatus->OnUpdate(deltaTime);
}

void StatusDrive::Release()
{
	for (StatusMap::iterator it = m_mapStatus.begin();
		it != m_mapStatus.end(); ++it)
	{
		delete it->second;
	}
	m_mapStatus.clear();
}

void StatusDrive::AddStatus(int nStatusID)
{
	StatusMap::iterator it = m_mapStatus.find(nStatusID);
	if (it == m_mapStatus.end())
	{
		m_mapStatus[nStatusID] = CreateStatus(nStatusID);
	}
}

void StatusDrive::DestroyStatus(StatusBase* pStatus)
{
	if (pStatus == NULL)
		return;
	StatusMap::iterator it = m_mapStatus.find(pStatus->GetStatusID());
	if (it == m_mapStatus.end())
		return;

	delete it->second;
	m_mapStatus.erase(it);
}

void StatusDrive::DestroyStatus(int nStatusID)
{
	StatusMap::iterator it = m_mapStatus.find(nStatusID);
	if (it == m_mapStatus.end())
		return;
	delete it->second;
	m_mapStatus.erase(it);
}

bool StatusDrive::InputProcess(int message, int wParam, int lParam)
{
	bool bIsHandle = false;
	if (m_pCurStatus)
		bIsHandle = m_pCurStatus->InputProcess(message, wParam, lParam);

	return bIsHandle;
}

void StatusDrive::SwitchStatus(int nStatusID)
{
	m_queSwitchStatusID.push(nStatusID);
}

int StatusDrive::GetCurrentStatus()
{
	if (m_pCurStatus == NULL)
	{
		return GAME_MAX_STATUS;
	}
	else
	{
		return m_pCurStatus->GetStatusID();
	}
}
