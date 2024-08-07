#include "stdafx.h"
#include "..\public\Dead_BadIdea.h"

CDead_BadIdea::CDead_BadIdea(CBadIdea * _pBadIdea)
	:m_pBadIdea(_pBadIdea)
{
}

_bool CDead_BadIdea::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsReadyDead)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		return true;
	}

	else
	{
		if (0 >= m_pBadIdea->Get_Hp() &&
			11 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(11);
		}

		if (_pModel->Get_IsAnimFinishied(11))
		{
			m_IsReadyDead = true;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->PlaySoundOnce(L"BadIDea_Die.ogg", CHANNELID::MONSTER);
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

CDead_BadIdea * CDead_BadIdea::Create(CBadIdea * _pBadIdea)
{
	CDead_BadIdea* pInstance = new CDead_BadIdea(_pBadIdea);

	return pInstance;
}

void CDead_BadIdea::Free()
{
}
