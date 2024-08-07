#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END

#define LOBO_MAXMOVEARR 8
class CLoboto final :public CGameObject
{
private:
	enum LOBOANIM { LA_IDLE, LA_WALK, LA_LOOKAROUND, LA_TALK,LA_RUN, LA_DISTANCE_IDLE, LA_DISTANCE, LA_MOCKING, LA_LAUGH,LA_END };
	/* Animation State
	 * 0 : Walk With VacationPackage
	 * 1 : Idle With VP
	 * 2 : LookAround With VP _ RIGHT
	 * 3 : LookAround With VP _ LEFT
	 * 4 : TALK Long
	 * 5 : TALK Ponder
	 * 6 : Run WithOut VP
	 * 7 : Idle WithOut VP ( For DistanceEvent )
	 * 8 : Idle To Mocking ( For DistanceEvent )
	 * 9 : Idle Mocking ( For DistanceEvent )
	 * 10 : Laugh ( End Of DistanceEvent )
	 * 11 : WalkUp Wall 
	*/ 

protected:
	explicit CLoboto(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLoboto(const CLoboto& rhs);
	virtual ~CLoboto() = default;

public:
	void	SetUpPointNum(_int iNum=-1);
	void	Set_Move(_bool isMove);
private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_MovePoints();

private:
	void	MoveToPoint(_double TimeDelta);
	void	SetUpForDistanceEvent();
	void	SetAnimationByMoveStartPoint();
	void	SetAnimationByMoveEndPoint();
	void	SetUpAnimation();
	void	UpdateAnimState(_uint iNextAnim);
	void	DemoRect();
	void	Destroy_RectEvent(_double TimeDelta);
public:
	void	Set_MoveStartLoboto(_bool bMove);
private:
	//	Trigger발동시 이동하게하는 변수 (TriggerEvent)
	_bool		m_isMove = false;
	//	Trigger발동시 이동하게하는 변수 (distanceEvent)
	_bool		m_isDistanceMove = false;
	/*	TriggerEnter시 반환되는 숫자를 통해 
	 *	TriggerEvent시 이동범위를 설정 해줄 변수
	 */
	_int		m_iTriggerCnt = -1;

	// TriggerEvent시 이동 사이 보간을 위한 변수
	_double		m_lfLerpTimeAcc = 0.0;
	// TriggerEvent시 이동 사이 보간을 위한 변수
	_double		m_lfLerpTimeMax = 5.0;

	// TriggerEvent시 이동 포인트를 저장한 vector
	vector<_float4> m_vecMovePoints;

	// m_iTriggerCnt의 값에 따라 이벤트의 이동 시작값과 도착값을 설정해줄 배열
	_uint			m_iArrVecNum[LOBO_MAXMOVEARR] = { 1,4,8,9,11,12,13,23 };
	// 배열에서 뽑아온 값을 통해 시작값과 끝값을 저장해줄 변수
	_uint m_iStartPointNum = 0;
	// 배열에서 뽑아온 값을 통해 시작값과 끝값을 저장해줄 변수
	_uint m_iEndPointNum = 1;

	// 현재 이동중인 인덱스를 표현해 주기 위한 변수
	_uint m_iCurrentMovePoint = 0;

	LOBOANIM m_iCurAnimNum = LOBOANIM::LA_IDLE;
	LOBOANIM m_iNextAnimNum = LOBOANIM::LA_IDLE;
	_bool		m_isLookAroundLeft = false;
	_bool		m_isUpdateDistanceAnim = false;
	_bool		m_isLerpForWall = false;
	_bool		m_isRenderVP = true;
	//_bool		m_bActive = false;
	_double		m_dAnimSpeed = 1.0;
private:/*For.DemoRect*/
	_bool		m_bIsDestroyDemoRect = false;
	_bool		m_bActiveDestroyDemoRect = false;
	_double		m_dDestoryTimeAcc = 0.0;
public:
	static CLoboto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

