#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(_uint iIndex = 0) {
		return m_Textures[iIndex];
	}

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT SetUp_OnShader(char* pConstantName, class CVIBuffer* pVIBuffer, _uint iIndex = 0);

private:	
	vector<ID3D11ShaderResourceView*>			m_Textures;
	typedef vector<ID3D11ShaderResourceView*>	TEXTURES;

	_uint			m_iNumTextures = 0;
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END