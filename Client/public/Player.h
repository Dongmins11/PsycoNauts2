#pragma once

#include "Client_Defines.h"
#include "Client_PlayerAnimationDefines.h"
#include "PhysXScene_Manager.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CVIBuffer_Rect;
class CNavigation;
class CCharacterController;
class CPhysXScene_Manager;
class CSphereCollider;
END

BEGIN(Client)
class CPlayer_LeftHand;
class CPlayer_RightHand;
class CPlayer_AnimState;

class CPlayerTrigger_Parkour;
class CPlayerTrigger_Body;
class CPlayer final : public CGameObject
{
public:
	enum TEXTRUETYPE { TEXTRUETYPE_MOUSE, TEXTRUETYPE_END };
	enum HPCOUNT {
		HPCOUNT_DEMO = 0,
		HPCOUNT_STAGE1 = 3,
		HPCOUNT_STAGE2 = 4,
		HPCOUNT_STAGE3 = 5,
		HPCOUNT_STAGE4 = 6,
		HPCOUNT_STAGE5 = 7,
		HPCOUNT_END
	};
	enum SWITCHMOVE
	{
		SWITCHMOVE_FORWARD,
		SWITCHMOVE_BACKWARD,
		SWITCHMOVE_END
	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	virtual void OnTriggerEnter(CCollisionObject& collision);
	virtual void OnControllerCollisionHit(CCollisionObject& collision);

public:
	void SetUp_PlayerPosition(SCENE eScene);
private:
	void Player_Sliding(_double TimeDelta);
private:
	HRESULT	Render_Change();
	HRESULT Bone_MatrixCalculation();

