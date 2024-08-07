#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
class CVIBuffer_Rect;
class CBoxCollider;
END

BEGIN(Client)

class CCloud final:public CGameObject
{
private:
	CCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCloud(const CCloud& rhs);
	virtual ~CCloud() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg,_uint iSceneIndex);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();


private:  /* 만들어질떄 불림 */
	void SettingMovePostion();

private: /* 이동 좌표 세팅 & 씬에따라 분기*/
	void SettingStage1(_uint iCount);
	//void SettingStage2(_uint iCount);
	void SettingStage3(_uint iCount);
	//void SettingStage4(_uint iCount);

private: /* 이동 시 씬에따라 분기*/
	void Move_And_Create(_double TimeDelta);

private: /* 각 씬에 따라 이동 및 객체 생성등 */
	void Stage1_MoveLerp(_double TimeDelta);

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
	void MoveRoation(_double TimeDelta); /* 회전돌아가게 설정 */
public:
	void	Set_PlayerCastCheck(_bool CastCheck) { m_bCastCheck = CastCheck; }
	_bool	Get_PlayerCastCheck() { return m_bCastCheck; }
	_vector Get_CloudPos();

private:
	CRenderer*				m_pRendererCom  = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CBoxCollider*			m_pColliderCom  =  nullptr;
	CTexture*				m_pTextureCom   = nullptr;
	CVIBuffer_Rect*			m_pVIBuffer = nullptr;

	_double					m_AccTime = 0.0;
	_bool					m_bMove = false;
	_bool					m_bNextCreate = false;
	_float3					m_vStartPostion;
	_float3					m_vEndPostion;

	_double					m_TimeAcc = 0.0;
	_float					m_fRatio = 0.f;
	_bool					m_MoveYCheck = true;

	_double					m_AccTimeYMove = 0.0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

private:
	_int					m_iNavigationIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;
	_uint					m_iPX_TYPE = 0;
private: /* 움직임에 필요 */
	_uint					m_iMyCount = 0; /* 현재의 나의 Count*/
	_bool					m_isMove = false;
	_bool					m_isColController = false;
	_bool					m_bCastCheck = false;
	_bool					m_bOnes = true;


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
	static CCloud* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;

};

END
