#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CSkill_Info final : public CGameObject
{
private:
	explicit CSkill_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSkill_Info(const CSkill_Info& rhs);
	virtual ~CSkill_Info() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

	_double				m_lfTimeAcc = 0.0;
	_double				m_lfTimeMax = 3.0;

public:
	static CSkill_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END