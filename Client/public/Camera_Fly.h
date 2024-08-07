#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Fly final : public CCamera
{
public:/*For.Enum*/
	enum CAMERATYPE {
		CAMERATYPE_INDEPENDENT, CAMERATYPE_DEPENDENT, CAMERATYPE_TEST, CAMERATYPE_LOBOTO_TALK, CAMERTYPE_LOOKFIX,
		CAMERTYPE_PARKOUR, CAMERATYPE_PARKOUR_END, CAMERATYPE_STAGE0_END, CAMERATYPE_PLAYER_DEAD, CAMERATYPE_JUDGE, CAMERATYPE_JUDGE_DEAD,
		CAMERTYPE_HAROLDMOUSE, CAMERTYPE_CLOUD, CAMERTYPE_STAGE2_2D,
		CAMERATYPE_BOSSDISH_THROW, CAMERTYPE_BOSSDISH_POSE, CAMERTYPE_BOSSDISH_HIT, CAMERTYPE_BOSSINTRO, CAMERTYPE_STAGE3, CAMERATYPE_MONSTER,
		CAMERATYPE_END
	};

	enum CAMERAINFO { CAMAERA_RIGHT, CAMAERA_UP, CAMAERA_LOOK, CAMAERA_POSTION, CAMAERA_END };

	/*For.Enum_Loboto_Talk*/
	enum LOBOTOTALK { LOBOTOTALK_START, LOBOTOTALK_TALK, LOBOTOTALK_RUN, LOBOTOTALK_FINISH, LOBOTOTALK_END };
	enum JUDGE { JUDGE_START, JUDGE_CAMERA_1, JUDGE_CAMERA_2, JUDGE_CAMERA_3, JUDGE_END };
	enum JUDGEDEAD { JUDGEDEAD_START, JUDGEDEAD_1, JUDGEDEAD_2, JUDGEDEAD_3, JUDGEDEAD_4, JUDGEDEAD_5, JUDGEDEAD_END };
	enum HAROLDMOUSE { HAROLDMOUSE_START, HAROLDMOUSE_CAM1, HAROLDMOUSE_CAM2, HAROLDMOUSE_CAM3, HAROLDMOUSE_CAM4, HAROLDMOUSE_CAM5, HAROLDMOUSE_END };
	enum STAGE0_END { STAGE0_END_LOOK_DOOR, STAGE0_END_PUSHBACK, STAGE0_END_PUNCH, STAGE0_END_PUNCHTWO, STAGE0_END_FINISH, STAGE0_END_END };
	enum CLOUD_SCENE { CLOUD_START, CLOUD_1, CLOUD_2, CLOUD_3, CLOUD_4, CLOUD_5, CLOUD_6, CLOUD_7, CLOUD_8, CLOUD_9, CLOUD_10, CLOUD_11, CLOUD_12, CLOUD_13, CLOUD_14, CLOUD_15, CLOUD_16, CLOUD_17, CLOUD_18, CLOUD_END };

	enum BOSS_TYPE { BOSS_TYPE_OTTO, BOSS_TYPE_HOLLIS, BOSS_TYPE_GOAT, BOSS_TYPE_END };

	enum BOSS_INTRO { BOSS_INTRO_START, BOSS_INTRO_LOOP, BOSS_INTRO_END };

	enum BOSS_OUTRO { BOSS_OUTRO_START, BOSS_OUTRO_END };

	enum BOSS_DISH_THROW {
		BOSS_DISH_THROW_CHECKING, BOSS_DISH_THROW_DISH, BOSS_DISH_THROW_SOUP, BOSS_DISH_THROW_COOKIE, BOSS_DISH_THOWCHECKING,

		BOSS_DISH_THROW_OTTO, BOSS_DISH_THROW_HOLLIES, BOSS_DISH_THROW_GOAT, BOSS_DISH_THROW_END
	};
	enum BOSS_DISH_POS { BOSS_DISH_POS_START, BOSS_DISH_POS_CAM1, BOSS_DISH_POS_END };

	enum BOSS_DISH_HIT {
		BOSS_DISH_HIT_START, BOSS_DISH_HIT_OTTO, BOSS_DISH_HIT_HOLLIES,
		BOSS_DISH_HIT_OTTOTWO, BOSS_DISH_HIT_HOLLIESTWO,
		BOSS_DISH_HIT_OTTOTHREE, BOSS_DISH_HIT_HOLLIESTHREE, BOSS_DISH_HIT_END, BOSS_DISH_HIT_REALEND
	};
	enum MAP_2D { MAP_2D_START, MAP_2D_1, MAP_2D_2, MAP_2D_3, MAP_2D_4, MAP_2D_5, MAP_2D_6, MAP_2D_7, MAP_2D_END };

	enum STAGE3_CUT {STAGE3_CUT_START, STAGE3_CUT_1, STAGE3_CUT_2, STAGE3_CUT_3, STAGE3_CUT_4, STAGE3_CUT_5, STAGE3_CUT_6, STAGE3_CUT_7, STAGE3_CUT_8, STAGE3_CUT_9, STAGE3_CUT_10,
					 STAGE3_CUT_11, STAGE3_CUT_12, STAGE3_CUT_13, STAGE3_CUT_14, STAGE3_CUT_15, STAGE3_CUT_16, STAGE3_CUT_17, STAGE3_CUT_18, STAGE3_CUT_END};
	enum STATICMONSTER { MONSTER_CENSOR, MONSTER_CENSORB, MONSTER_BADIDEA, MONSTER_DENTURE, MONSTER_REGRET, MONSTER_END };

protected:
	explicit CCamera_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Fly(const CCamera_Fly& rhs);
	virtual ~CCamera_Fly() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg, _uint iSceneIndex) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:/*For.Get*/
	_vector Get_Camera_TransformState(CTransform::STATE eState) { return m_pTransform->Get_State(eState); }
	_float4x4 Get_LightView(_float3 vLightDir, _float3 vLightPos);
	_matrix	Get_View();
	_matrix	Get_ReflectionView(_float fHeight);
	_matrix		Get_WorldMatrixCam() { return m_pTransform->Get_WorldMatrix(); }
	const _float4x4* Get_WorldMatrixCamPtr() { return m_pTransform->Get_WorldMatrixPtr(); }
	const _bool	Get_IsInterverZCheck() const { return m_IsLerpZCheck; }
