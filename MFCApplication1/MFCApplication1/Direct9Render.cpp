#include "stdafx.h"
#include "Direct9Render.h"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = NULL; }}
#define SAFE_DELETE(ptr)  { if(ptr) { delete ptr; ptr = NULL; }}

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position; // The position
	D3DXVECTOR2 tuv;
};
// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


Direct9Render::Direct9Render(HWND hWnd)
	:m_hWnd(hWnd)
{
	// 指定默认高宽
	m_nWidth = 1366;
	m_nHeight = 768;

	m_pD3D = NULL; // Used to create the D3DDevice
	m_pd3dDevice = NULL; // Our rendering device

	m_pVB = NULL;
	m_nVBCount = 0;
}

Direct9Render::~Direct9Render()
{
	Release();
}

void Direct9Render::Inital()
{
	InitialD3d(m_hWnd);

	restoreDeviceObjects();

	//InitGeometry();


}

void Direct9Render::Update(float deltaTime)
{
	m_vecQuadData.clear();
}

void Direct9Render::Render()
{
	HRESULT hr = S_FALSE;
	if(FAILED(hr = m_pd3dDevice->TestCooperativeLevel()))
	{
		// The device has been lost but cannot be reset at this time. 
		// Therefore, rendering is not possible and we''ll have to return 
		// and try again at a later time.
		if (hr == D3DERR_DEVICELOST)
			return;

		// The device has been lost but it can be reset at this time. 
		if (hr == D3DERR_DEVICENOTRESET)
		{
			//
			// If the device can be restored, the application prepares the 
			// device by destroying all video-memory resources and any 
			// swap chains. 
			//
			invalidateDeviceObjects();
			//
			// Then, the application calls the Reset method.
			//
			// Reset is the only method that has an effect when a device 
			// is lost, and is the only method by which an application can 
			// change the device from a lost to an operational state. 
			// Reset will fail unless the application releases all 
			// resources that are allocated in D3DPOOL_DEFAULT, including 
			// those created by the IDirect3DDevice9::CreateRenderTarget 
			// and IDirect3DDevice9::CreateDepthStencilSurface methods.
			//

			hr = m_pd3dDevice->Reset(&m_d3dpp);
			if (FAILED(hr))
				return;
			//
			// Finally, a lost device must re-create resources (including  
			// video memory resources) after it has been reset.
			//
			restoreDeviceObjects();
		}

		return;
	}

	prepareRender();

	CAutoLock autoLock(&m_ThreadLock);

	// Clear the backbuffer and the zbuffer
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 255, 255), 1.0f, 0);
	// Begin the scene
	if (SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		// Set up world matrix
		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		//D3DXMatrixRotationX( &matWorld, deltaTime );
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
		m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		LPDIRECT3DTEXTURE9 lpTexture = NULL;
		for (int i = 0; i < m_vecQuadRenderData.size(); ++i)
		{
			QuadRenderData& quadRenderData = m_vecQuadRenderData.at(i);
			if (lpTexture != quadRenderData.lpTexture)
			{
				lpTexture = quadRenderData.lpTexture;
				m_pd3dDevice->SetTexture(0, lpTexture);
			}
			m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, quadRenderData.nBeginIndex, 2);
		}
		// End the scene
		m_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Direct9Render::Release()
{
	invalidateDeviceObjects();
	for (std::map<string, Direct9Texture*>::iterator it = m_mapTexture.begin();
		it != m_mapTexture.end(); ++it)
	{
		delete it->second;
	}

	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pD3D);
}

Direct9Texture* Direct9Render::GetTextureByName(string strName)
{
	std::map<string, Direct9Texture*>::iterator it = m_mapTexture.find(strName);
	if (it == m_mapTexture.end())
		return NULL;
	
	return it->second;
}

void Direct9Render::DestroyTextureByName(string strName)
{
	std::map<string, Direct9Texture*>::iterator it = m_mapTexture.find(strName);
	if (it != m_mapTexture.end())
	{
		SAFE_DELETE(it->second);
		m_mapTexture.erase(it);
	}
}

