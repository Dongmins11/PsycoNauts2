#include "stdafx.h"
#include "..\public\Attack_Beater.h"
#include "Puke_Ball.h"

CAttack_Beater::CAttack_Beater(CBeater * _pBeater)
	:m_pBeater(_pBeater)
{
}

_bool CAttack_Beater::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pBeater->Get_Hp())
		return true;

	if (_pModel->Get_IsAnimFinishied(1) ||
		_pModel->Get_IsAnimFinishied(4)||
		_pModel->Get_IsAnimFinishied(6))
	{
		m_IsDelayAttack = true;
		m_IsAttackLoop = false;
		m_IsReadyAttack = false; 
		m_IsAttack = false;
		m_IsRecovery = false;
		m_pBeater->Set_AttackLoop(false);
		m_IsRenderEffect = false;
		m_IsDestroyEffect = false;
		m_IsThrow[0] = false;
		m_IsThrow[1] = false;
		m_IsThrow[2] = false;
		m_IsThrow[3] = false;
		m_IsThrow[4] = false;
		if (4 == _pModel->Get_AnimIndex())
		{
   			m_pBeater->Set_LinkSkill(false);
		}
		return true;
	}

	if (!m_IsAttackLoop&&
		m_pBeater->IsLock())
		return true;

	if (m_pBeater->IsPatternLock())
	{
		if (!m_IsAttackLoop &&
			!m_IsDelayAttack&&
			m_pBeater->IsLinkSkill())
		{
			m_pBeater->Set_AttackLoop(true);
			m_IsAttackLoop = true;
			m_iAttackType = 2;
		}
	}
	else {
		if (!m_IsAttackLoop &&
			!m_IsDelayAttack)
		{
			m_pBeater->Set_AttackLoop(true);
			m_IsAttackLoop = true;
			m_iAttackType = rand() % 2;
		}
	}

	if (m_IsDelayAttack)
	{
		m_dDelayTime += TimeDelta;
		if (4.0 < m_dDelayTime)
		{
			m_IsDelayAttack = false;
			m_dDelayTime = 0.0;
		}
	}

	if (!m_IsAttackLoop ||
		m_IsDelayAttack)		//10미만의 거리로 좁혀지면 Idle상태를 벗어날거야 시야각 10도 안에 들어와있어야 true
	{
		return true;
	}

	else
	{
		if (m_IsAttackLoop)
		{
			if (0 == m_iAttackType)
			{
				if (!m_IsReadyAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(2);
					m_IsReadyAttack = true;
				}
				else if (!m_IsAttack &&
					_pModel->Get_IsAnimFinishied(2))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(0);
					m_IsAttack = true;
				}
				else if (!m_IsRecovery&&
					_pModel->Get_IsAnimFinishied(0))
				{
					//이펙트 소멸시점입니다.
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(1);
					m_IsRecovery = true;
					
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					if (FAILED(pGameInstance->Delete_GameObject(SCENE_STAGE05, TEXT("Layer_BossDount"), 0)))
					{
						RELEASE_INSTANCE(CGameInstance);
						return false;
					}

					RELEASE_INSTANCE(CGameInstance);
				}
			}
			else if(1==m_iAttackType)
			{
				if (!m_IsReadyAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(6);
					m_IsReadyAttack = true;
				}
			}
			else
			{
				if (!m_IsReadyAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(5);
					m_IsReadyAttack = true;
				}
				else if (_pModel->Get_IsAnimFinishied(5) &&
					!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(3);
					m_IsAttack = true;
				}
				else if (_pModel->Get_IsAnimFinishied(3) &&
					!m_IsRecovery)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(4);
					m_IsRecovery = true;
				}
				
			}
		}

		if (2 == _pModel->Get_AnimIndex()) //믹서기의 회전 공격입니다. AttackPoint 뼈위치에 토네이도 이펙트 달면 될 것 같습니다 소멸시점은 94번째 코드라인에서 잡아주시면됩니다.
		{
			if (50 <= _pModel->Get_AnimationCurrentKeyFrame()&&
				!m_IsRenderEffect)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BossDonut"), SCENE_STAGE05, TEXT("Layer_BossDount"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				RELEASE_INSTANCE(CGameInstance);
				//이펙트 생성해주세요
				m_IsRenderEffect = true;
			}
		}
		else if (3 == _pModel->Get_AnimIndex())
		{
			if (0 < _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsThrow[0])
			{
				m_IsThrow[0] = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CPuke_Ball::BallWorldDesc tDesc;
				tDesc.fDistance = m_pBeater->Get_Distance();
				XMStoreFloat3(&tDesc.vPos, XMLoadFloat4((_float4*)m_pBeater->Get_BoneMatrix().m[3]));
				tDesc.vDir.x = m_pBeater->Get_TargetDir().x;
				tDesc.vDir.y = m_pBeater->Get_TargetDir().y;
				tDesc.vDir.z = m_pBeater->Get_TargetDir().z;
				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), SCENE_STAGE05, TEXT("Layer_Ball"), &tDesc);
				
				RELEASE_INSTANCE(CGameInstance);

			}
			else if (10 < _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsThrow[1])
			{
				m_IsThrow[1] = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CPuke_Ball::BallWorldDesc tDesc;
				tDesc.fDistance = m_pBeater->Get_Distance();
				XMStoreFloat3(&tDesc.vPos, XMLoadFloat4((_float4*)m_pBeater->Get_BoneMatrix().m[3]));
				tDesc.vDir.x = m_pBeater->Get_TargetDir().x;
				tDesc.vDir.y = m_pBeater->Get_TargetDir().y;
				tDesc.vDir.z = m_pBeater->Get_TargetDir().z;
				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), SCENE_STAGE05, TEXT("Layer_Ball"), &tDesc);
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (20 < _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsThrow[2])
			{
				m_IsThrow[2] = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CPuke_Ball::BallWorldDesc tDesc;
				tDesc.fDistance = m_pBeater->Get_Distance();
				XMStoreFloat3(&tDesc.vPos, XMLoadFloat4((_float4*)m_pBeater->Get_BoneMatrix().m[3]));
				tDesc.vDir.x = m_pBeater->Get_TargetDir().x;
				tDesc.vDir.y = m_pBeater->Get_TargetDir().y;
				tDesc.vDir.z = m_pBeater->Get_TargetDir().z;
				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), SCENE_STAGE05, TEXT("Layer_Ball"), &tDesc);
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (30 < _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsThrow[3])
			{
				m_IsThrow[3] = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CPuke_Ball::BallWorldDesc tDesc;
				tDesc.fDistance = m_pBeater->Get_Distance();
				XMStoreFloat3(&tDesc.vPos, XMLoadFloat4((_float4*)m_pBeater->Get_BoneMatrix().m[3]));
				tDesc.vDir.x = m_pBeater->Get_TargetDir().x;
				tDesc.vDir.y = m_pBeater->Get_TargetDir().y;
				tDesc.vDir.z = m_pBeater->Get_TargetDir().z;
				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), SCENE_STAGE05, TEXT("Layer_Ball"), &tDesc);
				RELEASE_INSTANCE(CGameInstance);
			}
			else if (40 < _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsThrow[4])
			{
				m_IsThrow[4] = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CPuke_Ball::BallWorldDesc tDesc;
				tDesc.fDistance = m_pBeater->Get_Distance();
				XMStoreFloat3(&tDesc.vPos, XMLoadFloat4((_float4*)m_pBeater->Get_BoneMatrix().m[3]));
				tDesc.vDir.x = m_pBeater->Get_TargetDir().x;
				tDesc.vDir.y = m_pBeater->Get_TargetDir().y;
				tDesc.vDir.z = m_pBeater->Get_TargetDir().z;
				pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), SCENE_STAGE05, TEXT("Layer_Ball"), &tDesc);
				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else if (4 == _pModel->Get_AnimIndex())
		{
			if (!m_IsDestroyEffect &&
				13 <= _pModel->Get_AnimationCurrentKeyFrame())
			{
				//이펙트 소멸해주세요
				m_IsDestroyEffect = true;
				
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Delete_GameObject(SCENE_STAGE05, TEXT("Layer_BossDount"), 0)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else if (5 == _pModel->Get_AnimIndex())//믹서기의 회전 공격입니다. AttackPoint 뼈위치에 토네이도 이펙트 달면 될 것 같습니다 소멸시점은 149번째 코드라인에서 잡아주시면됩니다.
		{
			if (!m_IsRenderEffect&&
				25<=_pModel->Get_AnimationCurrentKeyFrame())
			{
				//이펙트 생성해주세요
				m_IsRenderEffect = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BossDonut"), SCENE_STAGE05, TEXT("Layer_BossDount"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				RELEASE_INSTANCE(CGameInstance);

			}
		}
		else if (6 == _pModel->Get_AnimIndex())//믹서기의 회전 공격입니다. AttackPoint 뼈위치에 토네이도 이펙트 달면 될 것 같습니다 소멸시점은 156번째 코드라인에서 잡아주시면됩니다.
		{
			if (!m_IsRenderEffect)
			{
				//이펙트 생성해주세요
				m_IsRenderEffect = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				if (FAILED(pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_BossDonut"), SCENE_STAGE05, TEXT("Layer_BossDount"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

				RELEASE_INSTANCE(CGameInstance);
			 } 

			else if (!m_IsDestroyEffect &&
				292 <= _pModel->Get_AnimationCurrentKeyFrame())
			{
				m_IsDestroyEffect = true;
				//이펙트 소멸해주세요

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
	return false;
}

void CAttack_Beater::Reset_AttackState()
{
	m_dDelayTime = 0.0;
	m_IsDelayAttack = false;
	m_IsAttackLoop = false;
	m_IsReadyAttack = false;
	m_IsAttack = false;
	m_IsRecovery = false;
	m_IsRenderEffect = false;
	m_IsDestroyEffect = false;
	m_IsThrow[0] = false;
	m_IsThrow[1] = false;
	m_IsThrow[2] = false;
	m_IsThrow[3] = false;
	m_IsThrow[4] = false;
}

CAttack_Beater * CAttack_Beater::Create(CBeater * _pBeater)
{
	CAttack_Beater* pInstance = new CAttack_Beater(_pBeater);

	return pInstance;
}

void CAttack_Beater::Free()
{
}