public:/*For.Set*/
	void Set_CameraType(CAMERATYPE eCameraType);
	void Set_Camera_TransformState(CTransform::STATE eState, _fvector vector) { m_pTransform->Set_State(eState, vector); }
	void Set_Camera_TransformState(CTransform::STATE eState, _float3 vector) { m_pTransform->Set_State(eState, XMLoadFloat3(&vector)); }
	void Set_Camera_Transform_Ptr(CAMERATYPE eCameraType, const _float4x4* pWorldMatrix);
	void Set_DependentType_Value(_float4 vValue, _float fIntervalZ, _double dLerpSpeed = 1.0);
	void Set_DependentType_Value_Default(_double dLerpSpeed = 1.0);
	void Set_Fovy(_float fAngle) { m_CameraDesc.fFovy = XMConvertToRadians(fAngle); }
	void Set_Fovy(_float fAngle, _double dRatioSpeed, _float dBackUpFovy = 0.f, _bool IsShaking = false, _bool IsRecovery = false);
	void Set_Signal_Ledge() { m_bParkourSignal_Ledge = true; }
	void Set_Signal_Exit_Up() { m_bParkourSignal_Exit_Up = true; }
	void Set_Signal_Exit_Down() { m_bParkourSignal_Exit_Down = true; }
	void Set_Camera_Shaking(_bool bCheck, _double dMaxTime, _bool bIsFix = true);
	void Set_IsFixY(_bool bCheck) { m_bIsFixY = bCheck; }
	void Set_DownDistance(_float fDownDistance) { m_vDependentValue.w = fDownDistance; }
	void IsLook_Fix(_bool bCheck, _float3 vLookFixDir, _bool bIsLimitTime = false, _double dLimitTime = 0.0);
	void Set_Stage0_End_State(STAGE0_END eState) { m_eStage0_End = eState, m_bIsLerp = true, XMStoreFloat3(&m_vSourPos, m_pTransform->Get_Position()); }
	void Set_Bias(_float fBias) { m_fBias = fBias; }
	void Set_CollisionShaking(_bool isShaking) { m_isShaking = isShaking; }
	void Set_Cloud_CAM(CLOUD_SCENE eState) { m_CloudScene = eState; m_OnesCheck = true; }
	void Set_Stage3_CUT(STAGE3_CUT eState) { m_Stage3Cut_Scene = eState; m_Stage3_OnesCheck = true; }

	void Set_Bias(_float fBias, _float fBias_Down) { m_fBias = fBias, m_fBias_Down = fBias_Down; }

	void Set_InterverZ(_bool bCheck, _float fValueZ, _double lfLerpSpeed);
	void Set_DishCheckType_On() { m_eDishThrow = BOSS_DISH_THOWCHECKING; }
	void Set_DishEndType_On() { m_eDishThrow = BOSS_DISH_THROW_END; }

	void Set_Far(_float fFar) { m_CameraDesc.fFar = fFar; }
	void Set_FixAdjustPos(_float2 vPos) { m_vFixAdjustPos = vPos; }
	void Set_MouseMoveSpeed(_float2 vSpeed) { m_vMouseMoveSpeed = vSpeed; }
	void Set_MouseMove(_bool bCheck) { m_bMouseMove = bCheck; }
	void Set_MouseFix(_bool bCheck) { m_bIsMouseFix = bCheck; }
	void Set_Stage3UI_Render_Check(_bool bCheck);

