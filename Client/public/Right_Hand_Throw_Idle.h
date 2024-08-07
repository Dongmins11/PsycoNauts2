#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)

#define Right_Throw_Idle_One 7
#define Right_Throw_Idle_Two 9
#define Right_Throw_Idle_Three 11

class CRight_Hand_Throw_Idle final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CRight_Hand_Throw_Idle)
private:
	CRight_Hand_Throw_Idle();
	virtual ~CRight_Hand_Throw_Idle() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;
private:
	void	First_MotionState_Cheking(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta);
private:
	_bool	m_bFirstCheck = false;
public:
	virtual void Free() override;


};

END