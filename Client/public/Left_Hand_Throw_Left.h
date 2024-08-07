#pragma once

#include "Left_Hand_ParentAnim.h"


BEGIN(Client)

#define Throw_Left_Throw_One 4
#define Throw_Left_Throw_Two 6
#define Throw_Left_Throw_Three 8

class CLeft_Hand_Throw_Left final : public CLeft_Hand_ParentAnim
{
	DECLARE_SINGLETON(CLeft_Hand_Throw_Left)
private:
	CLeft_Hand_Throw_Left();
	virtual ~CLeft_Hand_Throw_Left() = default;
public:
	virtual void HandleInput(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Update(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CLeft_Hand_Anim * pLeftHandAnim) override;
private:
	void	First_MotionState_Cheking(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta);
	void	Food_Create(CLeft_Hand_Anim * pLeftHandAnim, const CLeft_Hand_Anim::LEFTHANDDESC & pLeftHandDesc, _double TimeDelta);
private:
	_bool	m_bFirstCheck = false;
	_bool	m_bFoodCreateCheck = false;
private:
	_uint	m_iDishFoodCount = 0;
	_uint	m_iSoupFoodCount = 0;
	_uint	m_iCookieFoodCount = 0;

public:
	virtual void Free() override;


};

END