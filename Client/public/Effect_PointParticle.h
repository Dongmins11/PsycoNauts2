#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCell;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_PointInstance;
class CEffect_Option;
END

BEGIN(Client)
class CEffect_PointParticle final :public CGameObject
{
public:
	enum UPDATE_TYPE { TYPE_DEFUALT, TYPE_ONEPOINT, TYPE_SPIRAL, TYPE_RANDOMPOS, TYPE_END };
private:
	explicit CEffect_PointParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_PointParticle(const CEffect_PointParticle& rhs);
	virtual ~CEffect_PointParticle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	class CTexture*					m_pTextureCom = nullptr;
	class CRenderer*				m_pRendererCom = nullptr;
	class CTransform*				m_pTransformCom = nullptr;
	class CVIBuffer_PointInstance*	m_pVIBufferCom = nullptr;
	class CEffect_Option*			m_pOptionCom = nullptr;

	EFFECTPARTICLEINFO				m_tParticleInfo;
	_double							m_TimeAcc = 0.0;
	_double							m_LiveTimeAcc = 0.0;

	/* »ó¼Ó */
	_bool	  m_isOnce = false;
	_float4x4 m_ParentMat;

private:
	HRESULT SetUp_Component(wstring	wstrVIBufferTag);
	HRESULT SetUp_ConstantTable();

public:
	static CEffect_PointParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free() override;
};
END
