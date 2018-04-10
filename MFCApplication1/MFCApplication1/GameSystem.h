#ifndef __GameSystem_H__
#define __GameSystem_H__

#include "Direct9Tool.h"
#include "Direct9Render.h"
#include "StatusDrive.h"
#include "GameStatus.h"

enum GAME_STATUS
{
	GAME_LOAD,
	GAME_IDLE,
	GAME_RUN,
	GAME_STATUS_MAX,
};

#include <vector>

class GameSystem : public StatusDrive
{
public:
	 GameSystem();
	~ GameSystem();

public:
	void Inital(HWND hWnd);
	void Update(float deltaTime);
	void Render();
	void Release();

	virtual StatusBase* CreateStatus(int nStatusID);

	BOOL PreTranslateMessage(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual void EnterIdle();

	virtual void LeaveIdle();

	virtual HWND GetWindowHandle() = 0;
public:
	Direct9Render* GetRender() { return m_pDirect9Render;  }
private:
	Direct9Render* m_pDirect9Render;
};



#endif
