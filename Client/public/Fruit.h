#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "GameInstance.h"

BEGIN(Client)

class CFruit abstract :public CGameObject
{
protected:
	enum FruitModelType { FMT_IDLE = 0, FMT_BOILED = 1, FMT_CUTTING = 2, FMT_BLENDED = 3, FMT_GRIDDLED = 4, FMT_END };

	explicit CFruit(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFruit(const CFruit& rhs);
	virtual ~CFruit() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_FruitType(FruitType eType, _uint SpwnerIndex) {m_eFruitType = eType, m_SpwnerIndex = SpwnerIndex;}
	FruitType Get_FruitType() { return m_eFruitType; }

	void Add_CookingNumber(_uint iNum) { m_iCookingNumber += iNum; }
	void Set_CookingNumber(_uint iNum) { m_iCookingNumber = iNum; }
	_uint Get_CookingNumber() { return m_iCookingNumber; }

	void Set_PlatePosition(_float3 vPos) { m_vPlatePosition = vPos; m_isPlating = true; }
	void Set_CookWarePosition(_float3 vPos) { m_vCookWarePosition = vPos; }
	void Update_CookingTime(_double TimeAcc) { m_lfCookingTimeAcc = TimeAcc; }
	void Set_StartBlending();
	void Set_EndBaking();
	void Cooking_MeshSetting();
	void Set_CutSceneAnimation(_uint Index);

protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

protected:
	void Curling(_vector pTransform, _float Range, class CBoxCollider* pCollider = nullptr);
	void Add_Force(_fvector vDir, PxForceMode::Enum eMode);
	void Setting_Throwing(_fvector vDir);
	void Set_FruitToPlate();
	void Bone_MatrixCalculation(_uint iFruitType);

protected:
	HRESULT WorldMatrixSetting(_matrix WorldMatrix);
	HRESULT PhysColliderBox_Setting();
	HRESULT Update_Cooking(_double TimeDelta);

protected:	/* For.Bone */
	HRESULT SetUp_BoneMatrices(const char* pBoneName,_uint iModelIndex);

private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);
	/* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex, class CBoxCollider* pCollider);
	void Ready_Setting_Cooking(_uint iCookWareType);
	void Reset_Setting_Cooking();

	_bool Checking_FruitCooking(CookType eCookType, _uint* iUINumber);
	void Dead_And_SettingFrit();
public:
	_float Get_Camera_Distance();
	
protected:
	_uint					m_iCurlingIndex = 0;
	_uint					m_iDeleteCurlingIndex = 0;
	_bool					m_isMove = false;
	_bool					m_isColController = false;
	_float4x4				m_InitMatrix;
	_float3					m_vPlatePosition = { 0.f,0.f,0.f };
	_float3					m_vCookWarePosition = { 0.f,0.f,0.f };
	_float3					m_vCookWareHitPosition = { 0.f,0.f,0.f };
	_float3					m_vBlenderFinishPosition = { 0.f,0.f,0.f };
	_float3					m_vCookWareCatmullRomEndPosition = { 0.f,0.f,0.f };


	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
	FruitType				m_eFruitType = FruitType::FRUIT_END;

	wstring				m_StrModelName[FMT_END] = { TEXT(""),TEXT(""),TEXT(""),TEXT(""),TEXT("") };
	CModel*				m_pModelCom[FMT_END] = { nullptr,nullptr,nullptr,nullptr,nullptr };
	_tchar*				m_szModelLayerTags[FMT_END] = { TEXT("Com_Model0") ,TEXT("Com_Model1") ,TEXT("Com_Model2") ,TEXT("Com_Model3") ,TEXT("Com_Model4") };
	// 현재 plating그릇에 올라가있는가?
	_bool					m_isPlating = false;
	// 현재 요리된 상태(덧셈으로표현)
	_uint					m_iCookingNumber = 0;

	// 요리를 할 준비가 되어있는가?
	_bool					m_isReadyCooking = false;
	// 요리가 시작하였는가?
	_bool					m_isStartCooking = false;
	// 요리가 끝났는가?
	_bool					m_isFinishCooking = false;
	// 현재 요리중인 시간
	_double					m_lfCookingTimeAcc = 0.0;
	// 현재 요리의 종료 시간
	_double					m_lfCookingTimeMax = 0.0;
	// 현재 요리중인 시간
	_double					m_lfLerpToCookWareTimeAcc = 0.0;
	_uint					m_iCookingUINumber = 0;
	class CCookingProgressBarUI*  pCookingUI = nullptr;
	// 현재 요리할(중인) 타입
	CookType				m_eCurrentCookingType = CookType::COOK_END;
	FruitModelType			m_eCurrentModelType = FMT_IDLE;


protected:
	_bool					m_bDead_Frit = false;
	_uint					m_SpwnerIndex = 0;
	_bool					m_bReset = false;
	_float4					m_BackUpPostion;
	_float4					m_BackUpLook;

	_double					m_Respawn = 0.0;


protected: /* Bone_Head 머리뼈위치잡기위한 매트릭스 */
	_float4x4				m_OffsetMatrix[FMT_END];
	_float4x4				m_PivotMatrix[FMT_END];
	const _float4x4*		m_pWorldMatrix = nullptr;
	const _float4x4*		m_pCombinedMatrix[FMT_END] = { nullptr,};

public:
	virtual void Free()override;
};

END