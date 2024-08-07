#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
//class CRenderer;
class CTransform;
//class CModel;
class CNavigation;
class CNavMeshCollider;
END

BEGIN(Client)

class CPlane final:public CGameObject
{
private:
	CPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlane(const CPlane& rhs);
	virtual ~CPlane() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
//	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
//	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CNavMeshCollider*		m_pNaviCollider = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
//	_int					m_iNavigationIndex = 0;

public:
	static CPlane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END