#ifndef __GameRunStatus_H__
#define __GameRunStatus_H__
#include "GameStatus.h"

class GameRunStatus : public GameStatus
{
public:
	GameRunStatus(StatusDrive* pStatusDrive);

	
	virtual ~GameRunStatus(){}

public:
	// 状态进入
	virtual void OnEnter();
	// 状态更新
	virtual void OnUpdate(float deltaTime);
	// 状态离开
	virtual void OnLeave();

	virtual int GetStatusID() { return GAME_RUN_STATUS; }

	virtual bool InputProcess(int message, int wParam, int lParam);

protected:
	LPDIRECT9TEXTURE		m_pArrTexture[TEXTURE_MAX];
	LPDIRECT9TEXTURE		m_pArrNumTexture[NUM_TEXTURE_MAX];
	// 当前角度
	float m_fCurrentAngle;
	// 角速度
	float m_fAngleSpeed;
	// 绘制当前帧
	int m_nCurFrame;

};

#endif
