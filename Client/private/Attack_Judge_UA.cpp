#include "stdafx.h"
#include "..\public\Attack_Judge_UA.h"
#include "Judge_Book.h"

CAttack_Judge_UA::CAttack_Judge_UA(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CAttack_Judge_UA::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pJudge->Get_Hp())
		return true;

	m_fSpeed = 0.f;

	if (55 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(55) ||
		(52 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(52)))
	{
		m_IsDelayAttack = true;
		m_IsFarAttack = false;
		m_IsNearAttack = false;
		m_IsAttackLoop = false;
		m_IsAttackReady = false;
		m_IsAttack = false;
		m_IsRecovery = false;
		m_pJudge->Set_AttackLoop(false);
		_pModel->Set_IsAnimLefp(true);
		_pModel->Set_Animation(60);
		return true;
	}
	if (20.f > m_pJudge->Get_Distance() &&
		!m_IsAttackLoop &&
		!m_IsDelayAttack &&
		(340.f <= m_pJudge->Get_Degree() ||
			20.f >= m_pJudge->Get_Degree()))
		m_IsAttackLoop = true;

	if (m_IsDelayAttack)
	{
		m_dDelayTime += TimeDelta;
		if (4.0 < m_dDelayTime ||
			1.5f > m_pJudge->Get_Distance())
		{
			m_IsDelayAttack = false;
			m_dDelayTime = 0.0;
		}
	}
	else if (m_IsAttackLoop &&
		!m_IsAttackReady)
	{
		m_pJudge->Set_AttackLoop(true);
		if (5.f > m_pJudge->Get_Distance())
		{
			m_IsFarAttack = false;
			m_IsNearAttack = true;
		}
		else
		{
			m_IsFarAttack = true;
			m_IsNearAttack = false;
		}
	}
	if (!m_IsAttackLoop ||
		m_IsDelayAttack)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야 시야각 10도 안에 들어와있어야 true
	{
		return true;
	}

	else
	{
		if (m_IsAttackLoop)
		{
			if (m_IsFarAttack)
			{
				if (55 != _pModel->Get_AnimIndex() &&
					56 != _pModel->Get_AnimIndex() &&
					57 != _pModel->Get_AnimIndex() &&
					!m_IsAttackReady)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(57);
					m_IsAttackReady = true;
					if (nullptr == m_pBook)
					{
						CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
						_uint iRandMeshType = 0;
						CJudge_Book::WEAPONINFO tagWeaponInfo;
						ZeroMemory(&tagWeaponInfo, sizeof(CJudge_Book::WEAPONINFO));
						tagWeaponInfo.iWeaponType = iRandMeshType;
						tagWeaponInfo.pParentMatrix = m_pJudge->Get_PropHandMatrixPtr();
						pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeBook"), m_pJudge->Get_CurrentScene(), TEXT("Layer_JudgeBook"), &m_pBook, &tagWeaponInfo);
						RELEASE_INSTANCE(CGameInstance);
					}
				}
				else if ((57 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(57)) &&
					!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(56);
					m_IsAttack = true;
				}
				else if ((56 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(56)) &&
					!m_IsRecovery)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(55);
					static_cast<CJudge_Book*>(m_pBook)->Set_Drop(true);
					m_pBook = nullptr;
					m_IsRecovery = true;
				}
			}
			else if (m_IsNearAttack)
			{
				if (52 != _pModel->Get_AnimIndex() &&
					53 != _pModel->Get_AnimIndex() &&
					54 != _pModel->Get_AnimIndex() &&
					!m_IsAttackReady)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(54);
					m_IsAttackReady = true;
				}
				else if ((54 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(54)) &&
					!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(53);
					m_IsAttack = true;

					//여기도 제자리 공격 토네이도랑 책 늘여놓는 이펙트 넣으면 될듯
					m_pJudge->Attack(CJudge::ARMEDTYPE::UNARMED, true);

					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperRect"),
						SCENE_STATIC, TEXT("Effect_JudgePaperRect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgePaperRect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperRect"), TEXT("Com_Option"), true);

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeDonut"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeDonut"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;						
					}

					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgeDonut"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeDonut"), TEXT("Com_Option"), true);
					RELEASE_INSTANCE(CGameInstance);
				}
				else if ((53 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(53)) &&
					!m_IsRecovery)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(52);
					m_IsRecovery = true;
				}
			}
		}

		if (55 == _pModel->Get_AnimIndex())
		{
			if (0 == _pModel->Get_AnimationCurrentKeyFrame())
				m_fSpeed = 2.45F;
			else if (1 == _pModel->Get_AnimationCurrentKeyFrame())
				m_fSpeed = 2.45F;
			else
				m_fSpeed = 0.0F;
		}
		else if (56 == _pModel->Get_AnimIndex())
		{
			if (18 <= _pModel->Get_AnimationCurrentKeyFrame())
				m_fSpeed = 2.F;
			else
				m_fSpeed = 0.F;
		}

		_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*m_fSpeed);

		return false;
	}
}

void CAttack_Judge_UA::Reset_AttackState()
{
	m_IsDelayAttack = false;
	m_IsFarAttack = false;
	m_IsNearAttack = false;
	m_IsAttackLoop = false;
	m_IsAttackReady = false;
	m_IsAttack = false;
	m_IsRecovery = false;
	m_dDelayTime = 0.0;
}

CAttack_Judge_UA * CAttack_Judge_UA::Create(CJudge * _pJudge)
{
	CAttack_Judge_UA* pInstance = new CAttack_Judge_UA(_pJudge);

	return pInstance;
}

void CAttack_Judge_UA::Free()
{
}
