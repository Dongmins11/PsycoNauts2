#pragma once
#include "Client_Defines.h"
#include "PlayerTrigger.h"


BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CPhysXCollider;
END

BEGIN(Client)
class CPlayerTrigger_Body : public CPlayerTrigger
{
public:
	CPlayerTrigger_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayerTrigger_Body(const CPlayerTrigger_Body& rhs);
	virtual ~CPlayerTrigger_Body() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual void Set_Position(_fvector vPos = XMVectorZero());
	virtual void Set_Matrix(const _float4x4* pMatPtr);

public:
	const PlayerTriggerType Get_PlayerTriggerType()const { return m_eTriggerType; }
protected:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	//virtual void OnTriggerStay(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

	virtual void OnControllerCollisionHit(CCollisionObject& collision);

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;

	const _float4x4*		m_pPlayerTransform = nullptr;
#ifdef _DEBUG
	CCollider*				m_pRenderCollider = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
//	HRESULT SetUp_ConstantTable();

private:
	void	TriggerEnter_ForLoboEvent(CCollisionObject& collision);
	void    TriggerEnter_MapCurling(CCollisionObject& collision);
	void    TriggerEnter_MapDeleteCurling(CCollisionObject& collision);
	void	TriggerEnter_TutoUIMsg(CCollisionObject& collision);
	void	TriggerEnter_Stage0_End(CCollisionObject& collision);
	void    TriggerEnter_Stage0_NPCMOVE(CCollisionObject& collision);
	void	TriggerEnter_Stage1_Monster(CCollisionObject& collision);
	void	TriggerEnter_Stage2_Monster(CCollisionObject& collision);

private: /* Stage1 */
	void	TriggerEnter_Map1Curling(CCollisionObject& collision);
	void    TriggerEnter_Map1End(CCollisionObject& collision);
	void    TriggerEnter_Stage1_SKILL_InfoUI(CCollisionObject& collision);

private: /* Stage2 */
	void	TriggerEnter_MouseCutScene(CCollisionObject& collision);
	void	TriggerEnter_2DMAP(CCollisionObject& collision);
	void    TriggerEnter_Stage2_SKILL_InfoUI(CCollisionObject& collision);
	
private: /* Stage3 */
	void	TriggerEnter_BlenderPath(CCollisionObject& collision);
	void    TriggerExit_BlenderPath(CCollisionObject& collision);

	void    TriggerEnter_Stage3_Climbing(CCollisionObject& collision);
	void    TriggerExit_Climbing(CCollisionObject& collision);
	void    TriggerEnter_Stage3_ClimbingEND(CCollisionObject& collision);
	void    TriggerExit_Stage3_ClimbingEND(CCollisionObject& collision);

	void    TriggerEnter_Stage3_GroundSlide(CCollisionObject& collision);
	void    TriggerExit_Stage3_GroundSlide(CCollisionObject& collision);

	void	TriggerEnter_Stage3_Dead(CCollisionObject& collision);

private: /* Stage5 */
	void	TriggerEnter_Stage5_DishIsIn(CCollisionObject& collision);
	void	TriggerExit_Stage5_DishIsOut(CCollisionObject& collision);

private:
	/*For.LoboEvent*/
	void	LoboEvent3_Cinema();
	void	LoboEvent4_LookFix();
	void	LoboEvent5_FovyIncrease();
	void	LoboEvent6_FovyDecrease();

	/*For.Parkour*/
	void	Parkour6_FovyIncrease();

private: /* For.Player_Battle */
	void	Player_HitEntry(CCollisionObject & collision);
	void	Player_AttackExit();
	void	Player_AttackEntry(CCollisionObject& collision);

protected:
	// 플레이어용 트리거 타입Enum
	PlayerTriggerType		m_eTriggerType = PlayerTriggerType::PTTYPE_END;
	// 트리거 피봇위치
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iTutoUICnt = -1;
	_int					m_iCutSceneCnt = -1;
private:/*젠*/
	_bool					m_IsGenerateCensor = false;
	_bool					m_IsGenerateDoubt = false;

private: /* Skill Render 1회만*/
	_bool					m_Is2DMapEnter = false;
	_bool					m_IsSkillRBCheck = false;
	_bool					m_IsSkillRBUse = false;
	_bool					m_IsSkillECheck = false;
	_bool					m_IsSkillQCheck = false;

public:
	static CPlayerTrigger_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END