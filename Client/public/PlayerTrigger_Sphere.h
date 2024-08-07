#pragma once
#include "Client_Defines.h"
#include "PlayerTrigger.h"


BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)
class CPlayerTrigger_Sphere : public CPlayerTrigger
{
public:
	CPlayerTrigger_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayerTrigger_Sphere(const CPlayerTrigger_Sphere& rhs);
	virtual ~CPlayerTrigger_Sphere() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual void Set_Position(_fvector vPos = XMVectorZero());
protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	//virtual void OnTriggerStay(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;

#ifdef _DEBUG
	CCollider*				m_pRenderCollider = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
//	HRESULT SetUp_ConstantTable();

protected:
	// �÷��̾�� Ʈ���� Ÿ��Enum
	PlayerTriggerType		m_eTriggerType = PlayerTriggerType::PTTYPE_END;
	// Ʈ���� �Ǻ���ġ
	_float3					m_vColPos = { 0,0,0 };

public:
	static CPlayerTrigger_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END