#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END

BEGIN(Client)
class CKitchenPanButton final : public CGameObject
{

private:
	explicit CKitchenPanButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CKitchenPanButton(const CKitchenPanButton& rhs);
	virtual ~CKitchenPanButton() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void OnTriggerEnter(CCollisionObject& collision) override;

private:
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;

private:
	_bool			m_bCheck[3] = { false, false, false };
	_uint			m_iLayerIndex = 0;
	_uint			m_iPX_TYPE = 0;
	_double			m_dTimeAcc[3] = { 0.0, 0.0, 0.0 };
	_vector			m_vInitialPos = XMVectorZero();

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();
	void Move_Update(_double TimeDelta);

public:
	static CKitchenPanButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END