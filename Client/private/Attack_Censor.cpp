#include "stdafx.h"
#include "..\public\Attack_Censor.h"

CAttack_Censor::CAttack_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CAttack_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{	//12 - 5번째 프레임 도장 데칼이펙트 필요
	//15 - 5번째 프레임 고함 지르는 모션인데 그에 따른 이펙트 필요 << 원거리공격
	//18 - 트레일 + 주변 소용돌이 이펙트  19번째가 회복애니메이션이기에 그때 이펙트가 죽으면됨. 서서히
	//9 - 4번째 프레임 도장 데칼이펙트 필요 룩
	if (0 >= m_pCensor->Get_Hp())
		return true;

	if (m_IsDelayAttack)
	{
		m_dDelayTime += TimeDelta;
		if (4.0 < m_dDelayTime ||
			(1.2f > m_pCensor->Get_Distance()))
		{
			m_IsDelayAttack = false;
			m_isFinishedAttack = false;
			m_dDelayTime = 0.0;
		}
	}

	if (_pModel->Get_IsAnimFinishied(16) ||
		_pModel->Get_IsAnimFinishied(19) ||
		_pModel->Get_IsAnimFinishied(12) ||
		_pModel->Get_IsAnimFinishied(9) &&
		!m_isFinishedAttack)
	{
		m_isFinishedAttack = true;
		m_IsAttackLoop = false;
		m_IsDelayAttack = true;
		m_pCensor->Set_AttackLoop(false);
		m_pCensor->Set_Gravity(true);
		_pModel->Set_IsAnimLefp(true);
		_pModel->Set_Animation(4);
		_pModel->Set_AnimSpeed(12, 1.0);
		_pModel->Set_AnimSpeed(9, 1.0);
		m_IsRenderEffect = false;
		m_IsRenderModelEffect = false;
		ZeroMemory(&m_tJumpInfo, sizeof(CCensor::JUMPINFO));
		return false;
	}

	if (!m_IsAttackLoop&&
		(340.f <=m_pCensor->Get_Degree()||
			20.f>=m_pCensor->Get_Degree()))
	{
		if (5.f> m_pCensor->Get_Distance() &&
			3.f< m_pCensor->Get_Distance())
		{
			m_IsAttackLoop = true;
			m_pCensor->Set_AttackLoop(true);
			m_iAttackType = rand() % 2 + 2;
		}
		else if (2.f> m_pCensor->Get_Distance())
		{
			m_IsAttackLoop = true;
			m_pCensor->Set_AttackLoop(true);
			m_iAttackType = rand() % 2;
		}
	}
	if (!m_IsAttackLoop ||
		m_IsDelayAttack)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}
	else
	{
		if (m_IsAttackLoop)
		{
			Reset();
			switch (m_iAttackType)
			{
			case 0:
				if (9 != _pModel->Get_AnimIndex() &&
					11 != _pModel->Get_AnimIndex() &&
					12 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(11);
				}
				else if (_pModel->Get_IsAnimFinishied(11) &&
					12 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(12);
				}
				break;
			case 1:
				if (17 != _pModel->Get_AnimIndex() &&
					18 != _pModel->Get_AnimIndex() &&
					19 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(17);
				}
				else if (_pModel->Get_IsAnimFinishied(17) &&
					18 != _pModel->Get_AnimIndex() &&
					19 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(18);
				}
				else if (_pModel->Get_IsAnimFinishied(18) &&
					19 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(19);
				}
				break;
			case 2:
				if (14 != _pModel->Get_AnimIndex() &&
					15 != _pModel->Get_AnimIndex() &&
					16 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(14);
				}
				else if (_pModel->Get_IsAnimFinishied(14) &&
					15 != _pModel->Get_AnimIndex() &&
					16 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(15);
				}
				else if (_pModel->Get_IsAnimFinishied(15) &&
					16 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(16);
				}
				break;
			case 3:
				if (6 != _pModel->Get_AnimIndex() &&
					7 != _pModel->Get_AnimIndex() &&
					8 != _pModel->Get_AnimIndex() &&
					9 != _pModel->Get_AnimIndex() &&
					10 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(6);
				}
				else if (_pModel->Get_IsAnimFinishied(6) &&
					7 != _pModel->Get_AnimIndex() &&
					10 != _pModel->Get_AnimIndex() &&
					9 != _pModel->Get_AnimIndex())
				{
					m_pCensor->Set_Gravity(false);
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(7);
					m_fDistance = m_pCensor->Get_Distance();
					_pModel->Set_AnimSpeed(7, 1.0 / m_fDistance);
					m_tJumpInfo.fJumpPower = m_pCensor->Get_Distance() / 1.5f;
				}
				else if (m_tJumpInfo.IsLandCheck &&
					9 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(9);
				}
				break;
			}

			if (12 == _pModel->Get_AnimIndex())
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (!m_IsRenderModelEffect)
				{
					m_IsRenderModelEffect = true;

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_CencerCSW"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerCSW"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerCSW"), _pTransform, _pModel, "Weapon")))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
				}

				if (3 < _pModel->Get_AnimationCurrentFrame(12))
					_pModel->Set_AnimSpeed(12, 0.1);

				if(3<=_pModel->Get_AnimationCurrentKeyFrame()&&
					5>=_pModel->Get_AnimationCurrentKeyFrame())
					m_pCensor->Swing(true);

				if (5 <= _pModel->Get_AnimationCurrentKeyFrame())
				{
					if (!m_IsRenderEffect)
					{
						//12 - 5번째 프레임 도장 바닥에 찍혀야됩니다
						//뼈이름은 Weapon 필요하면쓰삼
						m_IsRenderEffect = true;
						
						/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
						if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
							pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StopEffect"),
							SCENE_STATIC, TEXT("Effect_StopEffect"))))
						{
							RELEASE_INSTANCE(CGameInstance);
							return false;
						}
						
						if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StopEffect"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_LOOK) * 1.6f + _pTransform->Get_State(CTransform::STATE_RIGHT) * -0.2f + _pTransform->Get_State(CTransform::STATE_UP) * 0.1f, true)))
						{
							RELEASE_INSTANCE(CGameInstance);
							return false;
						}

						pGameInstance->Setting_Effect_Speed(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StopEffect"), TEXT("Com_Option"), 0.5f);

						if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
							pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencer"),
							SCENE_STATIC, TEXT("Effect_Smoke_Cencer"))))
						{
							RELEASE_INSTANCE(CGameInstance);
							return false;
						}

						if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencer"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_LOOK) * 1.6f + _pTransform->Get_State(CTransform::STATE_RIGHT) * -0.2f + _pTransform->Get_State(CTransform::STATE_UP) * 0.1f, true)))
						{
							RELEASE_INSTANCE(CGameInstance);
							return false;
						}
					}
				}
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (7 == _pModel->Get_AnimIndex())
			{
				m_tJumpInfo.lfJumpTime += (TimeDelta * 0.2);
				_pTransform->Jump(TimeDelta, &m_tJumpInfo.IsJump, nullptr, &m_tJumpInfo.lfJumpTime, &m_tJumpInfo.fJumpPower, &m_tJumpInfo.fJumpValue, &m_tJumpInfo.IsFallCheck, &m_tJumpInfo.IsLandCheck, _pController);
				m_IsMove = true;
				m_dSpeed = m_fDistance;
			}
			else if (9 == _pModel->Get_AnimIndex() &&
				3 > _pModel->Get_AnimationCurrentKeyFrame())
			{
				_pModel->Set_AnimSpeed(9, 0.25);

				if (!m_IsRenderEffect)
				{
					m_IsRenderEffect = true;
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencer"),
						SCENE_STATIC, TEXT("Effect_Smoke_Cencer"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_SmokeCencer"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_LOOK) * 1.6f + _pTransform->Get_State(CTransform::STATE_RIGHT) * -0.2f + _pTransform->Get_State(CTransform::STATE_UP) * 0.1f, true)))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					RELEASE_INSTANCE(CGameInstance);
				}
				m_pCensor->Swing(true);
			}
			else if (9 == _pModel->Get_AnimIndex() &&
				4 <= _pModel->Get_AnimationCurrentKeyFrame())
			{
				_pModel->Set_AnimSpeed(9, 1.0);
				if (!m_IsRenderEffect)
				{
					//9 - 4번째 프레임 도장  바닥에 찍혀야됩니다 
					//뼈이름은 Weapon 뼈이름필요하면쓰삼
					m_IsRenderEffect = true;
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
					if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
						pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StopEffect"),
						SCENE_STATIC, TEXT("Effect_StopEffect"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StopEffect"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_LOOK) * 1.3f + _pTransform->Get_State(CTransform::STATE_RIGHT) * -0.2f + _pTransform->Get_State(CTransform::STATE_UP) * 0.1f, true)))
						return false;

					RELEASE_INSTANCE(CGameInstance);
				}
			}
			else if (15 == _pModel->Get_AnimIndex() &&
				5 <= _pModel->Get_AnimationCurrentKeyFrame())
			{
				if (!m_IsRenderEffect)
				{
					//15 - 5번째 프레임 고함 지르는 모션인데 그에 따른 이펙트 도장 이미지 필요 << 원거리공격
				//이친구는 이펙트에 콜라이더 달아야되서 위치값 받아와야함. 
					m_IsRenderEffect = true;
				}
			}
			else if (18 == _pModel->Get_AnimIndex())
			{
				if (!m_IsRenderEffect)
				{
					//18 - 트레일 + 주변 소용돌이 이펙트  19번째가 회복애니메이션이기에 그때 이펙트가 죽으면됨. 서서히
					//뼈이름은 Weapon
					m_IsRenderEffect = true;
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_CencerDonut"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerSwirl"), &m_pDonut, _pTransform)))
						return false;
					RELEASE_INSTANCE(CGameInstance);
				}
				if(64>_pModel->Get_AnimationCurrentKeyFrame())
					m_pCensor->Swing(true);
			}
		}

		if (m_IsMove)
			_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)(TimeDelta*m_dSpeed));
		return false;
	}

}

void CAttack_Censor::Reset_AttackState()
{
	m_isFinishedAttack = false;
	m_IsAttackLoop = false;
	m_IsDelayAttack = false;
	m_IsRenderEffect = false;
	m_IsRenderModelEffect = false;
	m_pCensor->Set_AttackLoop(false);
	m_pCensor->Set_Gravity(true);
	m_dDelayTime = 0.0;
}

void CAttack_Censor::Clear_Effect()
{
	if (nullptr != m_pDonut)
	{
		m_pDonut->Set_IsDestroy(true);
		m_pDonut = nullptr;
	}
}

void CAttack_Censor::Reset()
{
	m_dSpeed = 1.0;
	m_IsMove = false;
}

CAttack_Censor * CAttack_Censor::Create(CCensor * _pCensor)
{
	CAttack_Censor* pInstance = new CAttack_Censor(_pCensor);

	return pInstance;
}

void CAttack_Censor::Free()
{
	__super::Free();
}
