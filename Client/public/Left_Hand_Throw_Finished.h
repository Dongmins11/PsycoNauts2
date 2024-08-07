#pragma once

#include "Left_Hand_ParentAnim.h"


BEGIN(Client)

#define Left_Throw_Finished 9

class CLeft_Hand_Throw_Finished final : public CLeft_Hand_ParentAnim
{
	DECLARE_SINGLETON(CLeft_Hand_Throw_Finished)
private:
	CLeft_Hand_Throw_Finished();
	virtual ~CLeft_Hand_Throw_Finished() = default;
public:
	virtual void HandleInput(CLeft_Hand_Anim* pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CLeft_Hand_Anim * pLeftHandAnim) override;
private:
	void	First_MotionState_Cheking(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta);
private:
	_bool	m_bFirstCheck = false;
	_bool	m_IsDishGrab = false;
	_bool	m_bSoundCheck = false;
public:
	virtual void Free() override;


};

END