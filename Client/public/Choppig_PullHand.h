#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)

class CChoppig_PullHand final :public CGameObject
{
protected:
	explicit CChoppig_PullHand(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CChoppig_PullHand(const CChoppig_PullHand& rhs);
	virtual ~CChoppig_PullHand() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	virtual void OnTriggerExit(CCollisionObject& collision);

public:
	void	Set_SwitchButtonCheck(_bool bButtonCheck) { m_bSwitchButtonCheck = bButtonCheck; }
	const _bool	Get_SwitchButtonCheck() const { return m_bSwitchButtonCheck; }
public:
	const _float4 Get_Trigger_Position() const { return m_vTriggerPosition; }
	const _float4 Get_Trigger_LookDirection() const { return m_vTriggerLook; }

private:
	CRenderer*				m_pRendererCom = nullptr;

	CPhysXCollider*			m_pColliderCom = nullptr;

	CPhysXCollider*			m_pPhysXColliderCom = nullptr;

	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pModelCom = nullptr;

private: /* For.SwitchHand */
	_float3					m_vColPos = { 0,0,0 };
	_float4x4*				m_pColiderMatrix = nullptr;
	_float4x4*				m_pPhysXColiderMatrix = nullptr;
private:
	_float					m_fPositionLook = 0.f;
	_bool					m_isMoveCheck = false;
private:
	_float4					m_vBackUpPosition = {};
	_float4					m_vCurrentBackUpPosition = {};
	_double					m_lfBackUpTimeAcc = 0.0;
	_bool					m_bFirstBackUpCheck = false;
private:
	_bool					m_bSwitchButtonCheck = false;
private:
	_float4					m_vTriggerPosition = {};
	_float4					m_vTriggerLook = {};
private:
	_uint					m_iMyCount = 0;
	_uint					m_iMyTargetCount = 0;

private: /* For.Cyilnder*/
	_float3					m_vScale = {};
	_int					m_iBackUpLength = 0;
	_int					m_iLength = 0;
	_uint					m_iMaxDestination = 0;


private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

private:
	HRESULT	Colider_PosUpdate();
	HRESULT Model_PosUpdate(_double TimeDelta);
private:
	void	BackUp_PositionUpdate(_double TimeDelta);

private:	/* For.Cyilnder */
	void	Cylinder_ScaleUpdate(_double TimeDelta);

public:
	static CChoppig_PullHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END