#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CCollider;
END

BEGIN(Client)

class CPlane_2D final :public CGameObject
{
protected:
	explicit CPlane_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlane_2D(const CPlane_2D& rhs);
	virtual ~CPlane_2D() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
	HRESULT Loading_NaviData(const _tchar * pNaviDataFilePath);

public:
	void Set_TriggerRender(_bool bCheck) { m_IsTriggerRender = bCheck; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavMeshCollider*		m_pNaviCollider = nullptr;
	vector<_float3*>		m_vecNaviData;

private:
	_bool					m_IsTriggerRender = false;
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
	_float					m_fValue = 0.1f;
	_float					m_fSize = 0.f;
	_float4x4				m_WorldMatrix;

private:
	HRESULT SetUp_Component(/*_float3 vColSize, _float3 vColPos, _float3 vObjScale*/);
	HRESULT SetUp_ConstantTable();

public:
	static CPlane_2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END