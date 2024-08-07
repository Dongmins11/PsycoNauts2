#include "stdafx.h"
#include "..\public\Hit_Censor.h"

CHit_Censor::CHit_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CHit_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	//33 ���� ���� hairMain << ���� �������.
	//				//�ǰ��̺�Ʈ�����̶�� �������� �⺻���� �ǰ� ����Ʈ ���� �� ��

	m_IsHit = m_pCensor->IsHit(); //�ǰݻ��¸� ��� ������Ʈ �� �� �̴�
	if (0 >= m_pCensor->Get_Hp() ||
		!m_IsHit &&
		m_IsRecovery)		//�ǰݻ��°� �ƴϸ� & ȸ���� �ٵ� ������ �� �����ž�
	{
		return true;
	}

	else
	{
		if (_pModel->Get_IsAnimFinishied(26) ||
			_pModel->Get_IsAnimFinishied(28) ||
			_pModel->Get_IsAnimFinishied(29) ||
			_pModel->Get_IsAnimFinishied(30) ||
			_pModel->Get_IsAnimFinishied(31) ||
			_pModel->Get_IsAnimFinishied(34))
		{
			m_IsRecovery = true;
			m_IsRecoveryLoop = false;
			m_pCensor->Reset_HitState();
			return true;
		}
		if (!m_IsRecoveryLoop)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			m_IsRecovery = false;
			if (CEnemy::ATTACKED::TYPE_END != m_pCensor->Get_Attacked_Type())
				m_IsRecoveryLoop = true;
			if (CEnemy::ATTACKED::FW == m_pCensor->Get_Attacked_Type())
			{
				if (28 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(28);
					//�ǰ�����Ʈ����

					//if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
					//	pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerHitEffect"),
					//	SCENE_STATIC, TEXT("Effect_HitEffect"))))
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return false;
					//}
					//
					//_vector vPos = XMVector3Normalize(pGameInstance->Get_CamPosition() - _pTransform->Get_Position());
					//
					//if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, _pTransform, TEXT("Layer_CencerHitEffect"))))
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return false;
					//}
					//
					//pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerHitEffect"), TEXT("Com_Option"), _pTransform->Get_State(CTransform::STATE_UP) * 0.5f + vPos);
				}
			}
			else if (CEnemy::ATTACKED::BK == m_pCensor->Get_Attacked_Type())
			{
				if (29 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(29);
					//�ǰ�����Ʈ����

				}
			}
			else if (CEnemy::ATTACKED::LT == m_pCensor->Get_Attacked_Type())
			{
				if (30 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(30);
					//�ǰ�����Ʈ����
				}
			}
			else if (CEnemy::ATTACKED::RT == m_pCensor->Get_Attacked_Type())
			{
				if (31 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(31);
					//�ǰ�����Ʈ����

				}
			}
			else if (CEnemy::ATTACKED::TO == m_pCensor->Get_Attacked_Type())
			{
				if (33 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(33);
					//�ǰ�����Ʈ����
					//33 ���� ���� hairMain << ���� �������.
					//33�� �����ɶ�������
					
					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DizziStar"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerStun"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					
					if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerStun"), _pTransform, _pModel, "Effect")))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					pGameInstance->Setting_Effect_Scale(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_CencerStun"), TEXT("Com_Option"), XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));
					
				}
			}
			else if (CEnemy::ATTACKED::PB == m_pCensor->Get_Attacked_Type())
			{
				if (25 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(25);
				}

			}
			RELEASE_INSTANCE(CGameInstance);
		}
		else //���谡�Ǵ� �ִϸ��̼�
		{
			if (_pModel->Get_IsAnimFinishied(25) &&
				26 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(26);
			}
			else if (_pModel->Get_IsAnimFinishied(33) &&
				26 != _pModel->Get_AnimIndex() &&
				34 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(34);
			}
		}
		if (28 == _pModel->Get_AnimIndex())
		{
			if (1> _pModel->Get_AnimationCurrentFrame(28))
				_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*-10.f);
		}
		else if (29 == _pModel->Get_AnimIndex())
		{
			if (1> _pModel->Get_AnimationCurrentFrame(29))
				_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*10.f);
		}
		else if (30 == _pModel->Get_AnimIndex())
		{
			if (1> _pModel->Get_AnimationCurrentFrame(30))
				_pTransform->Add_Velocity(CTransform::STATE_RIGHT, (_float)TimeDelta*10.f);
		}
		else if (31 == _pModel->Get_AnimIndex())
		{
			if (1> _pModel->Get_AnimationCurrentFrame(31))
				_pTransform->Add_Velocity(CTransform::STATE_RIGHT, (_float)TimeDelta*-10.f);
		}
		return false;
	}
}

CHit_Censor * CHit_Censor::Create(CCensor * _pCensor)
{
	CHit_Censor* pInstance = new CHit_Censor(_pCensor);

	return pInstance;
}

void CHit_Censor::Free()
{
	__super::Free();
}
