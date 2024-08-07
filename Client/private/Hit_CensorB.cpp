#include "stdafx.h"
#include "..\public\Hit_CensorB.h"

CHit_CensorB::CHit_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CHit_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	m_IsHit = m_pCensor->IsHit(); //피격상태를 계속 업데이트 할 것 이다
	if (0 >= m_pCensor->Get_Hp() ||
		!m_IsHit &&
		m_IsRecovery)		//피격상태가 아니며 & 회복이 다된 상태일 때 나갈거야
	{
		return true;
	}

	else
	{
		if (_pModel->Get_IsAnimFinishied(24) ||
			_pModel->Get_IsAnimFinishied(25) ||
			_pModel->Get_IsAnimFinishied(26) ||
			_pModel->Get_IsAnimFinishied(27) ||
			_pModel->Get_IsAnimFinishied(16) ||
			_pModel->Get_IsAnimFinishied(40))
		{
			m_IsRecovery = true;
			m_IsRecoveryLoop = false;
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(18);
			m_pCensor->Reset_HitState();
			return false;
		}
		if (!m_IsRecoveryLoop)
		{
			m_IsRecovery = false;
			if (CEnemy::ATTACKED::TYPE_END != m_pCensor->Get_Attacked_Type())
				m_IsRecoveryLoop = true;
			if (CEnemy::ATTACKED::FW == m_pCensor->Get_Attacked_Type())
			{
				if (24 != _pModel->Get_AnimIndex())
				{
					//피격 이펙트
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(24);
				}
			}
			else if (CEnemy::ATTACKED::BK == m_pCensor->Get_Attacked_Type())
			{
				if (25 != _pModel->Get_AnimIndex())
				{
					//피격 이펙트
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(25);
				}
			}
			else if (CEnemy::ATTACKED::LT == m_pCensor->Get_Attacked_Type())
			{
				if (27 != _pModel->Get_AnimIndex())
				{
					//피격 이펙트
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(27);
				}
			}
			else if (CEnemy::ATTACKED::RT == m_pCensor->Get_Attacked_Type())
			{
				if (26 != _pModel->Get_AnimIndex())
				{
					//피격 이펙트

					//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					//if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
					//	pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerBHitEffect3"),
					//	SCENE_STATIC, TEXT("Effect_HitEffect3"))))
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return false;
					//}

					//_vector vPos = XMVector3Normalize(pGameInstance->Get_CamPosition() - _pTransform->Get_Position());

					//if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_CencerBHitEffect3"))))
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return false;
					//}

					//pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerBHitEffect3"), TEXT("Com_Option"), _pTransform->Get_State(CTransform::STATE_UP) * 0.8f + vPos);

					//RELEASE_INSTANCE(CGameInstance);

					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(26);
				}
			}
			else if (CEnemy::ATTACKED::TO == m_pCensor->Get_Attacked_Type())
			{
				if (41 != _pModel->Get_AnimIndex())
				{
					//기절 이펙트 hairMain 에 달면됨
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DizziStar"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerBStun"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerBStun"), _pTransform, _pModel, "hairMain")))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					RELEASE_INSTANCE(CGameInstance);

					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(41);
				}
			}
			else if (CEnemy::ATTACKED::PB == m_pCensor->Get_Attacked_Type())
			{
				if (17 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(17);
				}

			}
		}
		else //연계가되는 애니메이션
		{
			if (_pModel->Get_IsAnimFinishied(17) &&
				16 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(16);
			}
			else if (_pModel->Get_IsAnimFinishied(41) &&
				17 != _pModel->Get_AnimIndex() &&
				40 != _pModel->Get_AnimIndex())
			{
				//기절이펙트 멈춰야되는 시점
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(40);
			}
		}


		if (24 == _pModel->Get_AnimIndex())
		{
			if (1 > _pModel->Get_AnimationCurrentFrame(24))
				_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*-10.f);
		}

		else if (25 == _pModel->Get_AnimIndex())
		{
			if (1 > _pModel->Get_AnimationCurrentFrame(25))
				_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*10.f);
		}
		else if (26 == _pModel->Get_AnimIndex())
		{
			if (1> _pModel->Get_AnimationCurrentFrame(26))
				_pTransform->Add_Velocity(CTransform::STATE_RIGHT, (_float)TimeDelta*10.f);

		}
		else if (27 == _pModel->Get_AnimIndex())
		{
			if (1 > _pModel->Get_AnimationCurrentFrame(27))
				_pTransform->Add_Velocity(CTransform::STATE_RIGHT, (_float)TimeDelta*10.f);
		}
		return false;
	}
}

CHit_CensorB * CHit_CensorB::Create(CCensor_Burly * _pCensor)
{
	CHit_CensorB* pInstance = new CHit_CensorB(_pCensor);

	return pInstance;
}

void CHit_CensorB::Free()
{
	__super::Free();
}
