#include "stdafx.h"
#include "..\public\Dead_Censor.h"

CDead_Censor::CDead_Censor(CCensor * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CDead_Censor::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (m_IsReadyDead)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (0 >= m_pCensor->Get_Hp() &&
			35 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(35);
		}

		if (_pModel->Get_IsAnimFinishied(35))
		{
			//죽는 위치에 연기 이펙트 
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"),
				SCENE_STATIC, TEXT("Effect_Smoke_Die"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}

			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"), _pTransform->Get_Position() +  _pTransform->Get_State(CTransform::STATE_UP) * 1.5f, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StarDieRect"),
				SCENE_STATIC, TEXT("Effect_StarDieRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}

			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_StarDieRect"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_UP) * 1.5f, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}
			RELEASE_INSTANCE(CGameInstance);
			m_IsReadyDead = true;
		}

		if (35 == _pModel->Get_AnimIndex()
			&&10 > _pModel->Get_AnimationCurrentFrame(35)) //죽는 모션시 넉백 효과를 주기 위함.
		{
			_pTransform->Add_Velocity(CTransform::STATE_LOOK, (_float)(TimeDelta*-m_dPower)); // m_dPower 값에 넉백 거리가 달라짐
		}
		return false;
	}
}

void CDead_Censor::Reset_DeadState()
{
	m_IsReadyDead = false;
	m_dAcc = 0.0;
	m_dPower = 10.0;
}

CDead_Censor * CDead_Censor::Create(CCensor * _pCensor)
{
	CDead_Censor* pInstance = new CDead_Censor(_pCensor);

	return pInstance;
}

void CDead_Censor::Free()
{
	__super::Free();
}
