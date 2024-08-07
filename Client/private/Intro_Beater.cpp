#include "stdafx.h"
#include "..\public\Intro_Beater.h"

CIntro_Beater::CIntro_Beater(CBeater * _pBeater)
	:m_pBeater(_pBeater)
{
}

_bool CIntro_Beater::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
		return true;

	else
	{
		if (_pModel->Get_IsAnimFinishied(8))
		{
			m_IsSpawned = true;
			m_pBeater->Spawned(true);
			return false;
		}
		else if (8 == _pModel->Get_AnimIndex())
		{
			if (31 <= _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsRenderEffect)
			{
				//���⼭����
				m_IsRenderEffect = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BossDonut"), SCENE_STAGE05, TEXT("Layer_BossDount"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				RELEASE_INSTANCE(CGameInstance);
			}
			else if (120 <= _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsDestroyEffect)
			{
				m_IsDestroyEffect = true;
				//���⼭ �Ҹ�

				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Delete_GameObject(SCENE_STAGE05, TEXT("Layer_BossDount"), 0)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				RELEASE_INSTANCE(CGameInstance);
			}
		}
		return false;
	}
}

CIntro_Beater * CIntro_Beater::Create(CBeater * _pBeater)
{
	CIntro_Beater* pInstance = new CIntro_Beater(_pBeater);

	return pInstance;
}

void CIntro_Beater::Free()
{
}
