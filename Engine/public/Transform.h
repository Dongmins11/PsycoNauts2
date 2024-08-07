#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_double			SpeedPerSec;
		_double			RotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	_float3 Get_State_Float3(STATE eState) {
		return *(_float3*)&m_WorldMatrix.m[eState];
	}

	_vector Get_Position() const {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[STATE_POSITION][0]);
	}

	_vector Get_Scale();
	_float3 Get_ScaleFloat3();

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_matrix Get_WorldMatrixTranspose() {
		return XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix));
	}

	_matrix Get_WorldMatrixInverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_float4x4 Get_WorldFloat4x4() {
		return m_WorldMatrix;
	}

public:/* For.TransformDesc */
	void	Set_TransFormDesc(_double lfRotationPerSec, _double lfSpeedPerSec);
	_double Get_TransFormDescSpeed() { return m_TransformDesc.SpeedPerSec; }
	_double	Get_TransFormDescRotation() { return m_TransformDesc.RotationPerSec; }

	// 현재 오브젝트의 빌보드 행렬을 받아오는 함수 매개변수 1: 카메라좌표 매개변수 2: 오브젝트의 스케일
	// * 현재 오브젝트의 좌표값을 사용하므로 위치 변경이 필요할 시 Reset이 필요
	_float4x4 Get_BillBoardFloat4x4(_fvector CamPos, _float3 vSize = _float3(1.f,1.f,1.f));
	_matrix Get_BillBoardMatrix(_fvector CamPos, _float3 vSize = _float3(1.f, 1.f, 1.f));

	const _float4x4* Get_WorldMatrixPtr() {
		return &m_WorldMatrix;
	}

public: /* For.CharacterController */
	const _float3& Get_Velocity() const;
	void Set_Velocity(const _fvector& vVelocity);
	void Add_Velocity(const _fvector& vVelocity);
	void Add_Velocity(const STATE _eState, const _float _fValue);

public:

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState);
	}

	void Set_State_Float3(STATE eState, _float3 vState) {
		*(_float3*)&m_WorldMatrix.m[eState]= vState;
	}

	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) {
		m_TransformDesc = TransformDesc;
	}
	void Set_WorldMatrix(_float4x4 fmatix) { 
		m_WorldMatrix = fmatix; 
	}

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Go_Straight(_double TimeDelta, class CNavigation* pNavigation = nullptr);
	HRESULT Go_Backward(_double TimeDelta);
	HRESULT Go_Left(_double TimeDelta);
	HRESULT Go_Right(_double TimeDelta);
	HRESULT Go_Straight(_double TimeDelta, class CCharacterController* _pController);
	HRESULT Go_Backward(_double TimeDelta, class CCharacterController* _pController);
	HRESULT Go_Left(_double TimeDelta, class CCharacterController* _pController);
	HRESULT Go_Right(_double TimeDelta, class CCharacterController* _pController);

	HRESULT Go_ByAxis(_fvector vAxis, _double TimeDelta, _fvector vPos = XMVectorZero(), class CNavigation* pNavigation = nullptr);
	HRESULT Go_ByAxis(_fvector vAxis, _double TimeDelta, _fvector vPos, class CCharacterController* pController);
	HRESULT RotationPerSec(_fvector vAxis, _double TimeDelta);
	HRESULT RotationByRadian(_fvector vAxis, _float fRadian);
	HRESULT RotationByRadian(_fvector vAxis, _float3 fRadian);
	HRESULT RotationQuaternion(_fvector vQuaternion);
	HRESULT Scaling(_float3 vScale);
	HRESULT Scaling(_fvector vScale);

	// 빌보드 시켜줌
	// 매개변수 1: 카메라좌표 / 매개변수2 : Default값 : 현재 transform의좌표
	HRESULT BillBoard(_fvector vCamPos, _fvector vPosition = XMVectorZero());
	HRESULT BillBoardY(_fvector vCamPos, _fvector vPosition = XMVectorZero());

	HRESULT LerpBillBoardRotation(_fvector vSrcLook, _fvector vDestLook, _double lfLerpTimeAcc, _double lfLerpMaxTime);

	// 점프함수 ( 수정이 필요할 수 있음 )

	void Jump(_double lfTimeDelta, _bool* isJump, _float* startY, _double* lfJumpTime, _float* jumpPower, _float* fJumpValue, _bool* bFallCheck, _bool* bLandCheck, class CCharacterController* _pController = nullptr, CNavigation* pNavigation = nullptr);
	void Double_Jump(_double lfTimeDelta, _bool* isJump, _float* startY, _float* EndY, _double* lfJumpTime, _float* jumpPower, _float* fJumpValue, _bool* bFallCheck, _bool* bLandCheck, class CCharacterController* _pController = nullptr, CNavigation* pNavigation = nullptr);
	void Ledge_Jump(_double lfTimeDelta, _bool* isJump, _float* startY, _double* lfJumpTime, _float* jumpPower, _float* fJumpValue, _bool* bFallCheck, _bool* bLandCheck,_bool* bControllerCheck, class CCharacterController* _pController = nullptr);


	// 내가 쳐다볼곳 함수
	void Set_MyLook(_fvector vLook);
	void Set_MyUp(_fvector vUp);
private:
	TRANSFORMDESC		m_TransformDesc;
	_float4x4			m_WorldMatrix;
	_float3				m_vVelocity = _float3(0.f, 0.f, 0.f);
	_float3				m_vColPivot = _float3(0.f, 0.f, 0.f);
	
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END