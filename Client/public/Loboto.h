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
	//	Trigger�ߵ��� �̵��ϰ��ϴ� ���� (TriggerEvent)
	_bool		m_isMove = false;
	//	Trigger�ߵ��� �̵��ϰ��ϴ� ���� (distanceEvent)
	_bool		m_isDistanceMove = false;
	/*	TriggerEnter�� ��ȯ�Ǵ� ���ڸ� ���� 
	 *	TriggerEvent�� �̵������� ���� ���� ����
	 */
	_int		m_iTriggerCnt = -1;

	// TriggerEvent�� �̵� ���� ������ ���� ����
	_double		m_lfLerpTimeAcc = 0.0;
	// TriggerEvent�� �̵� ���� ������ ���� ����
	_double		m_lfLerpTimeMax = 5.0;

	// TriggerEvent�� �̵� ����Ʈ�� ������ vector
	vector<_float4> m_vecMovePoints;

	// m_iTriggerCnt�� ���� ���� �̺�Ʈ�� �̵� ���۰��� �������� �������� �迭
	_uint			m_iArrVecNum[LOBO_MAXMOVEARR] = { 1,4,8,9,11,12,13,23 };
	// �迭���� �̾ƿ� ���� ���� ���۰��� ������ �������� ����
	_uint m_iStartPointNum = 0;
	// �迭���� �̾ƿ� ���� ���� ���۰��� ������ �������� ����
	_uint m_iEndPointNum = 1;

	// ���� �̵����� �ε����� ǥ���� �ֱ� ���� ����
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

