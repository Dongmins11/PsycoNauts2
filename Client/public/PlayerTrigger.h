#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)
class CPlayerTrigger : public CGameObject
{
public:
	CPlayerTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayerTrigger(const CPlayerTrigger& rhs);
	virtual ~CPlayerTrigger() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual void Set_Position(_fvector vPos = XMVectorZero()) {};
	virtual void Set_Matrix(const _float4x4* pMatPtr) {};
protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnTriggerStay(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;

#ifdef _DEBUG
	CCollider*				m_pRenderCollider = nullptr;
#endif // _DEBUG

//private:
protected:
	HRESULT SetUp_Component(_uint iSceneIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg);
	//HRESULT SetUp_ConstantTable();

protected:
	// 플레이어용 트리거 타입Enum
	PlayerTriggerType		m_eTriggerType = PlayerTriggerType::PTTYPE_END;
	// 트리거 피봇위치
	_float3					m_vColPos = { 0,0,0 };

public:
	static CPlayerTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};
END
