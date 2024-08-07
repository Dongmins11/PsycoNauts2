#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_BrainStom final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_BrainStom)
private:
	explicit CPlayer_BrainStom();
	virtual ~CPlayer_BrainStom() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Animation_BlendSpeedSet(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Create_Brain_Strom_Effect(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
private:
	CGameObject* m_pEffectObject = nullptr;
private:
	_bool		m_bFirstCheck = false;
	_bool		m_bKeyCheck = false;
	_bool		m_bChargeCheck = false;
	_bool		m_bEndCheck = false;
private:
	_double		m_lfTimeAcc = 0.0;
private:
	_uint		m_iRootPreIndex = 0;
private:
	_bool		m_bRootHoldCheck = false;
	_uint		m_iCount = 0;
public:
	virtual void Free() override;

};

END