HRESULT Direct9Render::InitialD3d(HWND hWnd)
{
	// Create the D3D object.
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	m_nWidth = windowRect.right - windowRect.left;
	m_nHeight = windowRect.bottom - windowRect.top;

	// Set up the structure used to create the D3DDevice. Since we are now
	// using more complex geometry, we will create a device with a zbuffer.
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_d3dpp.BackBufferWidth = m_nWidth;
	m_d3dpp.BackBufferHeight = m_nHeight;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.Flags = 0;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.hDeviceWindow = hWnd;
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.MultiSampleQuality = 0;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	DWORD vp = 0;
	D3DCAPS9 caps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	if (caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;


	// Create the D3DDevice
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		vp, &m_d3dpp, &m_pd3dDevice)))
	{
		SAFE_RELEASE(m_pD3D);
		return E_FAIL;
	}

	// Turn off culling
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Turn off D3D lighting
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Turn on the zbuffer
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	return S_OK;
}

void Direct9Render::createVertexBuffer(int nSize)
{
	if (nSize <= m_nVBCount)
		return;
	m_nVBCount = nSize + 6;
	// 创建顶点缓冲区
	SAFE_RELEASE(m_pVB);
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_nVBCount * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &m_pVB, NULL)))
	{
		return;
	}
}

void Direct9Render::AddQuad(const RectF& rect, LPDIRECT9TEXTURE lpTexture)
{
	QuadData quadData;
	quadData.rect = rect;
	quadData.lpTexture = lpTexture;
	m_vecQuadData.push_back(quadData);
}

void Direct9Render::prepareRender()
{
	m_vecQuadRenderData.clear();
	createVertexBuffer(m_vecQuadData.size() * 6);

	if (m_pVB == NULL)
		return;

	// 填充顶点缓冲区
	CUSTOMVERTEX* pTextVertices;
	if (FAILED(m_pVB->Lock(0, m_vecQuadData.size()*6* sizeof(CUSTOMVERTEX), (void**)&pTextVertices, 0)))
		return;

	int nBeginIndex = 0;
	for (int i = 0; i < m_vecQuadData.size(); ++i)
	{
		QuadData& quadData = m_vecQuadData.at(i);
		quadData.lpTexture->Reload();
		RectF loadRect = GetD3dRect(quadData.rect, 1080, 1920, m_nWidth, m_nHeight);
		RectF uv(0, 0, 1, 1);
		QuadRenderData quadRenderData;
		quadRenderData.lpTexture = quadData.lpTexture->GetTexture();
		quadRenderData.nBeginIndex = nBeginIndex;

		pTextVertices[nBeginIndex].position = D3DXVECTOR3(loadRect.left, loadRect.bottom, 0.0f);
		pTextVertices[nBeginIndex].tuv = D3DXVECTOR2(uv.left, uv.bottom);
		nBeginIndex++;

		pTextVertices[nBeginIndex].position = D3DXVECTOR3(loadRect.left, loadRect.top, 0.0f);
		pTextVertices[nBeginIndex].tuv = D3DXVECTOR2(uv.left, uv.top);
		nBeginIndex++;

		pTextVertices[nBeginIndex].position = D3DXVECTOR3(loadRect.right, loadRect.top, 0.0f);
		pTextVertices[nBeginIndex].tuv = D3DXVECTOR2(uv.right, uv.top);
		nBeginIndex++;

		pTextVertices[nBeginIndex].position = D3DXVECTOR3(loadRect.left, loadRect.bottom, 0.0f);
		pTextVertices[nBeginIndex].tuv = D3DXVECTOR2(uv.left, uv.bottom);
		nBeginIndex++;

		pTextVertices[nBeginIndex].position = D3DXVECTOR3(loadRect.right, loadRect.top, 0.0f);
		pTextVertices[nBeginIndex].tuv = D3DXVECTOR2(uv.right, uv.top);
		nBeginIndex++;

		pTextVertices[nBeginIndex].position = D3DXVECTOR3(loadRect.right, loadRect.bottom, 0.0f);
		pTextVertices[nBeginIndex].tuv = D3DXVECTOR2(uv.right, uv.bottom);
		nBeginIndex++;

		quadRenderData.nNum = 2;
		m_vecQuadRenderData.push_back(quadRenderData);
	}
	m_pVB->Unlock();
	m_vecQuadData.clear();
}

