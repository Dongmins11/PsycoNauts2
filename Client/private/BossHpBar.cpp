#include "stdafx.h"
#include "..\public\BossHpBar.h"
#include "GameInstance.h"

CBossHpBar::CBossHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBossHpBar::CBossHpBar(const CBossHpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossHpBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBossHpBar::NativeConstruct(void * pArg, _uint iCurrentScene)
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
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 10.f));

	m_fLogoSizeX = 500;
	m_fLogoSizeY = 38;

	m_fX = 640.f;
	m_fY = 50.f;
	
	return S_OK;
}

_int CBossHpBar::Tick(_double TimeDelta)
{
	if (!m_isRender)
		return 0;

	m_iCurrentHp = *m_tMonsterHpBarInfo.m_pParentHp;

	if (m_iCurrentHp != m_iDelayHp)
		m_iDelayHp--;

	return _int();
}

_int CBossHpBar::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return 0;
}

HRESULT CBossHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (0 < m_iCurrentHp &&
		100 >= m_iCurrentHp)
	{

		if (FAILED(SetUp_ConstantTableForBG()))
			return E_FAIL;

		m_pVIBufferCom->Render(39);

		if (FAILED(SetUp_ConstantTableForDELAY()))
			return E_FAIL;

		m_pVIBufferCom->Render(39);

		if (FAILED(SetUp_ConstantTableForFILL()))
			return E_FAIL;

		m_pVIBufferCom->Render(39);
	}

	return S_OK;
}

HRESULT CBossHpBar::SetUp_Component()
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

HRESULT CBossHpBar::SetUp_ConstantTableForBG()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fLogoSizeX;
	WorldMatrix._22 = m_fLogoSizeY;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 9.f;

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));

	_float	fUVPercent = 100.f;
	m_pVIBufferCom->Set_RawValue("g_fUVPercent", &fUVPercent, sizeof(_float));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 0);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBossHpBar::SetUp_ConstantTableForDELAY()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fLogoSizeX;
	WorldMatrix._22 = m_fLogoSizeY;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 8.f;

	_float	fUVPercent = (_float)(m_iDelayHp);
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));

	m_pVIBufferCom->Set_RawValue("g_fUVPercent", &fUVPercent, sizeof(_float));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 1);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBossHpBar::SetUp_ConstantTableForFILL()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fLogoSizeX;
	WorldMatrix._22 = m_fLogoSizeY;
	WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	WorldMatrix._43 = 7.f;

	_float	fUVPercent = (_float)(m_iCurrentHp);

	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_fUVPercent", &fUVPercent, sizeof(_float));
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, 2);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBossHpBar * CBossHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBossHpBar*	pInstance = new CBossHpBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBossHpBar::Clone(void * pArg, _uint iCurrentScene)
{
	CBossHpBar*	pInstance = new CBossHpBar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossHpBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
