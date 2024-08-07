#include "stdafx.h"
#include "..\public\Dead_GoatH.h"

CDead_GoatH::CDead_GoatH(CGoatHollis * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CDead_GoatH::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsReadyDead)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_StarEffect"),
			SCENE_STATIC, TEXT("Effect_Boss_StarEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_StarEffect"), _pTransform, _pModel, "AttackPoint")))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		pGameInstance->Setting_Effect_TransformPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_StarEffect"), TEXT("Com_Option"), CTransform::STATE_UP, 8.f);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_DieEffect"),
			SCENE_STATIC, TEXT("Effect_Boss_DieEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(pGameInstance->Effect_ParentBoneMatrix(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_DieEffect"), _pTransform, _pModel, "AttackPoint")))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		pGameInstance->Setting_Effect_TransformPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_DieEffect"), TEXT("Com_Option"), CTransform::STATE_UP, 8.f);

		RELEASE_INSTANCE(CGameInstance);

		return true;
	}

	else
	{
		if (0 >= m_pGoat->Get_Hp() &&
			16 != _pModel->Get_AnimIndex())
		{
			_pModel->Set_IsAnimLefp(true);
			_pModel->Set_Animation(16);
		}

		if (_pModel->Get_IsAnimFinishied(16))
		{
			m_IsReadyDead = true;
		}

		if (16 == _pModel->Get_AnimIndex())
		{
			if (120 < _pModel->Get_AnimationCurrentKeyFrame())
			{
				m_fScale += (_float)TimeDelta;
				if (3.f < m_fScale)
				{
					m_fScale = 3.f;
					_pTransform->Go_ByAxis(XMVectorSet(0.f, -1.f, 0.f, 0.f), TimeDelta);
				}
				_pModel->BoneScailing("spine0", m_fScale);
				_pModel->BoneScailing("spine2", 1.f / m_fScale);
				_pModel->BoneScailing("Lf_clavicle", m_fScale);
				_pModel->BoneScailing("Rt_clavicle", m_fScale);
				_pModel->BoneScailing("Lf_shoulder", 1.f / m_fScale);
				_pModel->BoneScailing("Rt_shoulder", 1.f / m_fScale);
				_pModel->BoneScailing("spine0", m_fScale);
			}
		}

		return false;
	}
}

CDead_GoatH * CDead_GoatH::Create(CGoatHollis * _pGoat)
{
	CDead_GoatH* pInstance = new CDead_GoatH(_pGoat);

	return pInstance;
}

void CDead_GoatH::Free()
{
}
