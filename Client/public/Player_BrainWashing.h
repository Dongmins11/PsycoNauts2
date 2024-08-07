#pragma once

#include "Player_ParentAnim.h"

BEGIN(Client)

class CPlayer_BrainWashing final : public CPlayer_ParentAnim
{
	DECLARE_SINGLETON(CPlayer_BrainWashing)
private:
	explicit CPlayer_BrainWashing();
	virtual ~CPlayer_BrainWashing() = default;
public:
	virtual void HandleInput(CPlayer_AnimState * pPlayer_AnimState, CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Update(CPlayer_AnimState * pPlayer_AnimState, const CPlayer_AnimState::ANIMDESC & pAnimKeyDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CPlayer_AnimState * pPlayer_AnimState) override;
	virtual void Dead_Reset(CPlayer_AnimState * pPlayer_AnimState) override;
private:
	void	Animation_BlendSpeedSet(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	Create_Brain_Washing_Effect(CPlayer_AnimState * pPlayer_AnimState, _double TimeDelta);
	void	BrainWashing();
	void	Lay_Cast_BrainWashing_Check();
	void	LayObject_Rlease();
private:
	CGameObject*	m_pRayObject = nullptr;
	_bool			m_bFirstCheck = false;
	_bool			m_bKeyCheck = false;
	_bool			m_bEffectCreate = false;
private:
	_uint			m_iRootPreIndex = 0;
public:
	virtual void Free() override;

};

END