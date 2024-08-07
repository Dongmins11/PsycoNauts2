#pragma once

#include "Component.h"
#include "PhysXScene_Manager.h"
BEGIN(Engine)

class CGameObject;
class CTransform;

class ENGINE_DLL CPhysXCollider abstract : public CComponent
{
public:
	typedef struct tagDesc
	{
		/* For.Rigidbody */
		// 타입- STATIC:움직이지 않는 고정된 actor / DYNAMIC:움직이면서 물리연산을 받는 actor
		CPhysXScene_Manager::TYPE eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		// 랜더를 할것인가?
		_bool isVisualization = false;		
		// 중력이 작용하는가?
		_bool isGravity = false;	
		// 다른 액터로부터 물리 연산을 받지 않는가?
		_bool isKinematic = false;	
		// 질량-  높을수록 무겁다
		_float fMass = 1.f;

		/* For.Shape */
		// CCT로 밀거나 물리작용하는가?
		_bool isSceneQuery = false;
		// 트리거로 만들것인가?
		_bool isTrigger = false;								

		/* For.Material */
		// 정적 마찰 - 보통 정적 마찰이 동적 마찰보다 값이 크다
		_float fStaticFriction = 0.5f;		
		// 동척 마찰력
		_float fDynamicFriction = 0.5f;
		// 탄성력
		_float fRestitution = 0.6f;			

		/* For.Collider */
		CGameObject* pGameObject = nullptr;
		//CTransform*		pTransform = nullptr;
	}DESC;

protected:
	explicit CPhysXCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CPhysXCollider(const CPhysXCollider& _rhs);
	virtual ~CPhysXCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* pArg)override;

public:
	const DESC& Get_Desc() const;

public:
	HRESULT Set_ShapeLayer(LayerType eLayerType);

	void SetRigidActor(PxRigidActor* _pRigidbody);
	void SetShape(PxShape* _pShape);
	void SetMaterial(PxMaterial* _pMaterial);
	void SetMass(const _float _fValue);
	void SetPivotMatrix(_fmatrix _PivotMatrix);
	void SetGravity(const _bool _isGravity);
	void SetKinematic(const _bool _isKinematic);
	void SetTrigger(const _bool _isTrigger);
	void SetRestitution(const _float fValue);
public:
	void Update_Transform();
	void Update_Transform(_float4x4* _pWorldMatrix);
	void Update_TransformPhysX();
	void Update_TransformPhysX(_fmatrix _WorldMatrix);

public:
	void Remove_Actor();
	void Add_Actor();
	void Sleep();
	void WakeUp();
	void Add_Force(_fvector vPow);
	void Add_Force(_fvector vPow, PxForceMode::Enum eMode);
	void Add_Torque(_fvector vAxis, PxForceMode::Enum eMode = PxForceMode::eFORCE);
	void Reset_Power();
	_bool Get_Gravity() const { return m_tDesc.isGravity; }
	PxShape* Get_Shape();
protected:
	//CPhysXScene_Manager* m_pPhysX = nullptr;
	PxRigidActor* m_pRigidActor = nullptr;
	PxShape*	m_pShape = nullptr;
	PxMaterial* m_pMaterial = nullptr;
	CGameObject*	m_pGameObject = nullptr;
	CTransform*		m_pTransform = nullptr;

	DESC m_tDesc;
	_float4x4 m_PivotMatrix;
	

public:
	virtual void Free() override;
};

END