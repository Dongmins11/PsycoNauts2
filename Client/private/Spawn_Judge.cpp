#include "stdafx.h"
#include "..\public\Spawn_Judge.h"
#include "MonsterInfo.h"
#include "Camera_Fly.h"

CSpawn_Judge::CSpawn_Judge(CJudge * _pJudge)
	:m_pJudge(_pJudge)
{
}

_bool CSpawn_Judge::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (m_IsSpawned)
	{
		return true;
	}

	else
	{
		if (CJudge::TYPE_DEFAULT == m_pJudge->Get_Type())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_uint iLayerSize = pGameInstance->Get_LayerSize(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"));
			if (m_IsCamStart&&
				m_IsCreateInfo&&
				m_IsReadySpawn &&
				0 == iLayerSize)
			{
				m_IsSpawned = true;
				RELEASE_INSTANCE(CGameInstance);
				return true;
			}

			if (_pModel->Get_IsAnimFinishied(51) &&
				!m_IsReadySpawn)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(18);
				m_IsReadySpawn = true;
			}

			if (!m_IsCamStart)
			{
				((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERATYPE_JUDGE);

				m_IsCamStart = true;
			}

			if (!m_IsCreateInfo &&
				30 < _pModel->Get_AnimationCurrentKeyFrame())
			{
				CMonsterInfo::MONSTERINFO  tagMonsterInfo;
				ZeroMemory(&tagMonsterInfo, sizeof(CMonsterInfo::MONSTERINFO));
				tagMonsterInfo.iNumTextrue = CMonsterInfo::MONSTER_JUDGE;
				tagMonsterInfo.IsSpawn = &m_IsReadySpawn;

				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_MonsterUI"), &tagMonsterInfo);
				m_IsCreateInfo = true;

			}

			if (!m_IsScream &&
				50 <= _pModel->Get_AnimationCurrentKeyFrame())
			{
				((CCamera_Fly*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Camera_Shaking(true, 0.7, true);
				m_IsScream = true;
			}
			RELEASE_INSTANCE(CGameInstance);
		}

		else
		{
			if (_pModel->Get_IsAnimFinishied(51) &&
				!m_IsSpawned)
			{
				_pModel->Set_IsAnimLefp(true);
				_pModel->Set_Animation(18);
				m_IsSpawned = true;
				return true;
			}
		}
		return false;
	}
}

CSpawn_Judge * CSpawn_Judge::Create(CJudge * _pJudge)
{
	CSpawn_Judge* pInstance = new CSpawn_Judge(_pJudge);

	return pInstance;
}

void CSpawn_Judge::Free()
{
}
