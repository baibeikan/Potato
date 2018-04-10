#ifndef __GameLoadStatus_H__
#define __GameLoadStatus_H__
#include "GameStatus.h"

class StatusDrive;
class GameLoadStatus : public GameStatus
{
public:
	GameLoadStatus(StatusDrive* pStatusDrive);
	
	virtual ~GameLoadStatus();

public:
	// 状态进入
	virtual void OnEnter();
	// 状态更新
	virtual void OnUpdate(float deltaTime);
	// 状态离开
	virtual void OnLeave();

	virtual int GetStatusID() { return GAME_LOAD_STATUS; }

private:
	static DWORD LoadResourceThread(LPVOID lParam);

protected:
	LPDIRECT9TEXTURE		m_pArrLoadTexture[LOAD_TEXTURE_MAX];
	LPDIRECT9TEXTURE		m_pArrNumTexture[NUM_TEXTURE_MAX];

	float					m_fLoadFrame;
	float					m_fLoadfps;
	int						m_nLoadCompete;
	HANDLE m_hThread;
	HANDLE m_hRcvEvent;//事件

	// 是否加载完成
	bool					m_bIsLoaded;
};

#endif
