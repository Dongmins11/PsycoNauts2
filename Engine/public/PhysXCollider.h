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
		// Ÿ��- STATIC:�������� �ʴ� ������ actor / DYNAMIC:�����̸鼭 ���������� �޴� actor
		CPhysXScene_Manager::TYPE eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		// ������ �Ұ��ΰ�?
		_bool isVisualization = false;		
		// �߷��� �ۿ��ϴ°�?
		_bool isGravity = false;	
		// �ٸ� ���ͷκ��� ���� ������ ���� �ʴ°�?
		_bool isKinematic = false;	
		// ����-  �������� ���̴�
		_float fMass = 1.f;

		/* For.Shape */
		// CCT�� �аų� �����ۿ��ϴ°�?
		_bool isSceneQuery = false;
		// Ʈ���ŷ� ������ΰ�?
		_bool isTrigger = false;								

		/* For.Material */
		// ���� ���� - ���� ���� ������ ���� �������� ���� ũ��
		_float fStaticFriction = 0.5f;		
		// ��ô ������
		_float fDynamicFriction = 0.5f;
		// ź����
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