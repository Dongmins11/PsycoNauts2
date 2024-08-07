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

class CBlender2 final:public CGameObject
{
public:
	enum BLENDERSTATE{ BLENDERSTATE_RIGHT, BLENDERSTATE_UP,BLENDERSTATE_LOOK,BLENDERSTATE_POSTION,BLENDERSTATE_END};

protected:
	explicit CBlender2(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBlender2(const CBlender2& rhs);
	virtual ~CBlender2() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();


private:
	void CheckOnClimbing(_double TimeDelta);
	void CheckOnClimbing2(_double TimeDelta);

public:
	void Set_Climbing(_bool ClimbingCheck, _matrix Matrix);
	_bool Get_Climbing() { return m_bClimbing; }
	void Set_Move(_bool bCheck) { m_bMove = bCheck; }

public:
	_vector Get_STATE_Trigger(BLENDERSTATE Index) { return XMLoadFloat4((_float4*)&m_Move4x4.m[Index][0]); }
	
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	//CGameObject*			m_pFruitCollider[2] = { nullptr };
	//CBoxCollider*			m_pTriggerCom[2] = { nullptr };
	CNavMeshCollider*		m_pColliderCom = nullptr;

	_bool					m_bClimbing = false;

#ifdef _DEBUG
	//CCollider*				m_pTriggerRenderCom[2] = { nullptr };
#endif // _DEBUG

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
	_bool					m_isCreated = false;
	_uint					m_iMyCount = 0;
	_uint					m_iCurlingIndex = 0;
	_uint					m_iDeleteCurlingIndex = 0;
	_float4x4               m_Move4x4;
	vector<_float3*>		m_vecNaviData;

	_bool					m_bMove = false;
	_float3					m_vDir = _float3(0.f, 0.f, 0.f);
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Loading_PlateNaviData(const _tchar * pNaviDataFilePath);
	HRESULT SetUp_Triggers();


public:
	static CBlender2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;

};

END