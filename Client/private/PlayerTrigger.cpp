#include "stdafx.h"
#include "..\public\PlayerTrigger.h"
#include "Transform.h"
#include "PhysXCollider.h"

CPlayerTrigger::CPlayerTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPlayerTrigger::CPlayerTrigger(const CPlayerTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerTrigger::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerTrigger::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	return S_OK;
}

_int CPlayerTrigger::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;



	return _int();
}

_int CPlayerTrigger::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	return _int();
}

HRESULT CPlayerTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerTrigger::OnTriggerEnter(CCollisionObject & collision)
{
}

void CPlayerTrigger::OnTriggerStay(CCollisionObject & collision)
{
}

void CPlayerTrigger::OnTriggerExit(CCollisionObject & collision)
{
}

void CPlayerTrigger::OnControllerCollisionHit(CCollisionObject & collision)
{
}


HRESULT CPlayerTrigger::SetUp_Component(_uint iSceneIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	return __super::SetUp_Component(iSceneIndex, pPrototypeTag, pComponentTag, ppOut, pArg);
}

CPlayerTrigger * CPlayerTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerTrigger*	pInstance = new CPlayerTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayerTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayerTrigger::Clone(void * pArg, _uint iCurrentScene = 0)
{
	CPlayerTrigger*	pInstance = new CPlayerTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayerTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerTrigger::Free()
{
	__super::Free();
}
