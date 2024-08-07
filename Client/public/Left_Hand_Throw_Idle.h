#pragma once

#include "Left_Hand_ParentAnim.h"


BEGIN(Client)

#define Throw_Idle_One 3
#define Throw_Idle_Two 5
#define Throw_Idle_Three 7

class CLeft_Hand_Throw_Idle final : public CLeft_Hand_ParentAnim
{
	DECLARE_SINGLETON(CLeft_Hand_Throw_Idle)
private:
	CLeft_Hand_Throw_Idle();
	virtual ~CLeft_Hand_Throw_Idle() = default;
public:
	virtual void HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CLeft_Hand_Anim * pLeftHandAnim) override;
private:
	void	First_MotionState_Cheking(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta);
private:
	_bool	m_bFirstCheck = false;
public:
	virtual void Free() override;


};

END