#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "EnviromentTrigger.h"
#include "Fruit.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CKitchenPan final : public CGameObject
{
private:
	explicit CKitchenPan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CKitchenPan(const CKitchenPan& rhs);
	virtual ~CKitchenPan() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	class CFruit* Get_Pan_Fruit() { return m_pTargetFruit; }
	void  Set_Pan_Fruit(class CFruit* pFruit) { m_pTargetFruit = pFruit; }
	void Set_RenderFire() { m_bRandStart = false; }
	void Set_IsCookFinished(_bool isFinished) { m_IsCookFinished = isFinished; }
private:
	HRESULT SetUp_Trigger();
	void Move_Pan(_double TimeDelta);
	void Rand_Make(_double TimeDelta);

private:
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CGameObject*			m_pEnvironTrigger = nullptr;

public:
	void Set_Move(_bool IsMove, _int iIndex) { m_IsMove = IsMove, m_iPosIndex += iIndex; }

private:
	EnviromentTriggerType m_eType = EnviromentTriggerType::ETTYPE_COOKING;
	// 후라이팬과 불의 상호작용
	_bool	m_IsCollision = false;			// 후라이팬과 불이 겹치고 있는지 판단
	_bool	m_IsMove = false;				// 플레이어가 버튼을 눌렀을시 True
	_bool	m_IsCookFinished = false;		// 요리가 끝났는지 (m_dCookFinishedTimeAcc 보다 m_dCollisionTimeAcc 이 커지면 True반환)
	_int	m_iPosIndex = 2;				// 최초 후라이팬 위치 인덱스

	_double	m_dCollisionTimeAcc = 0.0;		// 후라이팬과 불이 겹치는 시간
	_double m_dCookFinishedTimeAcc = 10.0;	// 최종 후라이팬과 불이 겹쳐 요리가 완성될때까지 걸리는 시간

	// 후라이팬의 기본적인 변수
	_float3 m_iPanPos[5] = {};		// 후라이팬이 이동가능한 위치
	_float4x4 m_WorldMatrix;		// Trigger를 만들기 위한 멤버변수

	// 난수
	_bool	m_bRandStart = true;	// 처음 조건이 들어오면 5초마다 난수를 만드는 조건
	_uint	m_iRandIndex = 0;		// 난수 인덱스
	_double m_dRandTimer = 0.0;		// 랜덤한 숫자 만드는 시간 누적 타이머(5초에 한번씩 난수 생성)
	_float3 m_vecObjectPos = { 0.f,0.f,0.f };
private:
	CFruit*					m_pTargetFruit = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

public:
	static CKitchenPan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END