private:
	HRESULT SetUp_Component();

	void Camera_Collide();

	void Bone_PositionGet(const char * pBossBoneName, _float4 * pGetPosition, _bool bResetOn);
private:
	void Update_Dependent(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval);
	void Test_Update_Dependent(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval);
	void Update_Independent(_double TimeDelta);
	void Update_Loboto_Talk(_double TimeDelta);
	void Update_Judge_Spawn(_double TimeDelta);
	void Update_Judge_Dead(_double TimeDelta);
	void Update_HarOldMouse(_double TimeDelta);
	void Update_Parkour(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval);
	void Update_ParkourEnd(_double TimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval);
	void Update_Stage0_End(_double TimeDelta);
	void Update_Player_Dead(_double TimeDelta);
	void Update_Cloud_CutScene(_double TimeDelta);
	void Update_Boss_Throw_Dish(_double TimeDelta);
	void Update_Boss_Dish(_double TimeDelta);
	void Update_Boss_Hit(_double TimeDelta);
	void Update_Boss_Intro(_double TimeDelta);
	void Update_Stage2_2D(_double TimeDelta);
	void Lerp_Values(_double TimeDelta);
	void Update_Stage3_CUT_SCENE(_double TimeDelta);
	void Update_StaticMonsterView(_double TimeDelta);

	/*For.Type_Dependent*/
	_vector Compute_Pos_Dependent(_double dTimeDelta, _float fMaxDot, _float fMinDot, _float fUpDis, _float fDownDis, _float4 vInterval, _float4 vPlayerPos);
	_bool	IsMoveY(_long lMouseMove);
	void	Lerp_Once(_double TimaDelta, _vector vSourPos, _vector vDestPos, _vector vSourLook, _vector vDestLook, _float3* pOut_Look, _float3* pOut_Pos);
	void	Lerp_Once(_double TimaDelta, _vector vSourPos, _vector vDestPos, _float3* pOut_Pos);
	void	Lerp_DependentValue(_double TimaDelta);
	void	Lerp_InterverZ(_double TimeDelta);
	void	Set_Default_Value();
	_float4	Get_StandardPos();
	/*For.Fovy*/
	void	Lerp_Fovy(_double TimaDelta);

	/*For. Shake*/
	void	Shaking(_double TimeDelta, _bool bIsFix = true, _float fPower = 0.01f);

