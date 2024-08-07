#include "stdafx.h"
#include "..\public\Attack_CensorB.h"

CAttack_CensorB::CAttack_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
	ZeroMemory(&m_tJumpInfo, sizeof(CCensor_Burly::JUMPINFO));
}

_bool CAttack_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pCensor->Get_Hp())
		return true;

	//트레일 달아야하는 시점 다잡아놨으니 cpp에 주석 달린곳에 트레일 업데이트 돌리면될거
	if (42 == _pModel->Get_AnimIndex() ||
		43 == _pModel->Get_AnimIndex())
	{
		if (_pModel->Get_IsAnimFinishied(42) ||
			_pModel->Get_IsAnimFinishied(43))
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(44);
		}
		return false;
	}
	if (m_IsDealyAttack)
	{
		m_dDelayTime += TimeDelta;
		if (3.0 < m_dDelayTime ||
			4.f > m_pCensor->Get_Distance())
		{
			m_IsDealyAttack = false;
		}
	}
	if (!m_IsAttackLoop &&
		!m_IsDealyAttack&&
		(340.f <= m_pCensor->Get_Degree() ||
			20.f >= m_pCensor->Get_Degree()))
	{
		if (8.f < m_pCensor->Get_Distance() &&
			13.f > m_pCensor->Get_Distance())
		{
			m_iAttackType = 2;
			m_IsAttackLoop = true;
			m_pCensor->Set_AttackLoop(true);
		}
		else if (4.f > m_pCensor->Get_Distance()&&
			3.f<= m_pCensor->Get_Distance())
		{
			m_iAttackType = 3;
			m_IsAttackLoop = true;
			m_pCensor->Set_AttackLoop(true);
		}
		else if (4.f <= m_pCensor->Get_Distance() &&
			8.f >= m_pCensor->Get_Distance())
		{
			m_iAttackType = rand() % 2;
			m_IsAttackLoop = true;
			m_pCensor->Set_AttackLoop(true);
		}
		else if (3.f > m_pCensor->Get_Distance())
		{
			m_iAttackType = 4;
			m_IsAttackLoop = true;
			m_pCensor->Set_AttackLoop(true);
		}
	}
	if (!m_IsAttackLoop ||
		m_IsDealyAttack)
	{
		return true;
	}
	else
	{
		if (3 == _pModel->Get_AnimIndex())
			m_dExhaustTime += TimeDelta;
		if (_pModel->Get_IsAnimFinishied(2) ||
			_pModel->Get_IsAnimFinishied(6) ||
			_pModel->Get_IsAnimFinishied(10) ||
			_pModel->Get_IsAnimFinishied(13) ||
			_pModel->Get_IsAnimFinishied(20))
		{
			m_dExhaustTime = 0.0;
			m_dDelayTime = 0.0;
			m_IsAttackLoop = false;
			m_IsDealyAttack = true;
			m_IsAttackReady1 = false;
			m_IsAttackReady2 = false;
			m_IsRecovery = false;
			m_IsAttack = false;
			m_IsRenderEffect = false;
			m_pCensor->SetOnTrail_LeftHand(false);
			m_pCensor->SetOnTrail_RightHand(false);
			m_IsPunch[CCensor_Burly::HAND_R] = false;
			m_IsPunch[CCensor_Burly::HAND_L] = false;
			m_fDistance = 0.f;
			_pModel->Set_AnimSpeed(9, 1.0);
			_pModel->Set_IsAnimLefp(true);
			_uint irandAnimType = rand() % 2;
			if (0 == irandAnimType)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(42);
			}
			else
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(43);
			}
			ZeroMemory(&m_tJumpInfo, sizeof(CCensor_Burly::JUMPINFO));
			return false;
		}
		Reset();
		switch (m_iAttackType)
		{
		case 0:
			if (3 != _pModel->Get_AnimIndex() &&
				2 != _pModel->Get_AnimIndex() &&
				4 != _pModel->Get_AnimIndex() &&
				5 != _pModel->Get_AnimIndex() &&
				0.0 == m_dExhaustTime &&
				!m_IsAttackReady1)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(5);
				m_pCensor->SetOnTrail_RightHand(true);
				m_IsAttackReady1 = true;
			}
			else if (_pModel->Get_IsAnimFinishied(5) &&
				0.0 == m_dExhaustTime &&
				!m_IsAttackReady2)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(3);
				m_IsAttackReady2 = true;
			}
			if (5.0 > m_dExhaustTime)
			{
				if (3.0f >= m_pCensor->Get_Distance() &&
					3 == _pModel->Get_AnimIndex() &&
					!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(4);
					m_IsAttack = true;
				}
				else if (_pModel->Get_IsAnimFinishied(4) &&
					!m_IsRecovery)
				{
					m_pCensor->SetOnTrail_RightHand(false);
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(2);
					////hariMain뼈위치에 별이펙트 기절 이펙트 들어가면됨
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DizziStar"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerBStun"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerBStun"), _pTransform, _pModel, "Effect")))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					RELEASE_INSTANCE(CGameInstance);
					m_IsRecovery = true;
				}

			}
			else
			{
				if (21 != _pModel->Get_AnimIndex() &&
					20 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(21);
				}
				else if (_pModel->Get_IsAnimFinishied(21) &&
					20 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(20);
				}
			}
			break;
		case 1:
			if (13 != _pModel->Get_AnimIndex() &&
				14 != _pModel->Get_AnimIndex() &&
				15 != _pModel->Get_AnimIndex() &&
				!m_IsAttackReady1)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(15);
				m_IsAttackReady1 = true;
			}
			else if (_pModel->Get_IsAnimFinishied(15) &&
				!m_IsAttack)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(14);
				m_IsAttack = true;
			}
			else if (_pModel->Get_IsAnimFinishied(14) &&
				!m_IsRecovery)
			{
				//원거리 공격 이펙트 도장이펙트 생성하면됨.룩방향으로 전진해야됨.
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(13);
				m_IsRecovery = true;
			}
			break;
		case 2:
			if (9 != _pModel->Get_AnimIndex() &&
				!m_IsAttackReady1)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(12);
				m_IsAttackReady1 = true;
			}
			else if (_pModel->Get_IsAnimFinishied(12) &&
				!m_IsAttackReady2)
			{
				//점프 동작이라서 발위치에 연기나 점프 이펙트 있으면 좋을 것 같음
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencerB"),
					SCENE_STATIC, TEXT("Effect_Smoke_Cencer"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencerB"), _pTransform->Get_Position(), true)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
				RELEASE_INSTANCE(CGameInstance);
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(9);
				m_fDistance = m_pCensor->Get_Distance();
				_pModel->Set_AnimSpeed(9, 1.0 / m_fDistance);
				m_tJumpInfo.fJumpPower = m_pCensor->Get_Distance() / 3.2f;
				m_IsAttackReady2 = true;
				m_pCensor->Set_Gravity(false);
			}
			else if (m_tJumpInfo.IsLandCheck &&
				!m_IsAttack)
			{
				//착지모션이 진행되서 여기에 연기 작은거 발위치에 띄우면될듯
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(11);
				m_pCensor->Set_Gravity(true);
				m_IsAttack = true;

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencerB"),
					SCENE_STATIC, TEXT("Effect_Smoke_Cencer"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencerB"), _pTransform->Get_Position(), true)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (_pModel->Get_IsAnimFinishied(11) &&
				!m_IsRecovery)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(10);
				m_IsRecovery = true;
			}
			break;
		case 3:
			if (8 != _pModel->Get_AnimIndex() &&
				!m_IsAttackReady1)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(8);
				m_IsAttackReady1 = true;
			}
			else if (8 == _pModel->Get_AnimIndex() &&
				_pModel->Get_IsAnimFinishied(8) &&
				7 != _pModel->Get_AnimIndex() &&
				!m_IsAttack)
			{
				m_pCensor->SetOnTrail_LeftHand(true);
				m_pCensor->SetOnTrail_RightHand(true);
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(7);
				m_IsAttack = true;
			}
			else if (7 == _pModel->Get_AnimIndex() &&
				_pModel->Get_IsAnimFinishied(7) &&
				6 != _pModel->Get_AnimIndex() &&
				!m_IsRecovery)
			{
				m_pCensor->SetOnTrail_LeftHand(false);
				m_pCensor->SetOnTrail_RightHand(false);
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(6);
				m_IsRecovery = true;
			}
			break;
		case 4:
			if (!m_IsAttack)
			{
				_pModel->Set_AnimBlendSpeed(11, 10.0);
				_pModel->Set_AnimSpeed(11, 1.0);
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(11);
				m_pCensor->Set_Gravity(true);
				m_IsAttack = true;
			}
			else if (_pModel->Get_IsAnimFinishied(11) &&
				!m_IsRecovery)
			{
				_pModel->Set_AnimBlendSpeed(11, 20.0);
				_pModel->Set_AnimSpeed(11, 3.0);
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(10);
				m_IsRecovery = true;
			}
			break;
		}

		if (7 == _pModel->Get_AnimIndex())
		{
			if (2 < _pModel->Get_AnimationCurrentFrame(7) &&
				7 > _pModel->Get_AnimationCurrentFrame(7) ||
				27 < _pModel->Get_AnimationCurrentFrame(7) &&
				34 > _pModel->Get_AnimationCurrentFrame(7))
			{
				m_IsMove = true;
				m_dSpeed = 4.0;
			}

			if (2 <= _pModel->Get_AnimationCurrentKeyFrame()&&
				6 >=_pModel->Get_AnimationCurrentKeyFrame())
			{
				m_pCensor->Attack(CCensor_Burly::HAND_R, true);
				//오른손위치에 타격이펙트
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_BoneMat"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"),
					SCENE_STATIC, TEXT("Effect_HeavyCancerCharge"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"), _pTransform, _pModel, "RightHand")))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
				pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"), TEXT("Com_Option"), 0.3f);
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (27 <= _pModel->Get_AnimationCurrentKeyFrame()&&
				30>=_pModel->Get_AnimationCurrentKeyFrame())
			{
				m_pCensor->Attack(CCensor_Burly::HAND_L, true);

				//왼손위치에 타격이펙트
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_BoneMat"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"),
					SCENE_STATIC, TEXT("Effect_HeavyCancerCharge"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"), _pTransform, _pModel, "LeftHand")))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
				pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"), TEXT("Com_Option"), 0.3f);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else if (9 == _pModel->Get_AnimIndex())
		{
			_pTransform->Jump(TimeDelta, &m_tJumpInfo.IsJump, nullptr, &m_tJumpInfo.lfJumpTime, &m_tJumpInfo.fJumpPower, &m_tJumpInfo.fJumpValue, &m_tJumpInfo.IsFallCheck, &m_tJumpInfo.IsLandCheck, _pController);
			m_IsMove = true;
			m_dSpeed = m_fDistance;
		}
		else if (3 == _pModel->Get_AnimIndex())
		{
			m_IsMove = true;
			m_dSpeed = 5.0;
		}
		else if (36 == _pModel->Get_AnimIndex())
		{
			m_IsMove = true;
			m_dSpeed = 1.0;
		}
		else if (11 == _pModel->Get_AnimIndex()
			&&6<=_pModel->Get_AnimationCurrentKeyFrame())
		{
			if (!m_IsRenderEffect)
			{
				m_IsRenderEffect = true;
				//RightHand LeftHand 뼈위치나 루트기준 앞방향에 연기 이펙트 생성해도됨 땅 내려찍는 공격임 루트기준 앞방향으로 잡는게 편할거
				m_pCensor->Attack(CCensor_Burly::HAND_L, true);
				m_pCensor->Attack(CCensor_Burly::HAND_R, true);

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerImpact"),
					SCENE_STATIC, TEXT("Effect_HeavyCancerImpact"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerImpact"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_LOOK) * 1.6f /*+ _pTransform->Get_State(CTransform::STATE_RIGHT) * -0.2f*/ + _pTransform->Get_State(CTransform::STATE_UP) * 0.1f, true)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
				pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerImpact"), TEXT("Com_Option"), 0.3f);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else if (4 == _pModel->Get_AnimIndex() &&
			9 <= _pModel->Get_AnimationCurrentKeyFrame())
		{
			if (!m_IsRenderEffect)
			{
				m_IsRenderEffect = true;
				//RightHand 뼈위치에 타격이펙트

				m_pCensor->Attack(CCensor_Burly::HAND_R, true);

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_BoneMat"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"),
					SCENE_STATIC, TEXT("Effect_HeavyCancerCharge"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"), _pTransform, _pModel, "RightHand")))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
				pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_HeavyCancerCharge"), TEXT("Com_Option"), 0.3f);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		if (m_IsMove)
			_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)(TimeDelta*m_dSpeed));

		return false;
	}

}

void CAttack_CensorB::Reset()
{
	m_dSpeed = 1.0;
	m_IsMove = false;
}

void CAttack_CensorB::Reset_AttackState()
{
	m_dExhaustTime = 0.0;
	m_dDelayTime = 0.0;
	m_IsAttackLoop = false;
	m_IsDealyAttack = false;
	m_IsAttackReady1 = false;
	m_IsAttackReady2 = false;
	m_IsRecovery = false;
	m_IsAttack = false;
	m_IsRenderEffect = false;
	m_pCensor->SetOnTrail_LeftHand(false);
	m_pCensor->SetOnTrail_RightHand(false);
	m_IsPunch[CCensor_Burly::HAND_R] = false;
	m_IsPunch[CCensor_Burly::HAND_L] = false;
	ZeroMemory(&m_tJumpInfo, sizeof(CCensor_Burly::JUMPINFO));
}

CAttack_CensorB * CAttack_CensorB::Create(CCensor_Burly * _pCensor)
{
	CAttack_CensorB* pInstance = new CAttack_CensorB(_pCensor);

	return pInstance;
}

void CAttack_CensorB::Free()
{
	__super::Free();
}
