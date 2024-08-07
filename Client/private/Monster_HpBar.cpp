#include "stdafx.h"
#include "..\public\Monster_HpBar.h"
#include "GameInstance.h"

CMonster_HpBar::CMonster_HpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CMonster_HpBar::CMonster_HpBar(const CMonster_HpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster_HpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMonster_HpBar::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		ZeroMemory(&m_tMonsterHpBarInfo, sizeof(MONSTERHPBARINFO));
		m_tMonsterHpBarInfo = *(MONSTERHPBARINFO*)pArg;
	}

	m_iCurrentHp = *m_tMonsterHpBarInfo.m_pParentHp;
	m_iDelayHp = m_iCurrentHp;

	return S_OK;
}

_int CMonster_HpBar::Tick(_double TimeDelta)
{
	if (!m_isRender)
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTransformCom->Set_WorldMatrix(*m_tMonsterHpBarInfo.m_pParentMatrix);
	_float4x4 WorldMatrix;
	_matrix	XMWorldMatrix;
	XMWorldMatrix = m_pTransformCom->Get_BillBoardMatrix(pGameInstance->Get_CamPosition());
	XMWorldMatrix.r[1] *= 0.18f;
	XMStoreFloat4x4(&WorldMatrix, XMWorldMatrix);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	RELEASE_INSTANCE(CGameInstance);

	m_iCurrentHp = *m_tMonsterHpBarInfo.m_pParentHp;

	if (m_iCurrentHp != m_iDelayHp)
		m_iDelayHp--;

	return _int();
}

_int CMonster_HpBar::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return 0;
}

HRESULT CMonster_HpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	if (0 < m_iCurrentHp&&
		100>m_iCurrentHp)
	{

		if (FAILED(SetUp_ConstantTableForBG()))
			return E_FAIL;

		m_pVIBufferCom->Render(29);

		if (FAILED(SetUp_ConstantTableForDELAY()))
			return E_FAIL;

		m_pVIBufferCom->Render(29);

		if (FAILED(SetUp_ConstantTableForFILL()))
			return E_FAIL;

		m_pVIBufferCom->Render(29);
	}

	return S_OK;
}

HRESULT CMonster_HpBar::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_MonsterHpBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_HpBar::SetUp_ConstantTableForBG()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));

	_float	fUVPercent = 100.f;
	m_pVIBufferCom->Set_RawValue("g_fUVPercent", &fUVPercent, sizeof(_float));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 0);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMonster_HpBar::SetUp_ConstantTableForDELAY()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	_matrix			XMWorldMatrix;
	XMWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	XMWorldMatrix.r[3] += XMWorldMatrix.r[2] * 0.0001f;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMWorldMatrix));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));
	_float	fUVPercent = (_float)(m_iDelayHp);

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_fUVPercent", &fUVPercent, sizeof(_float));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 1);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMonster_HpBar::SetUp_ConstantTableForFILL()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	_matrix			XMWorldMatrix;
	XMWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	XMWorldMatrix.r[3] += XMWorldMatrix.r[2] * 0.0002f;
	XMStoreFloat4x4(&WorldMatrix,XMMatrixTranspose(XMWorldMatrix));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));
	_float	fUVPercent = (_float)(m_iCurrentHp);

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_fUVPercent", &fUVPercent, sizeof(_float));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 2);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMonster_HpBar * CMonster_HpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMonster_HpBar*	pInstance = new CMonster_HpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonster_HpBar::Clone(void * pArg, _uint iCurrentScene)
{
	CMonster_HpBar*	pInstance = new CMonster_HpBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster_HpBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
