#include "stdafx.h"
#include "..\public\Dead_Doubt.h"

CDead_Doubt::CDead_Doubt(CDoubt * _pDoubt)
	:m_pDoubt(_pDoubt)
{
}

_bool CDead_Doubt::Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)
{
	if (m_IsReadyDead)
	{
		return true;
	}

	else
	{
		if (0 >= m_pDoubt->Get_Hp() &&
			27 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(27);
		}

		if (_pModel->Get_IsAnimFinishied(27))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"),
				SCENE_STATIC, TEXT("Effect_Smoke_Die"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}

			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"), _pTransform->Get_Position(), true)))
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

		return false;
	}
}

CDead_Doubt * CDead_Doubt::Create(CDoubt * _pDoubt)
{
	CDead_Doubt* pInstance = new CDead_Doubt(_pDoubt);

	return pInstance;
}

void CDead_Doubt::Free()
{
	__super::Free();
}
