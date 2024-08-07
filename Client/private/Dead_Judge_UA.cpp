#include "stdafx.h"
#include "..\public\Dead_Judge_UA.h"
#include "Static_Bell.h"
#include "Camera_Fly.h"

CDead_Judge_UA::CDead_Judge_UA(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CDead_Judge_UA::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsReadyDead)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		if (CJudge::TYPE_DEFAULT == m_pJudge->Get_Type())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE::CAMERATYPE_JUDGE_DEAD);
			((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Camera_Shaking(true, 0.7, true);
			((CStatic_Bell*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Static_Bell"), 0))->SetOn_Raise();
			RELEASE_INSTANCE(CGameInstance);
		}
		return true;
	}

	else
	{
		if (0 >= m_pJudge->Get_Hp() &&
			61 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(61);
			_vector vPos = _pTransform->Get_Position();
			_pController->Set_Position(_float3(XMVectorGetX(vPos), XMVectorGetY(vPos) + 0.65f, XMVectorGetZ(vPos)));
		}

		if (_pModel->Get_IsAnimFinishied(61))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"),
				SCENE_STATIC, TEXT("Effect_Smoke_Die"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return false;
			}

			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Die"), _pTransform->Get_Position() + _pTransform->Get_State(CTransform::STATE_UP) * 2.3f, true)))
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
			return false;
		}

		return false;
	}
}

CDead_Judge_UA * CDead_Judge_UA::Create(CJudge * _pJudge)
{
	CDead_Judge_UA* pInstance = new CDead_Judge_UA(_pJudge);

	return pInstance;
}

void CDead_Judge_UA::Free()
{
}