public: /* For.Test*/
	void	Set_HarOldMouse(HAROLDMOUSE eMouseState) { m_eHarOldMouse = eMouseState; }
	void	Set_StaticMonsterType(STATICMONSTER eMonsterType);

public:
	_bool	IsDefault();

private:
	CAMERATYPE					m_eCameraType = CAMERATYPE_END;

	_bool						m_bLimitUp = false;
	_bool						m_bLimitDown = false;
	_bool						m_bIsLerp = true;
	_bool						m_bMouseMove = true;
	_bool						m_bIsFar = false;
	_bool						m_bIsStuck = false;
	_float2						m_RotationSpeed;
	_double						m_LerpSpeed = 0.0;
	_double						m_Ratio = 0.0;
	_double						m_dTimeAcc = 0.0;

	_float3						m_vBackUpLook;
	_float3						m_vBackUpDestLook;
	const _float4x4*			m_pTargetObjectMatrix = nullptr;
	_float						m_fLength = 0.f;

	/*For.Type_Dependent*/
	_float4						m_vDependentValue;
	_float						m_fDependentIntervalZ = 2.f;
	_bool						m_bIsFixY = false;
	_float						m_fBackUpPosY = 0.f;
	_float4						m_vDestDependentValue = _float4(0.f, 0.f, 0.f, 0.f);
	_float						m_fDestDependentIntervalZ = 0.f;
	_bool						m_bLerpOnce = false;
	_double						m_dDependentTimeAcc = 0.0;
	_double						m_dDependentLerpSpeed = 1.0;

	/*For. Fovy*/
	_float						m_fSourFovy = 0.f;
	_float						m_fDestFovy = 0.f;
	_double						m_dFovyLerpSpeed = 0.0;
	_double						m_dFovyRatio = 0.0;
	_bool						m_bIsChangeFovy = false;
	_bool						m_bIsFovyReverse = false;
	_float						m_fBackUpFovy = 0.f;
	/*For. LookFix */
	_bool						m_bLookFix = false;
	_float3						m_vLookFixDir;
	_bool						m_bIsLimitTime = false;
	_double						m_dLimitTime = 0.0;
	_double						m_dLookFixTimeAcc = 0.0f;

	/*For. LobotoTalk*/
	LOBOTOTALK					m_eLobotoTalk = LOBOTOTALK_END;
	_double						m_dLobotoTimeAcc = 0.0;
	const _double				m_dLobotoRunFinishTime = 3.f;
	_float3						m_vLobotoDestPos;

	/*For. Judge*/
	JUDGE						m_eJudge = JUDGE_END;

	/*For. Judge*/
	JUDGEDEAD					m_eJudgeDead = JUDGEDEAD_END;

	/*For. StaticMonsterView */
	STATICMONSTER				m_eStaticMonster = MONSTER_END;
	_bool						m_IsSpawned = false;


	/*For. HarOldMouse*/
	HAROLDMOUSE					m_eHarOldMouse = HAROLDMOUSE_END;
	_bool						m_bModelLerpCheck = false;

	/*For. Stage1 Cloud CutScene */
	CLOUD_SCENE					m_CloudScene = CLOUD_END;
	_float3						m_CameraINFO[CAMAERA_END];
	_float3						m_CameraCurrentINFO[CAMAERA_END];
	_bool						m_OnesCheck = true;

	/*For. Stage3 CutScene */
	STAGE3_CUT					m_Stage3Cut_Scene = STAGE3_CUT_END;
	_float3						m_Stage3_CameraINFO[CAMAERA_END];
	_float3						m_Stage3_CameraCurrentINFO[CAMAERA_END];
	_bool						m_Stage3_OnesCheck = true;


	/*For. Parkour*/
	_bool						m_bParkourSignal_Ledge = false;
	_bool						m_bParkourSignal_Exit_Up = false;
	_bool						m_bParkourSignal_Exit_Down = false;

	/*For. ParkourEnd*/
	_float						m_fParkourRatio = 0.f;
	_bool						m_bParkourEnd = false;
	_float						m_fDownDis = 0.f;

	/*For. Shaking*/
	_bool						m_bIsShaking = false;
	_bool						m_bIsFixShake = true;
	_double						m_dShakingTimeAcc = 0.0;
	_double						m_dShakingTimeMax = 0.0;
	_float3						m_vBackUpShakePos;
	_float3						m_vBackUpShakeLook;

	/*For. Mouse Fix*/
	_bool						m_bIsMouseFix = false;

	/*For. Stage0_End*/
	STAGE0_END					m_eStage0_End = STAGE0_END_END;
	_float3						m_vSourPos;
	_float3						m_vDestPos;
	_double						m_dStage0_End_TimeAcc = 0.0;

	/*For. Collide*/
	_float4x4					m_vBackUpWorld;
	_float4x4					m_vBackUpWorld2;
	_float4x4					m_vBackUpWorld3;
	_bool						m_bIsCollide = false;
	_uint						m_iCountt = 0;
	_bool						m_bIsStay = true;
	_float3						m_vBackUpPos;
	_float						m_fBias = 1.f;
	_float						m_fBias_Down = 1.f;
	_bool						m_bIsPlayerAlphaChange = false;

	/* For. Shaking */
	_bool						m_isShaking = false;
	_bool						m_isCollisionShaking = false;

	/*For. FixPos*/
	_float2						m_vFixAdjustPos = _float2(50.f, -100.f);
	_float2						m_vMouseMoveSpeed = _float2(0.16f, 0.16f);


