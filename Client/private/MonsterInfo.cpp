#include "stdafx.h"
#include "MonsterInfo.h"
#include "GameInstance.h"

CMonsterInfo::CMonsterInfo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CMonsterInfo::CMonsterInfo(const CMonsterInfo & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterInfo::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterInfo::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		MONSTERINFO tagInfo;
		ZeroMemory(&tagInfo, sizeof(MONSTERINFO));

		tagInfo = *(MONSTERINFO*)pArg;
		m_iNumTexture = tagInfo.iNumTextrue;
		m_IsCheckSpawn = tagInfo.IsSpawn;
	}

	m_fLogoSizeX = 400;
	m_fLogoSizeY = 600;

	m_fX[TYPE_BASE] = 1050.f;
	m_fY[TYPE_BASE] = 400.f;

	m_fX[TYPE_SHADOW] = 950.f;
	m_fY[TYPE_SHADOW] = 500.f;

	m_fAngle[ROTATE_BASE] = 0.f;
	m_fAngle[ROTATE_SHADOW] = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 10.f));

	return S_OK;
}

_int CMonsterInfo::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	
	if (m_fAlpha < 1.0)
	{
		if(false == *m_IsCheckSpawn)
			m_fAlpha += (_float)TimeDelta;
	}

	if (!m_IsScaling[SCALE_DOWN])
	{
		if (m_fX[TYPE_BASE] > 1000.f)
		{
			m_fLogoSizeX -= (_float)TimeDelta * 100;
			m_fLogoSizeY -= (_float)TimeDelta * 200;
			m_fX[TYPE_BASE] -= (_float)TimeDelta*100.f;
		}
		else
		{
			m_fX[TYPE_BASE] = 1000.f;
			m_IsScaling[SCALE_DOWN] = true;
		}

		if (m_fX[TYPE_SHADOW] < 990.f)
		{
			m_fX[TYPE_SHADOW] += (_float)TimeDelta*200.f;
		}
		else
		{
			m_fX[TYPE_SHADOW] = 990.f;
		}

		if (m_fY[TYPE_BASE] < 550.f)
			m_fY[TYPE_BASE] += (_float)TimeDelta*500.f;
		else
		{
			m_fY[TYPE_BASE] = 550.f;
		}

		if (m_fY[TYPE_SHADOW] > 550.f)
			m_fY[TYPE_SHADOW] -= (_float)TimeDelta*500.f;
		else
		{
			m_fY[TYPE_SHADOW] = 550.f;
		}
	}
	else
	{
		if (!m_IsScaling[SCALE_UP])
		{
			if (m_fLogoSizeX < 400.f)
			{
				m_fLogoSizeX += (_float)TimeDelta * 50.f;
				m_fLogoSizeY += (_float)TimeDelta * 100.f;
			}
			else
			{
				m_IsScaling[SCALE_UP] = true;
				m_fLogoSizeX = 400.f;
				m_fLogoSizeY = 600.f;
			}
		}
	}


	if (m_IsScaling[SCALE_DOWN])
	{
		if (!m_IsRotation)
		{
			if (5.f > m_fAngle[ROTATE_SHADOW])
				m_fAngle[ROTATE_SHADOW] += (_float)TimeDelta*20.f;
			else
			{
				m_fAngle[ROTATE_SHADOW] = 5.f;
				m_IsRotation = true;
			}

			if (5.f > m_fAngle[ROTATE_BASE])
				m_fAngle[ROTATE_BASE] += (_float)TimeDelta*20.f;
			else
				m_fAngle[ROTATE_BASE] = 5.f;
		}
	}

	if (m_IsScaling[SCALE_UP])
	{
		if (m_IsRotation&&
			!m_IsCheckSpawn)
		{
			if (3.f < m_fAngle[ROTATE_SHADOW])
			{
				m_fAngle[ROTATE_SHADOW] -= (_float)TimeDelta*5.f;
				m_fX[TYPE_SHADOW] += (_float)TimeDelta*20.f;
			}
			else
			{
				m_fAngle[ROTATE_SHADOW] = 3.f;
			}
		}
	}

	if (true == *m_IsCheckSpawn)
	{
		m_fY[TYPE_BASE] += (_float)TimeDelta*100.f;
		m_fY[TYPE_SHADOW] += (_float)TimeDelta*100.f;
		if (600 < m_fY[TYPE_BASE])
			m_fAlpha += (_float)TimeDelta*-2.f;
		
		if (0.f > m_fAlpha)
			m_isDestroy = true;
	}


	return _int();
}

_int CMonsterInfo::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMonsterInfo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTableShadowTexture()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTableBaseTexture()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterInfo::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture_Title */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_MonsterInfo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterInfo::SetUp_ConstantTableShadowTexture()
{
	if (nullptr == m_pModelCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fLogoSizeX;
	WorldMatrix._22 = m_fLogoSizeY;
	WorldMatrix._41 = m_fX[TYPE_SHADOW] - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY[TYPE_SHADOW] + (g_iWinCY * 0.5f) - 40.f;
	WorldMatrix._43 = 9.f;

	_matrix		FinalMatrix;
	_matrix		RotationMatrix;
	FinalMatrix = XMLoadFloat4x4(&WorldMatrix);
	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(m_fAngle[ROTATE_SHADOW]));
	FinalMatrix *= RotationMatrix;
	XMStoreFloat4x4(&WorldMatrix, FinalMatrix);

	m_pModelCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelCom, 0);

	m_pModelCom->Render(34);

	return S_OK;
}

HRESULT CMonsterInfo::SetUp_ConstantTableBaseTexture()
{
	if (nullptr == m_pModelCom || nullptr == m_pTextureCom)
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	WorldMatrix._11 = m_fLogoSizeX;
	WorldMatrix._22 = m_fLogoSizeY;
	WorldMatrix._41 = m_fX[TYPE_BASE] - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -m_fY[TYPE_BASE] + (g_iWinCY * 0.5f) - 40.f;
	WorldMatrix._43 = 9.5f;

	_matrix		FinalMatrix;
	_matrix		RotationMatrix;
	FinalMatrix = XMLoadFloat4x4(&WorldMatrix);
	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(m_fAngle[ROTATE_BASE]));
	FinalMatrix *= RotationMatrix;
	XMStoreFloat4x4(&WorldMatrix, FinalMatrix);

	m_pModelCom->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pModelCom, m_iNumTexture);

	m_pModelCom->Render(34);

	return S_OK;
}

CMonsterInfo * CMonsterInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMonsterInfo*	pInstance = new CMonsterInfo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CMonsterInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterInfo::Clone(void * pArg, _uint iCurrentScene)
{
	CMonsterInfo*	pInstance = new CMonsterInfo(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CMonsterInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterInfo::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);

}
