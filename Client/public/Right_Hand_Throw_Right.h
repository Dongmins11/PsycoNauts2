#pragma once

#include "Right_Parent_Anim.h"


BEGIN(Client)

#define Throw_Right_Throw_One 8
#define Throw_Right_Throw_Two 10
#define Throw_Right_Throw_Three 12

class CRight_Hand_Throw_Right final : public CRight_Parent_Anim
{
	DECLARE_SINGLETON(CRight_Hand_Throw_Right)
private:
	CRight_Hand_Throw_Right();
	virtual ~CRight_Hand_Throw_Right() = default;
public:
	virtual void HandleInput(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Update(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Reset_Anim(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta) override;
	virtual void Set_Colider(CRight_Hand_Anim * pRightHandAnim) override;
private:
	void	First_MotionState_Cheking(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta);
	void	Food_Create(CRight_Hand_Anim * pRightHandAnim, const CRight_Hand_Anim::RIGHTHANDDESC & pRightHandDesc, _double TimeDelta);
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