private: /*For. Boss_Dish*/
	BOSS_TYPE					m_eBossType = BOSS_TYPE_END;
	BOSS_DISH_THROW				m_eDishThrow = BOSS_DISH_THROW_END;
	BOSS_DISH_POS				m_eBossDish = BOSS_DISH_POS_END;
	BOSS_DISH_HIT				m_eBossHit = BOSS_DISH_HIT_END;
	BOSS_INTRO					m_eBossIntro = BOSS_INTRO_END;
	_bool						m_IsLerpZCheck = false;
	_bool						m_IsFirstCheck = false;
	_bool						m_IsFirstPositionCheck = false;
	_bool						m_IsFoveOneCheck = false;
	_bool						m_IsBossLerpCheck = false;
	_bool						m_IsFovyFirstCheck = false;
	_bool						m_bBackUpLerpCheck = false;
	_bool						m_bBoneFindCheck = false;
	_bool						m_bIntroCheck[2] = { false, };
	_float						m_fLerpInterverZ = 0.f;
	_float						m_fLerpInterverStartZ = 0.f;
	_double						m_lfZInterverSpeed = 0.0;
	_double						m_lfRatioZ = 0.0;
	_double						m_lfBossLerpTimeAcc = 0.0;
	_double						m_lfBossEndTimeAcc = 0.0;
	_double						m_lfBackUpRatio = 0.0;
	_double						m_lfBossIntroLoopTimeAcc = 0.0;
	_float4						m_vBossBackUpPosition = {};
	_float4						m_vBossBackUpLook = {};
	_bool						m_bBossSoundChecking = false;
	/* For.Cam Bone */
	_bool						m_IsBoneFinderCheck = false;
	_float4x4					m_TargetOffsetMatrix = {};
	_float4x4					m_TargetPivotMatrix = {};
	const _float4x4*			m_pTargetWorldMatrix = nullptr;
	const _float4x4*			m_pTargetCombinedMatrix = nullptr;

	/* For.2D MAP*/
	MAP_2D						m_eMap_2D = MAP_2D_END;

private:
	CSphereCollider*			    m_pColliderCom = nullptr;
public:
	static CCamera_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END