#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Fruit.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
class CBoxCollider;
END

BEGIN(Client)

class CBlender final :public CGameObject
{
private:
	CBlender(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBlender(const CBlender& rhs);
	virtual ~CBlender() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	class CFruit* Get_Blender_Fruit() { return m_pTargetFruit; }
	void  Set_Blender_Fruit(class CFruit* pFruit) { m_pTargetFruit = pFruit; }

public:
	void Set_Create_Effect(_bool isEffectCreate) { m_isEffectCreate = isEffectCreate; }

protected:
	virtual void Add_Force(_fvector vDir, PxForceMode::Enum eMode = PxForceMode::eFORCE);
	virtual void Setting_Throwing(_fvector vDir);
	virtual void OnCollisionEnter(CCollisionObject& collision) override;
	virtual void OnCollisionStay(CCollisionObject& collision) override;
	virtual void OnCollisionExit(CCollisionObject& collision) override;
	virtual void OnTriggerEnter(CCollisionObject& collision)override;
	virtual void SettingActorActive();

private: /* For.Curling */
	_bool Setting_CurlingIndex(_int* pCurlingIndex, _int* pDeleteIndex);

private: /* For.RenderBranch */
	void Stage_RenderBranch(_int iPlayerCurlingIndex);
	void HITCheck(_double TimeDelta); /* Hit 진행시 무적상태값 설정 */

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

	_bool					m_isColController = false;

	_bool					m_isMove = false;

	_bool					m_isEffectCreate = false;
	/* 움직임에 필요 */

private:
	CFruit*					m_pTargetFruit = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

	virtual void OnControllerCollisionHit(CCollisionObject& collision);
private:
	_int					m_iNavigationIndex = 0;
	//LayerType				m_eLayerType = LayerType::LAYERTYPE_STATIC;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;

private:
	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	_float					m_ArrKey[3];
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	/*For.NormalMapping*/
	_bool					m_bIsNormalMapping = true;

public:
	static CBlender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END