void Direct9Render::invalidateDeviceObjects()
{
	SAFE_RELEASE(m_pVB);
	for (std::map<string, Direct9Texture*>::iterator it = m_mapTexture.begin();
		it != m_mapTexture.end(); ++it)
	{
		it->second->Release();
	}
}

void Direct9Render::restoreDeviceObjects()
{
	//m_nVBCount = 256;
	//// 创建顶点缓冲区
	//if (FAILED(g_pd3dDevice->CreateVertexBuffer(m_nVBCount * sizeof(CUSTOMVERTEX),
	//	0, D3DFVF_CUSTOMVERTEX,
	//	D3DPOOL_MANAGED, &m_pVB, NULL)))
	//{
	//	return;
	//}
}

RectF Direct9Render::GetD3dRect(RectF& screenRect, int nScreenWidth, int nScreenHeight, int nBackWidth, int nBackHeight)
{
	float rate = min((float)nBackWidth / nScreenWidth, (float)nBackHeight / nScreenHeight);
	// 转化成Buffer坐标
	RectF tempRect;
	tempRect.left = screenRect.left * rate;
	tempRect.right = screenRect.right * rate;
	tempRect.top = (nScreenHeight - screenRect.top) * rate;
	tempRect.bottom = (nScreenHeight - screenRect.bottom) * rate;

	float fScreenWidth = nScreenWidth * rate;
	float fScreenHeight = nScreenHeight * rate;
	tempRect.left = tempRect.left + (nBackWidth - fScreenWidth) * 0.5f;
	tempRect.right = tempRect.right + (nBackWidth - fScreenWidth) * 0.5f;
	tempRect.top = tempRect.top + (nBackHeight - fScreenHeight) * 0.5f;
	tempRect.bottom = tempRect.bottom + (nBackHeight - fScreenHeight) * 0.5f;

	tempRect.left = tempRect.left / (float)nBackWidth;
	tempRect.right = tempRect.right / (float)nBackWidth;
	tempRect.top = tempRect.top / (float)nBackHeight;
	tempRect.bottom = tempRect.bottom / (float)nBackHeight;

	tempRect.left = tempRect.left * 2 - 1;
	tempRect.right = tempRect.right * 2 - 1;
	tempRect.top = tempRect.top * 2 - 1;
	tempRect.bottom = tempRect.bottom * 2 - 1;
	return tempRect;
}

LPDIRECT9TEXTURE Direct9Render::LoadTextureFromFile(string strName)
{
	Direct9Texture* lpTexture = GetTextureByName(strName);
	if (lpTexture == NULL)
	{
		LPDIRECT3DTEXTURE9 lpDirectTexture = NULL;
		//{
		//	CAutoLock autoLock(&m_ThreadLock);
		//	HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, strName.c_str(), &lpDirectTexture);
		//	if (hr != D3D_OK)
		//	{
		//		switch (hr)
		//		{
		//		case  D3DERR_INVALIDCALL:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_INVALIDCALL", MB_OK);
		//			break;
		//		case D3DERR_NOTAVAILABLE:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_NOTAVAILABLE", MB_OK);
		//			break;
		//		case D3DERR_OUTOFVIDEOMEMORY:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_OUTOFVIDEOMEMORY", MB_OK);
		//			break;
		//		case D3DXERR_INVALIDDATA:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DXERR_INVALIDDATA", MB_OK);
		//			break;
		//		case E_OUTOFMEMORY:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe E_OUTOFMEMORY", MB_OK);
		//			break;
		//		default:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe", MB_OK);
		//			break;
		//		}
		//		return NULL;
		//	}
		//}

		lpTexture = new Direct9Texture(this, strName);
		lpTexture->LoadFromFile();
		{
			CAutoLock autoLock(&m_ThreadLock);
			lpTexture->Reload();
		}
		m_mapTexture.insert(std::map<string, LPDIRECT9TEXTURE>::value_type(strName, lpTexture));
	}
	return lpTexture;
}

