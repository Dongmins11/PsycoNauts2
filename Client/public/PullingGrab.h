#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)

#define Grab_Fulling 25
#define Grab_FullingLoop 26
#define Greb_Fuliing_End 27

class CPullingGrab final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CPullingGrab)
private:
	CPullingGrab();
	virtual ~CPullingGrab() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;
private:
	_bool		m_bGrabFinishied = false;

public:
	virtual void Free() override;


};

END