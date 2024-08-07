#include "stdafx.h"
#include "..\public\Skill_UI.h"
#include "GameInstance.h"

CSkill_UI::CSkill_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CSkill_UI::CSkill_UI(const CSkill_UI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CSkill_UI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_UI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_fSkillSize = 65.f;	// 스킬 사이즈
	m_fKeySize = 40.f;		// 키 버튼 사이즈
	m_fBGSize = 85.f;		// 스킬 배경 사이즈

	m_fX = g_iWinCX >> 1;
	m_fY = g_iWinCY >> 1;

	for (_uint i = 0; i < 6; i++)
	{
		m_bKeyCheck[i] = false;
		m_bRenderCheck[i] = true;
		m_fTimeAcc[i] = 0.f;
	}

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinCX, g_iWinCY, 0.f, 1.f));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
		case SCENE_STAGE01:
		{
			for (_uint i = 0; i < 6; i++)
			{
				m_bKeyCheck[i] = false;
				m_bRenderCheck[i] = false;
			}
		}
		break;
		case SCENE_STAGE02:
		{
			for (_uint i = 0; i < 6; i++)
			{
				if (0 <= i && 4 > i)
				{
					m_bKeyCheck[i] = false;
					m_bRenderCheck[i] = true;
				}
				else
				{
					m_bKeyCheck[i] = false;
					m_bRenderCheck[i] = false;
				}

			}
		}
		break;
		case SCENE_STAGE03:
		{
			for (_uint i = 0; i < 6; i++)
			{
				m_bKeyCheck[i] = false;
				m_bRenderCheck[i] = true;
			}
		}
		break;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CSkill_UI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	// Q Cool Time
	if (true == m_bKeyCheck[0])
	{
		m_fTimeAcc[0] = 1.f;
		m_bKeyCheck[0] = false;
	}

	if (false == m_bKeyCheck[0])
	{
		m_fTimeAcc[0] -= (_float)TimeDelta * 0.7f;
		if (0 >= m_fTimeAcc[0])
			m_fTimeAcc[0] = 0.f;
	}

	// E Cool Time
	if (true == m_bKeyCheck[1])
	{
		//m_fTimeAcc[1] += TimeDelta * 60.f;
		m_fTimeAcc[1] = 1.f;
		m_bKeyCheck[1] = false;
	}

	if (false == m_bKeyCheck[1])
	{
		m_fTimeAcc[1] -= (_float)TimeDelta * 1.5f;
		if (0 >= m_fTimeAcc[1])
			m_fTimeAcc[1] = 0.f;
	}

	// R Cool Time
	if (m_bKeyCheck[2])
		m_fTimeAcc[2] += (_float)TimeDelta / 5.f;

	if (1.f <= m_fTimeAcc[2] && m_bKeyCheck[2])
		m_fTimeAcc[2] = 1.f;

	if (!m_bKeyCheck[2])
	{
		m_fTimeAcc[2] -= (_float)TimeDelta * 0.7f;
		if (0 >= m_fTimeAcc[2])
			m_fTimeAcc[2] = 0.f;
	}

	// Rbutton Cool Time
	if (m_bKeyCheck[3])
		m_fTimeAcc[3] = 0.f;

	if (m_bKeyCheck[4])
		m_fTimeAcc[4] = 0.f;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing('R'))
		m_bKeyCheck[2] = true;
	else
		m_bKeyCheck[2] = false;

	if (pGameInstance->Key_Pressing(VK_RBUTTON))
		m_bKeyCheck[3] = true;
	else
		m_bKeyCheck[3] = false;

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int CSkill_UI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CSkill_UI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_UI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture_Skill */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Skill"), TEXT("Com_Texture_Skill"), (CComponent**)&m_pTextureSkillCom)))
		return E_FAIL;

	/* Com_Texture_Skill_BG */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Skill_BG"), TEXT("Com_Texture_Skill_BG"), (CComponent**)&m_pTextureBGCom)))
		return E_FAIL;

	/* Com_Texture_Key */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Key"), TEXT("Com_Texture_Key"), (CComponent**)&m_pTextureKeyCom)))
		return E_FAIL;

	/* Com_Model_Skill_LU_Q */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_LU_Q"), (CComponent**)&m_pModelSkillCom[0])))
		return E_FAIL;

	/* Com_Model_Skill_RU_E */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_RU_E"), (CComponent**)&m_pModelSkillCom[1])))
		return E_FAIL;

	/* Com_Model_Skill_LD_R */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_LD_R"), (CComponent**)&m_pModelSkillCom[2])))
		return E_FAIL;

	/* Com_Model_Skill_RD_RB */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_RD_RButton"), (CComponent**)&m_pModelSkillCom[3])))
		return E_FAIL;

	/* Com_Model_Skill_LT_H */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_LT_H"), (CComponent**)&m_pModelSkillCom[4])))
		return E_FAIL;

	/* Com_Model_Skill_RT_T */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_RT_T"), (CComponent**)&m_pModelSkillCom[5])))
		return E_FAIL;

	/* Com_Model_Skill_BG_LU_Q */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_BG_LU_Q"), (CComponent**)&m_pModelBGCom[0])))
		return E_FAIL;

	/* Com_Model_Skill_BG_LU_E */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_BG_RU_E"), (CComponent**)&m_pModelBGCom[1])))
		return E_FAIL;

	/* Com_Model_Skill_BG_LU_R */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_BG_LD_R"), (CComponent**)&m_pModelBGCom[2])))
		return E_FAIL;

	/* Com_Model_Skill_BG_LU_RB */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_BG_RD_RButton"), (CComponent**)&m_pModelBGCom[3])))
		return E_FAIL;

	/* Com_Model_Skill_BG_LT_H */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_BG_LT_H"), (CComponent**)&m_pModelBGCom[4])))
		return E_FAIL;

	/* Com_Model_Skill_BG_RT_T */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Skill_BG_RT_T"), (CComponent**)&m_pModelBGCom[5])))
		return E_FAIL;

	/* Com_Model_Key_Q */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Key_Q"), (CComponent**)&m_pModeyKeyCom[0])))
		return E_FAIL;

	/* Com_Model_Key_E */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Key_E"), (CComponent**)&m_pModeyKeyCom[1])))
		return E_FAIL;

	/* Com_Model_Key_R */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Key_R"), (CComponent**)&m_pModeyKeyCom[2])))
		return E_FAIL;

	/* Com_Model_Key_RButton */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Key_RButton"), (CComponent**)&m_pModeyKeyCom[3])))
		return E_FAIL;

	/* Com_Model_Key_H */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Key_H"), (CComponent**)&m_pModeyKeyCom[4])))
		return E_FAIL;

	/* Com_Model_Key_T */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Model_Key_T"), (CComponent**)&m_pModeyKeyCom[5])))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkill_UI::SetUp_ConstantTable()
{
	// Skill : LU
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSkillSize;
	WorldMatrix._22 = m_fSkillSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSkillSize / 2.f) - (g_iWinCX * 0.5f) + 24;
	WorldMatrix._42 = -(m_fSkillSize / 2.f) + (g_iWinCY * 0.5f) - 578;
	WorldMatrix._43 = 0.5f;

	_float4x4	ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSkillCom[0]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSkillCom[0]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSkillCom[0]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));


	m_pTextureSkillCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSkillCom[0], 2);
	if (true == m_bRenderCheck[0])
		m_pModelSkillCom[0]->Render(8);

	// Skill : RU
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSkillSize;
	WorldMatrix._22 = m_fSkillSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSkillSize / 2.f) - (g_iWinCX * 0.5f) + 1205;
	WorldMatrix._42 = -(m_fSkillSize / 2.f) + (g_iWinCY * 0.5f) - 578;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSkillCom[1]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSkillCom[1]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSkillCom[1]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureSkillCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSkillCom[1], 1);
	if (true == m_bRenderCheck[1])
		m_pModelSkillCom[1]->Render(8);

	// Skill : LD
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSkillSize;
	WorldMatrix._22 = m_fSkillSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSkillSize / 2.f) - (g_iWinCX * 0.5f) + 98;
	WorldMatrix._42 = -(m_fSkillSize / 2.f) + (g_iWinCY * 0.5f) - 642;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSkillCom[2]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSkillCom[2]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSkillCom[2]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureSkillCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSkillCom[2], 7);
	if (true == m_bRenderCheck[2])
		m_pModelSkillCom[2]->Render(8);

	// Skill : RD
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSkillSize;
	WorldMatrix._22 = m_fSkillSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSkillSize / 2.f) - (g_iWinCX * 0.5f) + 1113;
	WorldMatrix._42 = -(m_fSkillSize / 2.f) + (g_iWinCY * 0.5f) - 642;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSkillCom[3]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSkillCom[3]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSkillCom[3]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureSkillCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSkillCom[3], 3);
	if (true == m_bRenderCheck[3])
		m_pModelSkillCom[3]->Render(8);

	// Skill : LT
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSkillSize;
	WorldMatrix._22 = m_fSkillSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSkillSize / 2.f) - (g_iWinCX * 0.5f) + 96;
	WorldMatrix._42 = -(m_fSkillSize / 2.f) + (g_iWinCY * 0.5f) - 500;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSkillCom[4]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSkillCom[4]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSkillCom[4]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureSkillCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSkillCom[4], 0);
	if (true == m_bRenderCheck[4])
		m_pModelSkillCom[4]->Render(8);

	// Skill : RT
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fSkillSize;
	WorldMatrix._22 = m_fSkillSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fSkillSize / 2.f) - (g_iWinCX * 0.5f) + 1111;
	WorldMatrix._42 = -(m_fSkillSize / 2.f) + (g_iWinCY * 0.5f) - 500;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelSkillCom[5]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelSkillCom[5]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelSkillCom[5]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureSkillCom->SetUp_OnShader("g_DiffuseTexture", m_pModelSkillCom[5], 5);
	if (true == m_bRenderCheck[5])
		m_pModelSkillCom[5]->Render(8);

	// Key : Q
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fKeySize;
	WorldMatrix._22 = m_fKeySize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fKeySize / 2.f) - (g_iWinCX * 0.5f) + 120;
	WorldMatrix._42 = -(m_fKeySize / 2.f) + (g_iWinCY * 0.5f) - 580;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModeyKeyCom[0]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[0]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[0]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureKeyCom->SetUp_OnShader("g_DiffuseTexture", m_pModeyKeyCom[0], 4);
	if (true == m_bRenderCheck[0])
		m_pModeyKeyCom[0]->Render(11);


	// Key : E
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fKeySize;
	WorldMatrix._22 = m_fKeySize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fKeySize / 2.f) - (g_iWinCX * 0.5f) + 1140;
	WorldMatrix._42 = -(m_fKeySize / 2.f) + (g_iWinCY * 0.5f) - 580;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModeyKeyCom[1]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[1]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[1]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureKeyCom->SetUp_OnShader("g_DiffuseTexture", m_pModeyKeyCom[1], 0);
	if (true == m_bRenderCheck[1])
		m_pModeyKeyCom[1]->Render(11);

	// Key : R
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fKeySize;
	WorldMatrix._22 = m_fKeySize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fKeySize / 2.f) - (g_iWinCX * 0.5f) + 200;
	WorldMatrix._42 = -(m_fKeySize / 2.f) + (g_iWinCY * 0.5f) - 660;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModeyKeyCom[2]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[2]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[2]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureKeyCom->SetUp_OnShader("g_DiffuseTexture", m_pModeyKeyCom[2], 6);
	if (true == m_bRenderCheck[2])
		m_pModeyKeyCom[2]->Render(11);

	// Key : RButton
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fKeySize;
	WorldMatrix._22 = m_fKeySize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fKeySize / 2.f) - (g_iWinCX * 0.5f) + 1050;
	WorldMatrix._42 = -(m_fKeySize / 2.f) + (g_iWinCY * 0.5f) - 660;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModeyKeyCom[3]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[3]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[3]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureKeyCom->SetUp_OnShader("g_DiffuseTexture", m_pModeyKeyCom[3], 3);
	if (true == m_bRenderCheck[3])
		m_pModeyKeyCom[3]->Render(11);

	// Key : H
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fKeySize;
	WorldMatrix._22 = m_fKeySize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fKeySize / 2.f) - (g_iWinCX * 0.5f) + 200;
	WorldMatrix._42 = -(m_fKeySize / 2.f) + (g_iWinCY * 0.5f) - 520;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModeyKeyCom[4]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[4]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[4]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureKeyCom->SetUp_OnShader("g_DiffuseTexture", m_pModeyKeyCom[4], 1);
	if (true == m_bRenderCheck[4])
		m_pModeyKeyCom[4]->Render(11);

	// Key : T
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fKeySize;
	WorldMatrix._22 = m_fKeySize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fKeySize / 2.f) - (g_iWinCX * 0.5f) + 1050;
	WorldMatrix._42 = -(m_fKeySize / 2.f) + (g_iWinCY * 0.5f) - 520;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModeyKeyCom[5]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[5]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModeyKeyCom[5]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureKeyCom->SetUp_OnShader("g_DiffuseTexture", m_pModeyKeyCom[5], 5);
	if (true == m_bRenderCheck[5])	
		m_pModeyKeyCom[5]->Render(11);

	// BG : LU
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fBGSize;
	WorldMatrix._22 = m_fBGSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fBGSize / 2.f) - (g_iWinCX * 0.5f) + 14;
	WorldMatrix._42 = -(m_fBGSize / 2.f) + (g_iWinCY * 0.5f) - 570;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom[0]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom[0]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom[0]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	_float fTimeRatio = _float(m_fTimeAcc[2] / 1.f);
	m_pModelBGCom[0]->Set_RawValue("g_fTimeAcc", &fTimeRatio, sizeof(_float));

	m_pTextureBGCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom[0], 0);
	m_pTextureBGCom->SetUp_OnShader("g_MixTexture", m_pModelBGCom[0], 1);
	if (true == m_bRenderCheck[0])
		m_pModelBGCom[0]->Render(12);

	// BG : RU
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fBGSize;
	WorldMatrix._22 = m_fBGSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fBGSize / 2.f) - (g_iWinCX * 0.5f) + 1195;
	WorldMatrix._42 = -(m_fBGSize / 2.f) + (g_iWinCY * 0.5f) - 570;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom[1]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom[1]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom[1]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	fTimeRatio = _float(m_fTimeAcc[1] / 1.f);
	m_pModelBGCom[1]->Set_RawValue("g_fTimeAcc", &fTimeRatio, sizeof(_float));

	m_pTextureBGCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom[1], 0);
	m_pTextureBGCom->SetUp_OnShader("g_MixTexture", m_pModelBGCom[1], 3);
	if (true == m_bRenderCheck[1])
		m_pModelBGCom[1]->Render(12);

	// BG : LD
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fBGSize;
	WorldMatrix._22 = m_fBGSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fBGSize / 2.f) - (g_iWinCX * 0.5f) + 85;
	WorldMatrix._42 = -(m_fBGSize / 2.f) + (g_iWinCY * 0.5f) - 635;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom[2]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom[2]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom[2]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	fTimeRatio = _float(m_fTimeAcc[4] / 1.f);
	m_pModelBGCom[2]->Set_RawValue("g_fTimeAcc", &fTimeRatio, sizeof(_float));

	m_pTextureBGCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom[2], 0);
	m_pTextureBGCom->SetUp_OnShader("g_MixTexture", m_pModelBGCom[2], 1);
	if (true == m_bRenderCheck[2])
		m_pModelBGCom[2]->Render(12);

	// BG : RD
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fBGSize;
	WorldMatrix._22 = m_fBGSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fBGSize / 2.f) - (g_iWinCX * 0.5f) + 1100;
	WorldMatrix._42 = -(m_fBGSize / 2.f) + (g_iWinCY * 0.5f) - 635;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom[3]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom[3]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom[3]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	fTimeRatio = _float(m_fTimeAcc[3] / 1.f);
	m_pModelBGCom[3]->Set_RawValue("g_fTimeAcc", &fTimeRatio, sizeof(_float));

	m_pTextureBGCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom[3], 0);
	m_pTextureBGCom->SetUp_OnShader("g_MixTexture", m_pModelBGCom[3], 2);
	if (true == m_bRenderCheck[3])
		m_pModelBGCom[3]->Render(12);

	// BG : LT
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fBGSize;
	WorldMatrix._22 = m_fBGSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fBGSize / 2.f) - (g_iWinCX * 0.5f) + 85;
	WorldMatrix._42 = -(m_fBGSize / 2.f) + (g_iWinCY * 0.5f) - 490;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom[4]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom[4]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom[4]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	fTimeRatio = _float(m_fTimeAcc[0] / 1.f);
	m_pModelBGCom[4]->Set_RawValue("g_fTimeAcc", &fTimeRatio, sizeof(_float));

	m_pTextureBGCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom[4], 0);
	m_pTextureBGCom->SetUp_OnShader("g_MixTexture", m_pModelBGCom[4], 1);
	if (true == m_bRenderCheck[4])
		m_pModelBGCom[4]->Render(12);

	// BG : RT
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	WorldMatrix._11 = m_fBGSize;
	WorldMatrix._22 = m_fBGSize;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fBGSize / 2.f) - (g_iWinCX * 0.5f) + 1100;
	WorldMatrix._42 = -(m_fBGSize / 2.f) + (g_iWinCY * 0.5f) - 490;
	WorldMatrix._43 = 0.5f;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	m_pModelBGCom[5]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelBGCom[5]->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelBGCom[5]->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	fTimeRatio = _float(m_fTimeAcc[5] / 1.f);
	m_pModelBGCom[5]->Set_RawValue("g_fTimeAcc", &fTimeRatio, sizeof(_float));

	m_pTextureBGCom->SetUp_OnShader("g_DiffuseTexture", m_pModelBGCom[5], 0);
	m_pTextureBGCom->SetUp_OnShader("g_MixTexture", m_pModelBGCom[5], 1);
	if (true == m_bRenderCheck[5])
		m_pModelBGCom[5]->Render(12);

	return S_OK;
}

CSkill_UI * CSkill_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSkill_UI*	pInstance = new CSkill_UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CSkill_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkill_UI::Clone(void * pArg, _uint iCurrentScene)
{
	CSkill_UI*	pInstance = new CSkill_UI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CSkill_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_UI::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureSkillCom);
	Safe_Release(m_pTextureBGCom);
	Safe_Release(m_pTextureKeyCom);

	for (_uint i = 0; i < 6; i++)
	{
		Safe_Release(m_pModelSkillCom[i]);
		Safe_Release(m_pModelBGCom[i]);
		Safe_Release(m_pModeyKeyCom[i]);
	}
}
