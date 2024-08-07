#include "stdafx.h"
#include "..\public\Hit_Doubt.h"

CHit_Doubt::CHit_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CHit_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	m_IsHit = m_pDoubt->IsHit(); //피격상태를 계속 업데이트 할 것 이다
	if (0 > m_pDoubt->Get_Hp() ||
		m_pDoubt->IsAttack() ||
		!m_IsHit &&
		m_IsRecovery)		//피격상태가 아니며 & 회복이 다된 상태일 때 나갈거야
	{
		return true;
	}

	else
	{
		m_pDoubt->Set_Armed(true);
		if (_pModel->Get_IsAnimFinishied(24) ||
			_pModel->Get_IsAnimFinishied(33) ||
			_pModel->Get_IsAnimFinishied(38))
		{
			m_IsRecovery = true;
			m_IsRecoveryLoop = false;
			m_pDoubt->Reset_HitState();
			return false;
		}
		if (!m_IsRecoveryLoop)
		{
			m_IsRecovery = false;
			if (CEnemy::ATTACKED::TYPE_END != m_pDoubt->Get_Attacked_Type())
				m_IsRecoveryLoop = true;

			if (CEnemy::ATTACKED::TO == m_pDoubt->Get_Attacked_Type())
			{
				if (39 != _pModel->Get_AnimIndex())
				{
					//기절 이펙트 띄워야함 Hair 뼈가 있긴한데  90도정도 로테이션 줘야할듯?? 

					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_DizziStar"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DoubtStun"))))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_DoubtStun"), _pTransform, _pModel, "Effect")))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}
					RELEASE_INSTANCE(CGameInstance);

					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(39);
				}
			}
			else if (CEnemy::ATTACKED::PB == m_pDoubt->Get_Attacked_Type())
			{
				if (25 != _pModel->Get_AnimIndex())
				{
					//피격 애니메이션
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(25);
				}

			}
			else
			{
				if (33 != _pModel->Get_AnimIndex())
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(33);
				}
			}
		}
		else //연계가되는 애니메이션
		{
			if (_pModel->Get_IsAnimFinishied(39) &&
				38 != _pModel->Get_AnimIndex())
			{
				//기절이펙트 끄면됨.
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(38);
			}
			else if (_pModel->Get_IsAnimFinishied(25) &&
				39 != _pModel->Get_AnimIndex() &&
				24 != _pModel->Get_AnimIndex())
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(24);
			}
		}

		return false;
	}
}

CHit_Doubt * CHit_Doubt::Create(CDoubt * _pDoubt)
{
	CHit_Doubt* pInstance = new CHit_Doubt(_pDoubt);

	return pInstance;
}

void CHit_Doubt::Free()
{
	__super::Free();
}