	/* Test용 함수(Stage03- 중보->요리대회 이동시 호출함수) */
public:
	void Setting_CookingContestUI();
public:
	/* Test용 함수(Quest) */
	void Setting_PlayerQuest(_uint iQuestNum, _bool isFirst =false);
	void Reset_PlayerQuest(_uint iQuestNum);
	const _int Get_QuestArr(_uint iArrNum)const { return m_iCurrentQuestArr[iArrNum]; }
public:
	void	Set_GrabSuccessCheck(_bool bSuccessCheck);
public:
	const _bool Get_IsGround() const { return m_bIsGround; }
	const _bool Get_IsParkour() const { return m_isParkour; }

public:
	void	Set_RimLightEnable(_bool bEnable) { m_bRimLightEnable = bEnable; }

public:		/* For.Dead */
	void	Set_IsDead(_bool IsDeadCheck) { m_IsDead = IsDeadCheck; }
	const _bool	Get_IsDead() const { return m_IsDead; }

public:		/* For.KnockBack*/
	void	Set_IsKnockBack(_bool IsKnockBack) { m_IsKnockBack = IsKnockBack; }
	const _bool Get_IsKnockBack() const { return m_IsKnockBack; }
public:
	const _uint Get_Stage03PhaseNum() const { return m_iCookingStage_PhaseNum; }
	void Add_Stage03PhaseNum() { m_iCookingStage_PhaseNum += 1; }
public:		/* For.Slide */
	const _bool Get_IsSlide() const { return m_isSlider; }
	const _float4 Get_UpRayDirection() const { return m_vUpRayDirection; }
	void	Set_IsSlide(_bool bIsSlider) { m_isSlider = bIsSlider; }

public:		/* For.Pull_Switch_Get */
	const _bool	Get_PullSwitch() const { return m_isPullSwitch; }
	const _bool	Get_PullSwitchingEnd() const { return m_isPullSwitchingEnd; }
	const _bool	Get_PullSwitchMove(SWITCHMOVE eMove) const { return m_isPullSwitchMove[eMove]; }

public:		/* For.Pull_Switch_Set */
	void	Set_PullSwitch(_bool IsSwitch) { m_isPullSwitch = IsSwitch; }
	void	Set_PullSwitchingEnd(_bool IsSwitchingEnd) { m_isPullSwitchingEnd = IsSwitchingEnd; }
	void	Set_PullSwitchMove(_bool bIsSwitchMove, SWITCHMOVE eMove) { m_isPullSwitchMove[eMove] = bIsSwitchMove; }

public:
	void Set_IsParkour(_bool isParkour) { m_isParkour = isParkour; }
public:		/* For.CamShaking */
	void	Set_HitCheck(_bool bHitCheck) { m_bHitCheck = bHitCheck; }
	void	Set_HitCamShaking(_bool bCamEnable) { m_bCamShackChecking = bCamEnable; }
	_bool	Get_HitCheck() const;
	_bool	Get_HitingCheck() const;
	_bool	Get_CamShake() const { return m_bCamShackChecking; }

public:		/* For.KeyCheck */
	void	Set_KeyCheck(_bool bKeyCheck) { m_bKeyCheck = bKeyCheck; }
	_bool	Get_KeyCheck() const { return m_bKeyCheck; }

public:		/*For. RenderCheck*/
	void	Set_RenderCheck(_bool bIsRender) { m_bIsRenderChange = bIsRender; }
	_bool	Get_RenderCheck() const { return m_bIsRenderChange; }

public:		/*For. MoveCheck */
	void	Set_MoveCheck(_bool bMoveCheck) { m_bMoveCheck = bMoveCheck; }
	_bool	Get_MoveCheck() const { return m_bMoveCheck; }

public:		/*For. SliderTriggerCheck */
	void	Set_SliderTriggerCheck(_bool IsSlider);
	_bool	Get_SliderTriggerCheck() const;

public:		/*For. WalkCheck */
	void	Set_WalkTiggerCheck(_bool IsWalk);
	_bool	Get_WalkTriggerCheck() const;

public:     /*For. Curling */
	void    Set_CurlingIndex(_int iCurlingIndex) { m_iCurlingIndex = iCurlingIndex; }
	void    Set_Delete_CurlingIndex(_int iDeleteCurlingIndex) { m_iDeleteCurlingIndex = iDeleteCurlingIndex; }
	_int    Get_CurlingIndex() { return m_iCurlingIndex; }
	_int    Get_Delete_CurlingIndex() { return m_iDeleteCurlingIndex; }

public:		/*For. JumpCheck*/
	_bool	IsJumping();
	void	Set_TrampolineJump();

public:		/*For.Is AnimUpdate*/
	void	Set_IsAnimUpdate(_bool bCheck) { m_bIsAnimUpdate = bCheck; }
	_bool	Get_IsAnimUpdate() { return m_bIsAnimUpdate; }
	
public:		/* For.Attack */
	_bool	Get_AttackCheck() const;
	void	Set_AttackMoveSpeed(_double lfAttackMoveSpeed);
	void	Set_DashAttackMoveSpeed(_double lfDashAttackMoveSpeed);

public:		/*For.Is AnimUpdate*/
	void	Set_IsAlphaDown(_double dSpeed) { m_bIsAlphaDown = true, m_dAlphaSpeed = dSpeed, m_bIsAlphaUp = false, m_iRenderPass = 7, m_fDecreaseAlpha = 0.0; }
	void	Set_IsAlphaUp(_double dSpeed) { m_bIsAlphaUp = true, m_dAlphaSpeed = dSpeed, m_bIsAlphaDown = false; }
private:	/*For. CameraCollide*/
	void	Set_Alpha(_double TimeDelta);

private:	/*For.HpBar*/
	_int	Update_HpBar(_double TimeDelta);

public:		/*For.HpBar*/
	void	Set_IsHeal(_bool bCheck) { m_IsGetHealPack = bCheck; }
	void	Set_IsDamage(_bool bCheck) { m_IsGetDamaged = bCheck; }
	void	Set_HpCreate(SCENE iScene);
	void	Set_SettingHp(_int iCount) { m_iSettingHp = iCount; }
	void	Set_HpRender(SCENE iScene, _bool bCheck);

public:		/*For.Bouncer*/
	_bool	Get_IsBouncer(_uint iIndex) { return m_IsBouncer[iIndex]; }
	void	Set_IsBouncer(_bool bBouncer, _uint iIndex) { m_IsBouncer[iIndex] = bBouncer; }

public:		/*For.KitchenButton*/
	_bool	Get_IsKitchenButton(_uint iIndex) { return m_IsKitchenButton[iIndex]; }
	void	Set_IsKitchenButton(_bool bKitchen, _uint iIndex) { m_IsKitchenButton[iIndex] = bKitchen; }


public:/*For. Rail*/
	_float3	Get_HitPos() { return m_vHitPos; }
	void	Set_RailValues(_double dStartSpeed, _double dDefaultSpeed, _double dFastSpeed, _double dSlowSpeed, _double dNonFixJumpPower,
		_double dRailCollisionCheckTerm, _double dJumpCorrection, _double dStartJumpAccMax, _double dRayTrans, _double dJumpPower,
		_double dJumpFallSpeed, _double dLookSpeed, _double dMoveSpeed, _double dAdjustSpeed);
	void	Set_CollsionObject(CGameObject* pCollisionObject);
private:
	_float3	m_vHitPos;

public: /* For. Climbing */
	void  Set_Climbing_TriggerCheck(_bool ClimbingCheck, _int iTriggerNum) { m_bClimbingCheck = ClimbingCheck; m_iTriggerNum = iTriggerNum; }
	void  Set_Climbing_TriggerEndCheck(_bool ClimbingEnd) { m_bClimbingEndCheck = ClimbingEnd; }

	_bool Get_Climbing_TriggerCheck() { return m_bClimbingCheck; }
	_bool Get_Climbing_TriggerEndCheck() { return m_bClimbingEndCheck; }

