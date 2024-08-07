#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
//class CNavigation;
class CBoxCollider;
END

BEGIN(Client)


class CNot_Affected_Render final :public CGameObject
{
private:
	CNot_Affected_Render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNot_Affected_Render(const CNot_Affected_Render& rhs);
	virtual ~CNot_Affected_Render() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_Rander_Trigger(_bool RanderCheck) { m_bRanderTrigger = RanderCheck; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
	_bool					m_bRanderTrigger = true;

	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	_float					m_ArrKey[3];
private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

public:
	static CNot_Affected_Render* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;

};

END