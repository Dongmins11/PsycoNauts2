#pragma once

#include "Left_Hand_ParentAnim.h"


BEGIN(Client)
class CLeft_Hand_Default final : public CLeft_Hand_ParentAnim
{
	DECLARE_SINGLETON(CLeft_Hand_Default)
private:
	CLeft_Hand_Default();
	virtual ~CLeft_Hand_Default() = default;
public:
	virtual void HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CLeft_Hand_Anim * pLeftHandAnim) override;

public:
	virtual void Free() override;


};

END