	_int  Get_Climbing_TriggerNum() { return m_iTriggerNum; }

public:	/* For. Dish*/
	void  Set_Boss_DishCheck(_bool bDishMotion, _uint iDishType) { m_bDishMotionCheck = bDishMotion; m_iDishType = iDishType; }
	const _bool Get_DishCheck() const { return m_bDishMotionCheck; }
	const _uint Get_DishType() const { return m_iDishType; }

public:/*For. OffsetRGB*/
	void Set_OffsetRGB(_float3 vRGB) { m_vOffsetRGB = vRGB; }

private:
	void Sort_PlayerQuest(_int iNumber = -1);
	void Update_PlayerQuestKeyDown();
	void Move_QuestSelecting();
	void Tap_Quest();
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CSphereCollider*		m_pColliderCom = nullptr;
	CPlayer_AnimState*		m_pAnimState = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;

private:	/*For.Hand*/
	CPlayer_LeftHand*		m_pLeftHand = nullptr;
	CPlayer_RightHand*		m_pRightHand = nullptr;

private:	/*For.SkillRender*/
	_bool					m_bIsRenderChange = false;
	TEXTRUETYPE				m_eTextureType = TEXTRUETYPE_END;

private:
	CPlayerTrigger_Parkour*	m_pTriggerParkour = nullptr;
	CPlayerTrigger_Body*	m_pTriggerBody = nullptr;

private:	/*For.Key */
	_bool					m_bKeyCheck = false;
	_double					m_lfMoveValue = 0.0;

private:	/*For_Move*/
	_bool					m_bMoveCheck = false;

private:
	_bool					m_bIsGround = false;
	_bool					m_isParkour = false;

private:
	_bool					m_bHitCheck = false;
	_bool					m_bChangeCheck = false;
private:
	_float					m_fHeight = 0.f;

private:  /* For.Curling */
	_int					m_iCurlingIndex = 0;
	_int					m_iDeleteCurlingIndex = -1;

private:/*For.MotionBlur*/
	_float4x4				m_OldWolrd[3];
	_float4x4				m_OldView[3];
private:/*For.Shadow*/
	_float					m_ArrKey[3];
	_bool					m_bIsAnimUpdate = true;

private:	/* For. My Bone_RightHand */
	_float4x4				m_MyWorldMatrix;
	_float4x4				m_MyOffsetMatrix;
	_float4x4				m_MyPivotMatrix;
	const _float4x4*		m_pMyCombinedMatrix = nullptr;
private:
	_float4x4				m_ColiderMatirx = {};
	_float4x4*				m_pBoneMatrix = nullptr;

private:/*For. CameraCollide*/
	_float					m_fDecreaseAlpha = 0.f;
	_double					m_dAlphaSpeed = 0.0;
	_bool					m_bIsAlphaUp = false;
	_bool					m_bIsAlphaDown = false;

private: /* For. Effect */
	_float3					m_vTargetEffectPos;


private: /*For.  Climbing */
	_bool				    m_bClimbingCheck = false;
	_bool					m_bClimbingEndCheck = false;
	_int				    m_iTriggerNum = -1;

private:  /*For. CamShaking */
	_bool					m_bCamShackChecking = false;

private:	/*For.RimLight*/
	_bool					m_bRimLightEnable = false;

private:	/* For.Dish */
	_bool					m_bDishMotionCheck = false;
	_uint					m_iDishType = -1;


private: /*For.PlayerHpBar*/
	SCENE	m_iHpCreateScene = SCENE_END;		// 씬마다 몇개의 체력을 만들것인가?
	_bool	m_IsGetHealPack = false;
	_bool	m_IsGetDamaged = false;
	_bool	m_IsDecrease = false;				// 체력 감소인가?
	_bool	m_IsIncrease = false;				// 체력 증가인가?
	_bool	m_IsDefault = false;				// 기본 체력인가?
	_int	m_iSettingHp = 1;					// 피가 줄거나 늘어나는 갯수
	_uint	m_iMaxHp = 0;						// 최대체력
	_uint	m_iCurrentHp = 0;					// 현재체력
	_uint	m_iBackUpHp = 0;					// 잔상체력
	_uint	m_iBackUpLayerIndex = 0;			// 객체 레이어 인덱스
	_double	m_dDecreaseTimeAcc = 0.0;			// 체력 감소 시간 누적
	_double	m_dIncreaseTimeAcc = 0.0;			// 체력 회복 시간 누적

private:	/* For.Slide */
	_bool	m_isSlider = false;
	_float4	m_vUpRayDirection = {};				//레이방향 위아래 룩을얻어옴

private:	/* For.PullSwitch */
	_bool	m_isPullSwitch = false;
	_bool	m_isPullSwitchingEnd = false;
	_bool	m_isPullSwitchMove[SWITCHMOVE_END] = { false, };

private:	/*For.Bouncer*/
	_bool	m_IsBouncer[3] = { false, false, false };

private:	/*For.KitchenButton*/
	_bool	m_IsKitchenButton[3] = { false, false, false };

private:
	_uint	m_iCookingStage_PhaseNum = 0;

private:	/* For.Dead*/
	_bool	m_IsDead = false;

private:	/* For.KnockBack */
	_bool	m_IsKnockBack = false;
	//Quest
private:
	_int m_iCurrentQuestArr[3] = { -1,-1,-1 };
	_uint iSelectQuestArrNumber = 0;
private:/*For.RGBOffset*/
	_float3		m_vOffsetRGB = _float3(0.2f, 0.2f, 0.2f);
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_HeightOnNavigation();
	HRESULT SetUp_MyBoneMatrices(const char* pBoneName);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END