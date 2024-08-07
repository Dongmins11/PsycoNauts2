#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTexture;
END

class CPurseRing final :public CGameObject
{
private:
	explicit CPurseRing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPurseRing(const CPurseRing& rhs);
	virtual ~CPurseRing() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pAlphaTexture = nullptr;

	_double					m_TimeAcc = 0.0;

	EFFECTMESHINFO*			m_pMeshInfo = nullptr;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CPurseRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
