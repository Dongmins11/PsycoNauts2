#include "stdafx.h"
#include "..\public\Attack_Spatula.h"

CAttack_Spatula::CAttack_Spatula(CSpatula * _pSpatula)
	:m_pSpatula(_pSpatula)
{
}

_bool CAttack_Spatula::Invoke(_double TimeDelta, CModel * _pModel, CTransform * _pTransform, CCharacterController* _pController)
{
	if (0 >= m_pSpatula->Get_Hp())
		return true;

	if (m_IsDelayAttack)
	{
		m_dDelayTime += TimeDelta;
		if (4.0 < m_dDelayTime)
		{
			m_IsDelayAttack = false;
			m_dDelayTime = 0.0;
		}
	}

	if (_pModel->Get_IsAnimFinishied(0) ||
		_pModel->Get_IsAnimFinishied(1))
	{
		m_IsDelayAttack = true;
		m_IsAttackLoop = false;
		m_IsAttack = false;
		m_pSpatula->Set_AttackLoop(false);
		m_IsRenderEffect[0] = false;
		m_IsRenderEffect[1] = false;
		m_IsRenderEffect[2] = false;
		return true;
	}

	if (!m_IsAttackLoop&&
		m_pSpatula->IsLock()||
		m_pSpatula->IsPatternLock())
		return true;

	if (!m_IsAttackLoop &&
		!m_IsDelayAttack)
	{
		m_pSpatula->Set_AttackLoop(true);
		m_IsAttackLoop = true;
		m_iAttackType = rand() % 2;
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
				if (!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(0);
					m_IsAttack = true;
				}
			}
			else if (1 == m_iAttackType)
			{
				
				if (!m_IsAttack)
				{
					_pModel->Set_IsAnimLefp(true);
					_pModel->Set_Animation(1);
					m_IsAttack = true;
				}
			}
		}

		if (0 == _pModel->Get_AnimIndex())//뒤집개 내려찍는 공격입니다 총 3번에 걸쳐서 내려찍습니다
		{
			if (16 <= _pModel->Get_AnimationCurrentKeyFrame()&&
				!m_IsRenderEffect[0])
			{
				m_IsRenderEffect[0] = true;
				//뼈이름은 AttackPoint입니다
				if (FAILED(Create_Effect(TEXT("Effect_Boss_SmokeEffectSpecula0"), _pModel, _pTransform)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}

			}
			else if (24 <= _pModel->Get_AnimationCurrentKeyFrame() && 
				!m_IsRenderEffect[1])
			{
				m_IsRenderEffect[1 ] = true;
				//뼈이름은 AttackPoint입니다
				if (FAILED(Create_Effect(TEXT("Effect_Boss_SmokeEffectSpecula1"), _pModel, _pTransform)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
			}
			else if (35 <= _pModel->Get_AnimationCurrentKeyFrame() && \
				!m_IsRenderEffect[2])
			{
				m_IsRenderEffect[2] = true;
				//뼈이름은 AttackPoint입니다
				if (FAILED(Create_Effect(TEXT("Effect_Boss_SmokeEffectSpecula2"), _pModel, _pTransform)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
			}
		}
		else if (1 == _pModel->Get_AnimIndex()) //내려찍은후 앞으로 내지르는 공격입니다.
		{
			if (47 <= _pModel->Get_AnimationCurrentKeyFrame() &&
				!m_IsRenderEffect[0])
			{
				m_IsRenderEffect[0] = true;
				//뼈이름은 AttackPoint입니다
				if (FAILED(Create_Effect(TEXT("Effect_Boss_SmokeEffectSpecula0"), _pModel, _pTransform)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return false;
				}
			}
		}
		return false;
	}
	return false;
}

void CAttack_Spatula::Reset_AttackState()
{
	m_dDelayTime = 0.0;
	m_iAttackType = 0;
	m_IsDelayAttack = false;
	m_IsAttackLoop = false;
	m_IsAttack = false;
	m_IsRenderEffect[0] = false;
	m_IsRenderEffect[1] = false;
	m_IsRenderEffect[2] = false;
}

HRESULT CAttack_Spatula::Create_Effect(const _tchar* pEffectTag, CModel* _pModel, CTransform* _pTransform)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_SmokeEffect"),
		SCENE_STATIC, pEffectTag)))
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
	return S_OK;
}

CAttack_Spatula * CAttack_Spatula::Create(CSpatula * _pSpatula)
{
	CAttack_Spatula* pInstance = new CAttack_Spatula(_pSpatula);

	return pInstance;
}

void CAttack_Spatula::Free()
{
}
