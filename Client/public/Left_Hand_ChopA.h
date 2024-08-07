#pragma once

#include "Left_Hand_ParentAnim.h"


BEGIN(Client)

#define Chop_A 0

class CLeft_Hand_ChopA final : public CLeft_Hand_ParentAnim
{
	DECLARE_SINGLETON(CLeft_Hand_ChopA)
private:
	CLeft_Hand_ChopA();
	virtual ~CLeft_Hand_ChopA() = default;
public:
	virtual void HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CLeft_Hand_Anim * pLeftHandAnim) override;
	
public:
	virtual void Free() override;
};

END