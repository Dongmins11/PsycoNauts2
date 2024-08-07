#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)

#define Chop_B 6

class CRight_Hand_ChopB final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CRight_Hand_ChopB)
private:
	CRight_Hand_ChopB();
	virtual ~CRight_Hand_ChopB() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;


public:
	virtual void Free() override;


};

END