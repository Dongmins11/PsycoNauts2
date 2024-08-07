#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)

#define Right_Throw_Finished 13

class CRight_Hand_Throw_Finished final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CRight_Hand_Throw_Finished)
private:
	CRight_Hand_Throw_Finished();
	virtual ~CRight_Hand_Throw_Finished() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;
private:
	void	First_MotionState_Cheking(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta);
private:
	_bool	m_bFirstCheck = false;
	_bool	m_IsDishGrab = false;
	_bool	m_bSoundCheck = false;
public:
	virtual void Free() override;


};

END