#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_RectInstance;
class CEffect_Option;
END

BEGIN(Client)
class CEffect_RectRepeat final :public CGameObject
{
public:
	enum UPDATE_TYPE {
		TYPE_OPTION, TYPE_BILLBOARD, TYPE_SMOKE_RANDING, TYPE_SMOKE_DIE, TYPE_SPARK,
		TYPE_SMOKE_WORK, TYPE_SMOKE_OBJECT, TYPE_PAPER, TYPE_FIRE, TYPE_PARTICLE, TYPE_BULLETFIRE,
		TYPE_PICKINGOBJECT, TYPE_BROKENOBJECT, TYPE_WATER, TYPE_VENTSMOKE, TYPE_BUNNERFIRE,
		TYPE_SPARKLING, TYPE_SPARKLING_FRUIT, TYPE_SPARKLING_GRIDDLE, TYPE_BOSS_EFFECT, TYPE_BOSS_RANDING, 
		TYPE_PAPEROPTION, TYPE_END
	};
private:
	explicit CEffect_RectRepeat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_RectRepeat(const CEffect_RectRepeat& rhs);
	virtual ~CEffect_RectRepeat() = default;

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
	class CVIBuffer_RectInstance*	m_pVIBufferCom = nullptr;
	class CEffect_Option*			m_pOptionCom = nullptr;

	EFFECTPARTICLEINFO				m_tParticleInfo;

	_double							m_TimeAcc = 0.f;
	_double							m_LiveTimeAcc = 0.f;

	/* »ó¼Ó */
	_bool	  m_isOnce = false;
	_float4x4 m_ParentMat;

private:
	HRESULT SetUp_Component(wstring	wstrVIBufferTag);
	HRESULT SetUp_ConstantTable();

public:
	static CEffect_RectRepeat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free() override;
};
END
