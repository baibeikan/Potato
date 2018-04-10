#ifndef __Direct9Tool_H__
#define __Direct9Tool_H__
#include "Directx9.0/Include/d3dx9.h"
#include <windows.h>
#include <vector>
#include <string.h>
#include <queue>
#include <map>

#ifdef UNICODE
typedef std::wstring string;
#else
typedef std::string string;
#endif

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = NULL; }}
#define SAFE_DELETE(ptr)  { if(ptr) { delete ptr; ptr = NULL; }}

class CLock
{
public:
	CLock(void)
	{
		InitializeCriticalSection(&cs);
	}
	~CLock(void)
	{
		DeleteCriticalSection(&cs);
	}
	inline void Lock()
	{
		EnterCriticalSection(&cs);
	}
	inline void UnLock()
	{
		LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;
};

class CAutoLock
{
public:
	CAutoLock(CLock* lock) : m_lock(lock)
	{
		m_lock->Lock();
	}

	~CAutoLock()
	{
		m_lock->UnLock();
	}
private:
	CLock* m_lock;
};

struct RectF
{
	float left, top;
	float right, bottom;

	RectF(float _left, float _top, float _right, float _bottom)
	{
		left = _left;
		top = _top;
		right = _right;
		bottom = _bottom;
	}

	RectF()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	RectF(const RectF& ref)
	{
		left = ref.left;
		top = ref.top;
		right = ref.right;
		bottom = ref.bottom;
	}

	RectF& operator=(const RectF& ref)
	{
		left = ref.left;
		top = ref.top;
		right = ref.right;
		bottom = ref.bottom;
		return *this;
	}

	void OffsetSelf(float x, float y) {
		OffsetXSelf(x);
		OffsetYSelf(y);
	}

	void OffsetXSelf(float x) {
		left = left + x;
		right = right + x;
	}

	void OffsetYSelf(float offset)
	{
		top = top + offset;
		bottom = bottom + offset;
	}

	float GetWidth()
	{
		return right - left;
	}

	float GetHeight() { return bottom - top; }
};

class Direct9Texture;
typedef Direct9Texture* LPDIRECT9TEXTURE;

struct QuadData
{
	LPDIRECT9TEXTURE lpTexture;
	RectF rect;
};

struct QuadRenderData
{
	LPDIRECT3DTEXTURE9 lpTexture;
	int nBeginIndex;
	int nNum;
};

#endif
