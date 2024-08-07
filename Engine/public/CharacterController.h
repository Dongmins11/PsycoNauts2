#pragma once

#include "Component.h"
#include "PhysXScene_Manager.h"

BEGIN(Engine)

class CControllerFilterCallback;
class CTransform;
class CGameObject;

class ENGINE_DLL CCharacterController final : public CComponent
{
public:
	typedef struct tagDesc
	{
		_float fHeight = 0.f;								// 가운데 기둥 길이
		_float fRadius = 0.f;								// 기둥 넓이와 위아래 구의 반지름
		_float fContactOffset = 0.1f;						// 접촉 오프셋 : 0.f 로 설정시 터짐, 공식적으로  최소 0.01 ~ 반지름의 1/10 크기를 권장함
		_float fStepOffset = 0.f;							// 계단 높이와 같이 어느정도 높이는 그냥 올라가지게 만드는 수치
		_float fSlopeLimit = 10.f;							// 오를수 있는 경사의 한계수치

		/* For.Material */
		_float fStaticFriction = 0.5f;						// 정적 마찰
		_float fDynamicFriction = 0.5f;						// 동적 마찰
		_float fRestitution = 0.6f;							//	부딛혔을때 튕기는 탄성력(?)

		_float3 vPosition = { 0.f, 0.f, 0.f };				// 오프셋 : 설정안해줘도 됨.
		_float3 vUpDirection = { 0.f, 1.f, 0.f };			// 업벡터 : 플레이어나 몬스터는 업벡터가 대부분 0, 1, 0
		PxCapsuleClimbingMode::Enum eClimbingMode = PxCapsuleClimbingMode::eCONSTRAINED;		// 등반모드 
		CGameObject*		pGameObject = nullptr;
		//CTransform*			pTransform = nullptr;
	}DESC;

private:
	explicit CCharacterController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CCharacterController(const CCharacterController& _rhs);
	virtual ~CCharacterController() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* _pArg = nullptr) override;
public:
	const DESC& Get_Desc() const;
	const _vector Get_CenterPos()const;
	const _vector Get_TopPos()const;
	const _vector Get_FootPos()const;
public:

	void SetMaterial(PxMaterial* _pMaterial);
	void SetPxController(PxController* _pPxController);
	void SetShapes(vector<PxShape*>& _vecShapes);
	void SetShaperLayer(LayerType eType);
	void SetHeight(_float Height);
	const _bool IsDown();
	const _bool IsUp();
	const _bool IsSides();

public:
	void		Remove_CharacterController();
	void Update_Transform();
	void Move(const _double& _dTimeDelta, const _float3 _vDisp);
//	void MoveImm(const _double& lfTimeDelta, const _float3 _vDisp);
	void Set_Position(const _float3 _vPos);
	void Set_FootPosition(const _float3 _vPos);

	const PxControllerCollisionFlags& Get_Flags() const{
		return m_curFlag;
	}

private:
	HRESULT Create_Controller();

private:
	DESC m_tDesc;
	PxControllerCollisionFlags m_preFlag;
	PxControllerCollisionFlags m_curFlag;
	PxFilterData m_filterData;

	CTransform*			m_pTranform = nullptr;
	PxController* m_pPxController = nullptr;
	//CControllerFilterCallback* m_pFilterCallback = nullptr;
	PxUserControllerHitReport* m_pControllerHitCallback = nullptr;
	PxMaterial* m_pMaterial = nullptr;

	vector<PxShape*> m_vecShapes;

public:
	CTransform* Get_Transform() const;

private:
	PxControllerCollisionFlags m_PreFlag;
	PxControllerCollisionFlags m_CurFlag;

	_float3 m_vVelocity = _float3(0.f,0.f,0.f);

public:
	static CCharacterController* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	virtual CComponent* Clone(void* _pArg = nullptr);
	virtual void Free() override;
};

END