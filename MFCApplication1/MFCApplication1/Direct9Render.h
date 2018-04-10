#ifndef __Direct9Render_H__
#define __Direct9Render_H__
#include "Direct9Tool.h"
#include "Direct9Texture.h"



class Direct9Render
{
public:
	Direct9Render(HWND hWnd);
	~Direct9Render();

public:
	void Inital();
	void Update(float deltaTime);
	void Render();
	void Release();

public:
	Direct9Texture* GetTextureByName(string strName);

	void DestroyTextureByName(string strName);
private:

	HRESULT InitialD3d(HWND hWnd);

	void createVertexBuffer(int nSize);

	void prepareRender();

	void invalidateDeviceObjects();

	void restoreDeviceObjects();

	RectF GetD3dRect(RectF& screenRect, int nScreenWidth, int nScreenHeight, int nBackWidth, int nBackHeight);

public:
	void AddQuad(const RectF& rect, LPDIRECT9TEXTURE lpTexture);
	LPDIRECT9TEXTURE LoadTextureFromFile(string strName);
	LPDIRECT9TEXTURE LoadTextureFromFileEx(string strName);

	void LoadTextureFromFile(Direct9Texture* pDirect9Texture);
	void LoadTextureFromFileEx(Direct9Texture* pDirect9Texture);



private:	
			
private:
	LPDIRECT3D9             m_pD3D = NULL; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9       m_pd3dDevice = NULL; // Our rendering device
	D3DPRESENT_PARAMETERS	m_d3dpp;

	LPDIRECT3DVERTEXBUFFER9 m_pVB = NULL;
	int						m_nVBCount;

	std::map<string, LPDIRECT9TEXTURE> m_mapTexture;
	// ´°¿Ú¾ä±ú
	HWND m_hWnd;
	CLock m_ThreadLock;
	int m_nWidth, m_nHeight;
	int m_nWndHeight, m_nWndWidth;
	std::vector<QuadData> m_vecQuadData;
	std::vector<QuadRenderData> m_vecQuadRenderData;
};



#endif
