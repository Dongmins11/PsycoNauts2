#include "stdafx.h"
#include "..\public\Left_Hand_ParentAnim.h"
#include "GameInstance.h"
#include "Player_LeftHand.h"
#include "Left_Hand_Anim.h"

CLeft_Hand_ParentAnim::CLeft_Hand_ParentAnim()
{
}

void CLeft_Hand_ParentAnim::SetUp_StateInfo(CPlayer_LeftHand * pLeftHand, CTransform * pTransformCom, CModel * pModelCom, CCollider * pColider, _uint iMyScene)
{
	m_pLeftHand = pLeftHand;
	m_pTransformCom = pTransformCom;
	m_pModelCom = pModelCom;
	m_pAABBCom = pColider;
	m_iCurScene = iMyScene;
}

void CLeft_Hand_ParentAnim::Free()
{
}
