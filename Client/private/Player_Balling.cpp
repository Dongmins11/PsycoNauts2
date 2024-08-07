#include "stdafx.h"
#include "..\public\Player_Balling.h"
#include "GameInstance.h"


CPlayer_Balling::CPlayer_Balling(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer_Balling::CPlayer_Balling(const CPlayer_Balling & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_Balling::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Balling::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_isRender = false;

	return S_OK;
}

_int CPlayer_Balling::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;


	return _int();
}

_int CPlayer_Balling::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);


	return _int();
}

HRESULT CPlayer_Balling::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(0);

	return S_OK;
}

HRESULT CPlayer_Balling::SetUp_Component()
{
	return S_OK;
}

HRESULT CPlayer_Balling::SetUp_ConstantTable()
{
	return S_OK;
}

CPlayer_Balling * CPlayer_Balling::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer_Balling*	pInstance = new CPlayer_Balling(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayer_Balling");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Balling::Clone(void * pArg, _uint iSceneIndex)
{
	CPlayer_Balling*	pInstance = new CPlayer_Balling(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayer_Balling");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Balling::Free()
{
	__super::Free();

	Safe_Release(m_pCharacterController);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
