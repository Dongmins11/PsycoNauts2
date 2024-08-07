#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)

#define Grab 22
#define Toss 24

class CRight_Hand_Grap final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CRight_Hand_Grap)
private:
	CRight_Hand_Grap();
	virtual ~CRight_Hand_Grap() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;
private:
	void	Lerp_Hand(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta);
private:
	_bool		m_bGrabFinishied = false;

public:
	virtual void Free() override;


};

END