LPDIRECT9TEXTURE Direct9Render::LoadTextureFromFileEx(string strName)
{
	LPDIRECT9TEXTURE lpTexture = GetTextureByName(strName);
	if (lpTexture == NULL)
	{

		LPDIRECT3DTEXTURE9 lpDirectTexture = NULL;
		//{
		//	CAutoLock autoLock(&m_ThreadLock);
		//	D3DXIMAGE_INFO imageInfo;
		//	HRESULT hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, strName.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
		//		D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE, D3DCOLOR_RGBA(0, 0, 0, 255),
		//		&imageInfo, NULL, &(lpDirectTexture));
		//	if (hr != D3D_OK)
		//	{
		//		switch (hr)
		//		{
		//		case  D3DERR_INVALIDCALL:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_INVALIDCALL", MB_OK);
		//			break;
		//		case D3DERR_NOTAVAILABLE:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_NOTAVAILABLE", MB_OK);
		//			break;
		//		case D3DERR_OUTOFVIDEOMEMORY:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_OUTOFVIDEOMEMORY", MB_OK);
		//			break;
		//		case D3DXERR_INVALIDDATA:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DXERR_INVALIDDATA", MB_OK);
		//			break;
		//		case E_OUTOFMEMORY:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe E_OUTOFMEMORY", MB_OK);
		//			break;
		//		default:
		//			MessageBox(NULL, strName.c_str(), L"DxTextures.exe", MB_OK);
		//			break;
		//		}
		//		return NULL;
		//	}
		//}

		lpTexture = new Direct9Texture(this, strName);
		lpTexture->LoadFromFileEx();
		{
			CAutoLock autoLock(&m_ThreadLock);
			lpTexture->Reload();
		}
		//lpTexture->SetTexture(lpDirectTexture);
		m_mapTexture.insert(std::map<string, LPDIRECT9TEXTURE>::value_type(strName, lpTexture));
	}
	return lpTexture;
}

void Direct9Render::LoadTextureFromFileEx(Direct9Texture* pDirect9Texture)
{
	if (pDirect9Texture == NULL)
		return;

	LPDIRECT3DTEXTURE9 lpDirectTexture = NULL;
	const string& strName = pDirect9Texture->GetName();
	D3DXIMAGE_INFO imageInfo;
	{
		CAutoLock autoLock(&m_ThreadLock);

		HRESULT hr = D3DXCreateTextureFromFileEx(m_pd3dDevice, strName.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0,
			D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE, D3DCOLOR_RGBA(0, 0, 0, 255),
			&imageInfo, NULL, &(lpDirectTexture));
		if (hr != D3D_OK)
		{
			switch (hr)
			{
			case  D3DERR_INVALIDCALL:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_INVALIDCALL", MB_OK);
				break;
			case D3DERR_NOTAVAILABLE:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_NOTAVAILABLE", MB_OK);
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_OUTOFVIDEOMEMORY", MB_OK);
				break;
			case D3DXERR_INVALIDDATA:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DXERR_INVALIDDATA", MB_OK);
				break;
			case E_OUTOFMEMORY:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe E_OUTOFMEMORY", MB_OK);
				break;
			default:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe", MB_OK);
				break;
			}
			return;
		}
	}
	pDirect9Texture->SetTexture(lpDirectTexture);
}

void Direct9Render::LoadTextureFromFile(Direct9Texture * pDirect9Texture)
{
	if (pDirect9Texture == NULL)
		return;

	const string& strName = pDirect9Texture->GetName();
	LPDIRECT3DTEXTURE9 lpDirectTexture = NULL;
	{
		CAutoLock autoLock(&m_ThreadLock);
		HRESULT hr = D3DXCreateTextureFromFile(m_pd3dDevice, strName.c_str(), &lpDirectTexture);
		if (hr != D3D_OK)
		{
			switch (hr)
			{
			case  D3DERR_INVALIDCALL:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_INVALIDCALL", MB_OK);
				break;
			case D3DERR_NOTAVAILABLE:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_NOTAVAILABLE", MB_OK);
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DERR_OUTOFVIDEOMEMORY", MB_OK);
				break;
			case D3DXERR_INVALIDDATA:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe D3DXERR_INVALIDDATA", MB_OK);
				break;
			case E_OUTOFMEMORY:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe E_OUTOFMEMORY", MB_OK);
				break;
			default:
				MessageBox(NULL, strName.c_str(), L"DxTextures.exe", MB_OK);
				break;
			}
			return;
		}
	}
	pDirect9Texture->SetTexture(lpDirectTexture);
}
