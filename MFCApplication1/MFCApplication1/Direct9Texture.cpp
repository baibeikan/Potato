#include "stdafx.h"
#include "Direct9Texture.h"
#include "Direct9Render.h"

Direct9Texture::Direct9Texture(Direct9Render*lpOwer, string strName)
	:m_pOwer(lpOwer)
	,m_strName(strName)
{
}

Direct9Texture::~Direct9Texture()
{
	Release();
}

void Direct9Texture::Release()
{
	SAFE_RELEASE(m_lpDirect3DTexture);
}

void Direct9Texture::Reload()
{
	if (m_lpDirect3DTexture == NULL)
	{
		if (m_bIsLoadNormal)
			m_pOwer->LoadTextureFromFile(this);
		else
			m_pOwer->LoadTextureFromFileEx(this);
	}
}

void Direct9Texture::SetTexture(LPDIRECT3DTEXTURE9 lpDirectTexture)
{
	m_lpDirect3DTexture = lpDirectTexture;

	if (m_lpDirect3DTexture != NULL)
	{
		D3DSURFACE_DESC d3dSurface_desc;
		if (SUCCEEDED(m_lpDirect3DTexture->GetLevelDesc(0, &d3dSurface_desc)))
		{
			m_nWidth = d3dSurface_desc.Width;
			m_nHeight = d3dSurface_desc.Height;
		}
	}
}

void Direct9Texture::LoadFromFile()
{
	m_bIsLoadNormal = true;
}

void Direct9Texture::LoadFromFileEx()
{
	m_bIsLoadNormal = false;
}
