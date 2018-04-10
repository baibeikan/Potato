#ifndef __StatusDrive_H__
#define __StatusDrive_H__
#include <map>
#include <queue>
#include "StatusBase.h"
typedef std::map<int, StatusBase*> StatusMap;
typedef std::queue<int> StatusIDQueue;


class StatusDrive
{
public:
	StatusDrive();
	~StatusDrive();
protected:
	void Inital();

	void Update(float deltaTime);

	void Release();

protected:
	void AddStatus(int nStatusID);

	virtual StatusBase* CreateStatus(int nStatusID) = 0;

	virtual void DestroyStatus(StatusBase* pStatus);

public:
	void SwitchStatus(int nStatusID);

	int GetCurrentStatus();

	void DestroyStatus(int nStatusID);

	bool InputProcess(int message, int wParam, int lParam);

protected:
	StatusMap m_mapStatus;
	StatusIDQueue m_queSwitchStatusID;
	StatusBase* m_pCurStatus;
};


#endif
