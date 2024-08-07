#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Fruits.h"

BEGIN(Client)
class CGoatHollis;
class CGoatFord;
class CGoatOtto;
class CBeater;
class CSpatula;
class CBoard_L;
class CBoard_R;
class CBoss_Goat final :public CGameObject
{
public:
	typedef struct tagBossGoatInfo
	{
		_float4x4		WorldMatrix;
		CGameObject*	pParent;
	}BOSSGOATINFO;

	enum BOSS
	{
		BOSS_H, BOSS_O, BOSS_F, BOSS_S, BOSS_B, BOSS_END
	};
private:
	enum REGENBOSS
	{
		RBOSS_HOLLIS, RBOSS_OTTO, RBOSS_END
	};
	enum SPAWNPOINT
	{
		POINT_L, POINT_C, POINT_R, POINT_END
	};
	enum PHASE
	{
		PHASE_1, PHASE_2, PHASE_3, PHASE_END
	};
	enum ATTACKPHASE
	{
		ATTPHASE_1, ATTPHASE_2, ATTPHASE_3, ATTPAHSE_END
	};
	enum WAVE
	{
		WAVE_1, WAVE_2, WAVE_END
	};
	enum PORTAL
	{
		PORTAL_L, PORTAL_R, PORTAL_END
	};

private:
	CBoss_Goat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBoss_Goat(const CBoss_Goat& rhs);
	virtual ~CBoss_Goat() = default;

public:
	virtual HRESULT NativeConstruct_Prototype()override;
	virtual HRESULT NativeConstruct(void* _pArg, _uint iCurrentScene)override;
	virtual _int Tick(_double TimeDelta)override;
	virtual _int LateTick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	void Lock_Boss(_bool*	_bArr);
	void Pattern_Lock();
	void Pattern_UnLock();
	CModel* Get_Model(BOSS eBoss);
	CTransform* Get_Transform(BOSS eBoss);
	CGameObject* Get_Bosses(BOSS eBoss);

public:
	void Damaged(_int _iDamage);

public:
	const _float4x4 Get_MouthMatrix(BOSS eBoss) const;
	const _float4x4 Get_BeaterMatrix() const;

public:
	CFruits::FRUITTYPE Get_FruitType();
	void Check_ClearWave();

private:
	void LinkSkill(_double TimeDelta);
	void Bosses_Tick(_double TimeDelta);
	void Bosses_LateTick(_double TimeDelta);
	void Bosses_Render();
	void Portal_Tick(_double TimeDelta);
	void Portal_LateTick(_double TimeDelta);
	void Update_Phase(_double TimeDelta);
	void Update_Wave(_double TimeDelta);
	void Generate_Fruits();
	void Generate_Monsters();
	void Reset_Phase();
	void Reset_Bosses();
	void Reset_AttackLoop();
	void Reset_State();
public:
	void Set_Ready(BOSS eBoss);
	void Taunt_Boss(BOSS eBoss, _bool bTaunt); //개별적인 도발컨트롤 true로
	void Taunt_Bosses(_bool _bTaunt); //전체 도발컨트롤 // 이놈을 호출해서 false 집어넣어주면됨 컷신후

	_bool IsSpawned();
private:
	CGameObject*			m_pBosses[BOSS::BOSS_END] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	CGameObject*			m_pPortal[PORTAL_END] = {nullptr, nullptr};
	CBoard_L*				m_pBoardL = nullptr;
	CBoard_R*				m_pBoardR = nullptr;

private:
	_double				m_dPhaseTime = 0.0;
	_double				m_dLinkSkillTime = 0.0;
	_bool				m_IsLinkSkillStart = false;
	_bool				m_IsRegeneration[RBOSS_END];
	_bool				m_IsLock[BOSS::BOSS_END];
	_bool				m_IsPatternLock = false;
	_bool				m_IsEnterPhase[PHASE_END] = { false, false, false };
	_bool				m_IsEndPhase[PHASE_END] = { false, false, false };
	_bool				m_IsSpawnDish[PHASE_END] = { false, false, false };
	_bool				m_IsAttPhase[ATTPAHSE_END] = { false, false, false };
	_bool				m_IsEnter = false;
	_bool				m_IsReadyPhase = false;
	_bool				m_IsGenerate = false;
	_bool				m_IsClearWave[WAVE_END] = {false, false};
	_float4x4			m_SpawnPointMatrix[POINT_END];
	_uint				m_iCount = 0;
	_int				m_iHp = 100;


	CFruits::FRUITTYPE  m_eFruitType = CFruits::FRUITTYPE::TYPE_END;
public:
	static CBoss_Goat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END