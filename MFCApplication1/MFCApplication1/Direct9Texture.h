#ifndef __Direct9Texture_H__
#define __Direct9Texture_H__
#include "Direct9Tool.h"

class Direct9Render;
class Direct9Texture
{
public:
	Direct9Texture(Direct9Render*lpOwer,string strName);

	~Direct9Texture();


public:
	int GetWidth() { return m_nWidth; }
	int GetHeight() { return m_nHeight; }

	const string& GetName() { return m_strName; }


	void Release();

	void Reload();

	void SetTexture(LPDIRECT3DTEXTURE9 lpDirectTexture);

	void SetName(string strName) { m_strName = strName; }

	void SetIsLoadNormal(bool bIsLoadNormal) {
		m_bIsLoadNormal = bIsLoadNormal;
	}

	void LoadFromFile();

	void LoadFromFileEx();

public:
	LPDIRECT3DTEXTURE9 GetTexture() {
		return m_lpDirect3DTexture;
	}
private:
	Direct9Render* m_pOwer;
	string m_strName;
	LPDIRECT3DTEXTURE9 m_lpDirect3DTexture;

	int m_nWidth, m_nHeight;
	bool m_bIsLoadNormal;
};

#endif
