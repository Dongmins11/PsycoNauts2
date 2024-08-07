#include "stdafx.h"
#include "..\public\Dead_CensorB.h"

CDead_CensorB::CDead_CensorB(CCensor_Burly * _pCensor)
	:m_pCensor(_pCensor)
{
}

_bool CDead_CensorB::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (m_IsReadyDead)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (0 >= m_pCensor->Get_Hp() &&
			19 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(19);
		}

		if (_pModel->Get_IsAnimFinishied(19))
		{
			m_IsReadyDead = true;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"),
				SCENE_STATIC, TEXT("Effect_Smoke_Die"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}

			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_UP) * 1.5f, true)))
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
			return true;
		}

		return false;
	}
}

CDead_CensorB * CDead_CensorB::Create(CCensor_Burly * _pCensor)
{
	CDead_CensorB* pInstance = new CDead_CensorB(_pCensor);

	return pInstance;
}

void CDead_CensorB::Free()
{
	__super::Free();
}
