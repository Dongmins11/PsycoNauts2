#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)
class CRight_Hand_Default final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CRight_Hand_Default)
private:
	CRight_Hand_Default();
	virtual ~CRight_Hand_Default() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;

public:
	virtual void Free() override;


};

END