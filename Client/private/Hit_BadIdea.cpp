#include "stdafx.h"
#include "..\public\Hit_BadIdea.h"

CHit_BadIdea::CHit_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CHit_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	m_IsHit = m_pBadIdea->IsHit(); //�ǰݻ��¸� ��� ������Ʈ �� �� �̴�
	if (0 >= m_pBadIdea->Get_Hp() ||
		!m_IsHit &&
		m_IsRecovery)		//�ǰݻ��°� �ƴϸ� & ȸ���� �ٵ� ������ �� �����ž�
	{
		return true;
	}

	else
	{
		if (_pModel->Get_IsAnimFinishied(8) ||
			_pModel->Get_IsAnimFinishied(25) ||
			_pModel->Get_IsAnimFinishied(34))
		{
			m_IsRecovery = true;
			m_IsRecoveryLoop = false;
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(37);
			m_pBadIdea->Reset_HitState();
			return false;
		}
		if (!m_IsRecoveryLoop)
		{
			m_IsRecovery = false;
			if (CEnemy::ATTACKED::TYPE_END != m_pBadIdea->Get_Attacked_Type())
				m_IsRecoveryLoop = true;
			
			if (CEnemy::ATTACKED::TO == m_pBadIdea->Get_Attacked_Type())
			{
				if (35 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(35);
					//���� ����Ʈ ��� headTop���� ���̸� ��.

					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DizziStar"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BeadIdeaStun"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BeadIdeaStun"), _pTransform, _pModel, "Effect")))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					RELEASE_INSTANCE(CGameInstance);
				}
			}
			else if (CEnemy::ATTACKED::PB == m_pBadIdea->Get_Attacked_Type())
			{
				if (9 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(9);
				}

			}
			else
			{
				if (26 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(26);
				}
			}
		}
		else //���谡�Ǵ� �ִϸ��̼�
		{
			if (_pModel->Get_IsAnimFinishied(26) &&
				25 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(25);
			}
			else if (_pModel->Get_IsAnimFinishied(9) &&
				8 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(8);
			}
			else if (_pModel->Get_IsAnimFinishied(35) &&
				34 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(34);
			}
		}

		if (26 == _pModel->Get_AnimIndex())
		{
			if(1> _pModel->Get_AnimationCurrentFrame(26))
				_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*-15.f);
			//�˹�ִϸ��̼��̶� ���⼭ ����� ����Ʈ ����
		}
			
		else if (25 == _pModel->Get_AnimIndex())
			_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta*-1.f);
		return false;
	}
}

CHit_BadIdea * CHit_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CHit_BadIdea* pInstance = new CHit_BadIdea(_pBadIdea);

	return pInstance;
}

void CHit_BadIdea::Free()
{
}
