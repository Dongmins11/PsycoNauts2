#include "stdafx.h"
#include "..\public\Attack_Judge.h"

CAttack_Judge::CAttack_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CAttack_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pJudge->Get_Hp())
		return true;

	m_fSpeed = 0.f;
	if ((6 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(6)) ||
		(3 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(3)) ||
		(13 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(13)))
	{
		m_IsDelayAttack = true;
		m_IsAttackLoop = false;
		m_IsAttackReady = false;
		m_IsAttack = false;
		m_IsRecovery = false;
		m_pJudge->Set_AttackLoop(false);
		m_IsColliderOn = false;
		return true;
	}
	if (m_IsDelayAttack)
	{
		m_dDelayTime += TimeDelta;
		if (5.0 < m_dDelayTime ||
			3.f > m_pJudge->Get_Distance())
		{
			m_IsDelayAttack = false;
		}
	}
	if (!m_IsAttackLoop &&
		!m_IsDelayAttack &&
		(340.f <= m_pJudge->Get_Degree() ||
			20.f >= m_pJudge->Get_Degree()))
	{
		if (8.f >= m_pJudge->Get_Distance() &&
			6.f <= m_pJudge->Get_Distance())
		{
			m_pJudge->Set_AttackLoop(true);
			m_IsAttackLoop = true;
			m_iAttackType = rand() % 2;
		}

		else if (3.f >= m_pJudge->Get_Distance())
		{
			m_pJudge->Set_AttackLoop(true);
			m_IsAttackLoop = true;
			m_iAttackType = 2;
		}
	}

	if (!m_IsAttackLoop ||
		m_IsDelayAttack)		//10�̸��� �Ÿ��� �������� Idle���¸� ����ž� �þ߰� 10�� �ȿ� �����־�� true
	{
		return true;
	}

	else
	{
		if (m_IsAttackLoop)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			switch (m_iAttackType)
			{
			case 0:
				if (6 != _pModel->Get_AnimIndex() &&
					7 != _pModel->Get_AnimIndex() &&
					8 != _pModel->Get_AnimIndex() &&
					!m_IsAttackReady)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(8);
					m_IsAttackReady = true;
					//���η� ��������� �غ�ܰ� ����

				}
				else if ((8 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(8)) &&
					!m_IsAttack)
				{
					m_pJudge->Render_Trail(true);
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(7);
					//�������� ���� ���� �ִϸ��̼� ���۽���

					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeChargeComplete2"),
						SCENE_STATIC, TEXT("Effect_JudgeChargeComplete2"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}


					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgeChargeComplete2"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					_vector vPos = _pTransform->Get_State(CTransform::STATE_UP) * 3.4f - _pTransform->Get_State(CTransform::STATE_LOOK) * 1.5f + _pTransform->Get_State(CTransform::STATE_RIGHT) * 0.3f;
					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeChargeComplete2"), TEXT("Com_Option"), vPos);
	
					m_IsAttack = true;
				}
				else if ((7 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(7)) &&
					!m_IsRecovery)
				{
					//�������� ���� ���� �ִϸ��̼� ȸ�� �ִϸ��̼� ����
					m_pJudge->Render_Trail(false); //�̱����� Ʈ���ϺҺ��� ���� �Լ� ����� �����ʿ��ϸ� ������ּ�
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(6);
					m_IsRecovery = true;
				}
				break;
			case 1:
				if (13 != _pModel->Get_AnimIndex() &&
					14 != _pModel->Get_AnimIndex() &&
					15 != _pModel->Get_AnimIndex() &&
					!m_IsAttackReady)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(15);
					m_IsAttackReady = true;
					//���ΰ��� �غ�ܰ� �ִϸ��̼� ��������
				}
				else if ((15 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(15)) &&
					!m_IsAttack)
				{
					m_pJudge->Render_Trail(true);
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(14);
					m_IsAttack = true;
					//���ΰ��� �������� �ִϸ��̼� ��������
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeCharge2"),
						SCENE_STATIC, TEXT("Effect_JudgeCharge2"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}


					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgeCharge2"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					_vector vPos = _pTransform->Get_State(CTransform::STATE_UP) * 1.7f + _pTransform->Get_State(CTransform::STATE_LOOK) * 2.3f;
					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeCharge2"), TEXT("Com_Option"), vPos);
				}

				else if ((14 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(14)) &&
					!m_IsRecovery)
				{
					//���ΰ��� �������� ȸ�� �ִϸ��̼� ��������
					m_pJudge->Render_Trail(false);  //�̱����� Ʈ���ϺҺ��� ���� �Լ� ����� �����ʿ��ϸ� ������ּ�
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(13);
					m_IsRecovery = true;
				}
				break;
			case 2:
				if (3 != _pModel->Get_AnimIndex() &&
					4 != _pModel->Get_AnimIndex() &&
					5 != _pModel->Get_AnimIndex() &&
					!m_IsAttackReady)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(5);
					m_IsAttackReady = true;
					//���ڸ� ���� ������� �ִϸ��̼� �غ� �ִϸ��̼� ��������
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeHead"),
						SCENE_STATIC, TEXT("Effect_JudgeHead"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgeHead"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					
					_vector vPos = _pTransform->Get_State(CTransform::STATE_UP) * 2.3f + _pTransform->Get_State(CTransform::STATE_LOOK) * 2.f;
					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeHead"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeHead"), TEXT("Com_Option"), true);

				}
				else if ((5 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(5)) &&
					!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(4);
					m_IsAttack = true;
					//���ڸ� ���� ������� �ִϸ��̼� �ִϸ��̼� ��������
				}
				else if ((4 == _pModel->Get_AnimIndex() && _pModel->Get_IsAnimFinishied(4)) &&
					!m_IsRecovery)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(3);
					m_IsRecovery = true;
					//���ڸ� ���� ������� �ִϸ��̼� ȸ�� �ִϸ��̼� ��������

				}
				break;
			}

			if (5 == _pModel->Get_AnimIndex() && (21 <= _pModel->Get_AnimationCurrentFrame(5)))
				m_pJudge->SetOn_RotateWeapon(true);

			else if (3 == _pModel->Get_AnimIndex() && (28 <= _pModel->Get_AnimationCurrentFrame(3)&& 31>= _pModel->Get_AnimationCurrentFrame(3)))
			{
				m_pJudge->SetOn_RotateWeapon(false);
				m_pJudge->SetOn_Drop(true);
			}
			else if (3 == _pModel->Get_AnimIndex() && (36 <= _pModel->Get_AnimationCurrentFrame(3) && 38 >= _pModel->Get_AnimationCurrentFrame(3)))
			{
				m_pJudge->SetOn_RotateWeapon(false);
				m_pJudge->SetOn_Drop(false);
			}
			if (7 == _pModel->Get_AnimIndex())
			{
				switch (_pModel->Get_AnimationCurrentKeyFrame())
				{
				case 0:
					m_fSpeed = 0.F;
					break;
				case 1:
					m_fSpeed = 0.F;
					break;
				case 2:
					m_fSpeed = 0.F;
					break;
				case 3:
					m_fSpeed = 0.F;
					break;
				case 4:
					m_fSpeed = 1.25F;
					break;
				case 5:
					m_fSpeed = 1.25F;
					break;
				case 6:
					m_fSpeed = 1.25F;
					break;
				case 7:
					m_fSpeed = 1.25F;
					break;
				case 8:
					m_fSpeed = 2.5F;
					break;
				case 9:
					m_fSpeed = 2.5F;
					break;
				case 10:
					m_fSpeed = 2.5F;
					break;
				case 11:
					m_fSpeed = 2.5F;
					break;
				case 12:
					m_fSpeed = 2.5F;
					break;
				case 13:
					m_fSpeed = 2.5F;
					break;
				case 14:
					m_fSpeed = 2.5F;
					break;
				case 15:
					m_fSpeed = 2.5F;
					break;
				case 16:
					m_fSpeed = 2.5F;
					break;
				case 17:
					m_fSpeed = 2.5F;
					break;
				case 18:
					m_fSpeed = 2.5F;
					break;
				case 19:
					m_fSpeed = 2.5F;
					break;
				case 20:
					m_fSpeed = 0.F;
					break;
				case 21:
					m_fSpeed = 0.F;
					break;
				case 22:
					m_fSpeed = 0.F;
					break;
				case 23:
					m_fSpeed = 0.F;
					break;
				}
				if (19 <= _pModel->Get_AnimationCurrentKeyFrame() &&
					!m_IsColliderOn)
				{
					m_IsColliderOn = true;
					m_pJudge->Attack(CJudge::ARMEDTYPE::ARMED, true);
					//���⿡ ��ġ�� ���� ��⶧���� ���������� ���⳪ ��ƼŬ ó�� ��Ź�帳�ϴ�.
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bottom"),
						SCENE_STATIC, TEXT("Effect_Smoke_Bottom"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					/* ����Ʈ ��� -> ���� ���� �ٷ� ������� ��Ʈ�޴��������� */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, _pTransform, TEXT("Layer_Smoke_Bottom"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					_vector vPos = _pTransform->Get_State(CTransform::STATE_LOOK) * 6.2f - _pTransform->Get_State(CTransform::STATE_RIGHT) * 0.1f + _pTransform->Get_State(CTransform::STATE_UP) * 0.2f;
					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bottom"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bottom"), TEXT("Com_Option"), 0.5f);


					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"),
						SCENE_STATIC, TEXT("Effect_JudgeTakeDown"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgeTakeDown"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"), TEXT("Com_Option"), true);
					pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"), TEXT("Com_Option"), 0.5f);

					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"),
						SCENE_STATIC, TEXT("Effect_JudgePaperAttackRect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					/* ����Ʈ ��� -> ���� ���� �ٷ� ������� ��Ʈ�޴��������� */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgePaperAttackRect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"), TEXT("Com_Option"), true);
				}
			}
			else  if (6 == _pModel->Get_AnimIndex())
			{
				if (20 <= _pModel->Get_AnimationCurrentKeyFrame() && 44 >= _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -1.F;
				else if (58 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -1.25F;
				else if (59 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -1.25F;
				else if (58 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -1.25F;
				else if (60 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -2.5F;
				else if (61 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -2.5F;
				else if (62 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -2.5F;
				else if (63 == _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = -2.5F;
				if (67 <= _pModel->Get_AnimationCurrentKeyFrame() && 76 >= _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = 0.625F;
			}
			else if (14 == _pModel->Get_AnimIndex())
			{
				if (5 > _pModel->Get_AnimationCurrentKeyFrame())
					m_fSpeed = 5.f;
				m_pJudge->Attack(CJudge::ARMEDTYPE::ARMED, true);
				//���ΰ����ϸ鼭 ������ �޷����� ���� ����Ʈ �־ �ɵ�? �ٸ���ġ��

			}
			else if (4 == _pModel->Get_AnimIndex())
			{
				if (5 <= _pModel->Get_AnimationCurrentKeyFrame() &&
					!m_IsColliderOn)
				{
					m_IsColliderOn = true;
					m_pJudge->Attack(CJudge::ARMEDTYPE::UNARMED, true);
					//���ڸ� ������� ���Ⱑ ������� �κ�



					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"),
						SCENE_STATIC, TEXT("Effect_JudgeTakeDown"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgeTakeDown"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					_vector vPos = _pTransform->Get_State(CTransform::STATE_LOOK) * 3.f + _pTransform->Get_State(CTransform::STATE_RIGHT) * 0.1f + _pTransform->Get_State(CTransform::STATE_UP) * 0.2f;
					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"), TEXT("Com_Option"), true);
					pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgeTakeDown"), TEXT("Com_Option"), 0.5f);


					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bottom"),
						SCENE_STATIC, TEXT("Effect_Smoke_Bottom"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					/* ����Ʈ ��� -> ���� ���� �ٷ� ������� ��Ʈ�޴��������� */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, _pTransform, TEXT("Layer_Smoke_Bottom"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bottom"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bottom"), TEXT("Com_Option"), 0.5f);

					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"),
						SCENE_STATIC, TEXT("Effect_JudgePaperAttackRect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					/* ����Ʈ ��� -> ���� ���� �ٷ� ������� ��Ʈ�޴��������� */
					if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_JudgePaperAttackRect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"), TEXT("Com_Option"), vPos);
					pGameInstance->Setting_Effect_NonRotation(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_JudgePaperAttackRect"), TEXT("Com_Option"), true);
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}

		_pTransform->Add_Velocity(CTransform::STATE_LOOK, _float(TimeDelta)*m_fSpeed);
		return false;
	}
}

void CAttack_Judge::Reset_AttackState()
{
	m_IsDelayAttack = false;
	m_IsAttackLoop = false;
	m_IsAttackReady = false;
	m_IsAttack = false;
	m_IsRecovery = false;
	m_dDelayTime = 0.0;
}

CAttack_Judge * CAttack_Judge::Create(CJudge * _pJudge)
{
	CAttack_Judge* pInstance = new CAttack_Judge(_pJudge);

	return pInstance;
}

void CAttack_Judge::Free()
{
}
