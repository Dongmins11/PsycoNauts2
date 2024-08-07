#include "stdafx.h"
#include "..\public\Attack_GoatO.h"
#include "Boss_Goat.h"
#include "Puke_Ball.h"

CAttack_GoatO::CAttack_GoatO(CGoatOtto * _pGoat)
	:m_pGoat(_pGoat)
{
}

_bool CAttack_GoatO::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pGoat->Get_Hp())
		return true;

	if (_pModel->Get_IsAnimFinishied(1) ||
		_pModel->Get_IsAnimFinishied(9))
	{
		m_dTelegraphLoopTime = 0.0;
		m_dStrikeLoopTime = 0.0;
		m_IsDelayAttack = true;
		m_IsAttackLoop = false;
		m_IsLoopFinished1 = false;
		m_IsLoopFinished2 = false;
		m_IsReadyAttack = false;
		m_IsAttack = false;
		m_IsRecovery = false;
		m_IsShoot[0] = false;
		m_IsShoot[1] = false;
		m_IsShoot[2] = false;
		m_IsShoot[3] = false;
		m_pGoat->Set_AttackLoop(false);
		if (1 == _pModel->Get_AnimIndex())
		{
			m_IsPukeDelay = true;
		}
		else if (9 == _pModel->Get_AnimIndex()) {
			_bool bArr[CBoss_Goat::BOSS::BOSS_END] = { false, false , false, false, false };
			m_pGoat->LockBosses(bArr);
		}
		return true;
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

	if (m_IsPukeDelay)
	{
		m_dPukeDelayTime += TimeDelta;
		if (10.0 < m_dPukeDelayTime)
		{
			m_dPukeDelayTime = 0.0;
			m_IsPukeDelay = false;
		}
	}
	if (!m_pGoat->IsPatternLock())
	{
		if (!m_IsAttackLoop &&
			!m_IsDelayAttack &&
			!m_IsPukeDelay &&
			!m_pGoat->IsLock())
		{
			m_pGoat->Set_AttackLoop(true);
			m_IsAttackLoop = true;
			m_iAttackType = rand() % 3;
		}
		else if (!m_IsAttackLoop &&
			!m_IsDelayAttack &&
			!m_IsPukeDelay&&
			m_pGoat->IsLock())
		{
			m_pGoat->Set_AttackLoop(true);
			m_IsAttackLoop = true;
			m_iAttackType = rand() % 2;
		}

	}
	if (5 == _pModel->Get_AnimIndex())
	{
		m_dTelegraphLoopTime += TimeDelta;
	}
	else if (3 == _pModel->Get_AnimIndex())
		m_dStrikeLoopTime += TimeDelta;

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
					_pModel->Set_Animation(11);
					m_IsReadyAttack = true;
				}
				else if (!m_IsAttack &&
					_pModel->Get_IsAnimFinishied(11))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(10);
					m_IsAttack = true;
				}
				else if (!m_IsRecovery&&
					_pModel->Get_IsAnimFinishied(10))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(9);
					m_IsRecovery = true;
				}
			}
			else if(2 == m_iAttackType)
			{
				if (!m_IsReadyAttack)
				{
					_bool bArr[CBoss_Goat::BOSS::BOSS_END] = { true, false , true, true, true };
					m_pGoat->LockBosses(bArr);
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(4);
					m_IsReadyAttack = true;
				}
				else if (_pModel->Get_IsAnimFinishied(4))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(5);
				}
				else if (1.0 <m_dTelegraphLoopTime &&
					!m_IsLoopFinished1)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(2);
					m_IsLoopFinished1 = true;
				}
				else if (_pModel->Get_IsAnimFinishied(2) &&
					!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(3);
					m_IsAttack = true;
				}
				else if (2.0 < m_dStrikeLoopTime &&
					!m_IsLoopFinished2 &&
					!m_IsRecovery)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(1);
					m_IsLoopFinished2 = true;
					m_IsRecovery = true;
				}
			}
			else
			{
				if (!m_IsReadyAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(11);
					m_IsReadyAttack = true;
				}
				else if (!m_IsAttack &&
					_pModel->Get_IsAnimFinishied(11))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(10);
					m_IsAttack = true;
				}
				else if (!m_IsRecovery&&
					_pModel->Get_IsAnimFinishied(10))
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(9);
					m_IsRecovery = true;
				}
			}
		}
		if (10 == _pModel->Get_AnimIndex())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPuke_Ball::BallWorldDesc tDesc;
			tDesc.fDistance = m_pGoat->Get_Distance();
			XMStoreFloat3(&tDesc.vPos, XMLoadFloat4((_float4*)m_pGoat->Get_BoneMatrix().m[3]));
			tDesc.vDir.x = m_pGoat->Get_TargetDir().x;
			tDesc.vDir.y = m_pGoat->Get_TargetDir().y;
			tDesc.vDir.z = m_pGoat->Get_TargetDir().z;
			pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), SCENE_STAGE05, TEXT("Layer_Ball"), &tDesc);

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PukeShoot"),
				SCENE_STATIC, TEXT("Effect_PukeShoot"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PukeShoot"),
				XMLoadFloat3(&tDesc.vPos) + _pTransform->Get_State(CTransform::STATE_LOOK) * 3.f, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			RELEASE_INSTANCE(CGameInstance);
		}
		else if (4 == _pModel->Get_AnimIndex())
		{
			if (!m_IsShoot[0] &&
				32 <= _pModel->Get_AnimationCurrentKeyFrame())
			{
				m_IsShoot[0] = true;
				//땅에 부딪히는 모션 연기이펙트 필요
				//MouthPoint나 형님이 따로 위치 잡아줘도 될듯?
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
					pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_SmokeEffect"),
					SCENE_STATIC, TEXT("Effect_Boss_SmokeEffectOtto"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, _pTransform, TEXT("Layer_Boss_SmokeEffect"))))
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
				pGameInstance->Setting_Effect_TransformPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_SmokeEffect"), TEXT("Com_Option"), CTransform::STATE_LOOK, 20.f);


				RELEASE_INSTANCE(CGameInstance);
			}
		}
		return false;
	}
	return false;
}

void CAttack_GoatO::Reset_AttackState()
{
	m_dDelayTime = 0.0;
	m_dPukeDelayTime = 0.0;
	m_dTelegraphLoopTime = 0.0;
	m_dStrikeLoopTime = 0.0;
	m_IsDelayAttack = false;
	m_IsAttackLoop = false;
	m_IsLoopFinished1 = false;
	m_IsLoopFinished2 = false;
	m_IsReadyAttack = false;
	m_IsAttack = false;
	m_IsRecovery = false;
	m_IsPukeDelay = false;
	m_IsShoot[0] = false;
	m_IsShoot[1] = false;
	m_IsShoot[2] = false;
	m_IsShoot[3] = false;
}

CAttack_GoatO * CAttack_GoatO::Create(CGoatOtto * _pGoat)
{
	CAttack_GoatO* pInstance = new CAttack_GoatO(_pGoat);

	return pInstance;
}

void CAttack_GoatO::Free()
{
}
