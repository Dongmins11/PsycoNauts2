#include "stdafx.h"
#include "..\public\QuestUI.h"
#include "GameInstance.h"
#include "QuestManager.h"
CQuestUI::CQuestUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CQuestUI::CQuestUI(const CQuestUI & rhs)
	:CGameObject(rhs)
{
}

HRESULT CQuestUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestUI::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	ORTHTEXUTREDESC tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(ORTHTEXUTREDESC));
	
	memcpy(&tTextureDesc, pArg, sizeof(ORTHTEXUTREDESC));
	
	if (FAILED(SetUp_Component()))
		return E_FAIL;
	m_fSizeX = (_float)tTextureDesc.iSize.x;
	m_fSizeY = (_float)tTextureDesc.iSize.y;
	
	m_fX = (_float)tTextureDesc.iPos.x;
	m_fY = (_float)tTextureDesc.iPos.y;
	m_iRenderPass = tTextureDesc.iPassNum;
	m_iCurrentImgIndex = tTextureDesc.iImgNum;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	m_isRender = true;
	m_eCurState = UISTATE_START;
	//m_iRenderPass = 1;
	return S_OK;
}

_int CQuestUI::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	Update_UIState();
	return _int();
}

_int CQuestUI::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	Update_UIPosY(TimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CQuestUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

void CQuestUI::WaitingForMoveX()
{
}

HRESULT CQuestUI::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Quest_Desc"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CQuestUI::SetUp_ConstantTable()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 배경그리기
	//m_fSizeX = 726.f;
	//m_fSizeY = 180.f;
	//m_fX = 324.f;
	//m_fY = 496.f;
	
	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._33 = 1.f;
	WorldMatrix._41 = (m_fX + m_fSizeX *0.5f) - (g_iWinCX * 0.5f);
	WorldMatrix._42 = -(m_fY + m_fSizeY * 0.5f) + (g_iWinCY * 0.5f);
	
	_float4x4	ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));
	
	m_pVIBufferCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBufferCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	
	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBufferCom, m_iRenderPass);
	
	m_pVIBufferCom->Render(11);
	
	
	//_float4x4	ViewMatrix, ProjMatrix;
	// 배경그리기
	//m_fSizeX = 478.f;
	//m_fSizeY = 90.f;
	//m_fX = 428.f;
	//m_fY = 550.f;
	return S_OK;
}

void CQuestUI::Update_UIState()
{
	if (m_eCurState != m_ePreState)
	{
		if (m_eCurState > m_ePreState)
		{
			m_isAnimatedUp = true;
			m_isAnimatedDown = false;
		}
		else if (m_eCurState < m_ePreState)
		{
			m_isAnimatedUp = false;
			m_isAnimatedDown = true;
		}

		switch (m_eCurState)
		{
			case Client::CQuestUI::UISTATE_START:
			{
				m_isAnimatedUp = false;
				m_isAnimatedDown = false;

				m_fY = m_fYPosArr[UISTATE_START];
				break;
			}
			case Client::CQuestUI::UISTATE_IDLE:
			{
				m_isIdle = true;
				m_isSelected = false;
				m_isTapping = false;
				break;
			}
			case Client::CQuestUI::UISTATE_SELECTED:
			{
				m_isSelected = true;
				m_isIdle = false;
				m_isTapping = false;
				break;
			}
			case Client::CQuestUI::UISTATE_TAB:
			{
				m_isTapping = true;
				m_isSelected = true;
				m_isIdle = false;
				break;
			}
			case Client::CQuestUI::UISTATE_END:
			{
				m_isTapping = false;
				m_isSelected = false;
				m_isIdle = false;
				break;
			}
		}

		m_ePreState = m_eCurState;
	}
}

void CQuestUI::Update_UIPosY(_double TimeDelta)
{
	_float fFinalPos;

	if (m_isFinished)
	{
		fFinalPos = m_fYPosArr[UISTATE_START];
		m_isAnimatedUp = false;
		m_isAnimatedDown = true;
	}
	else if (m_isIdle)
	{
		fFinalPos = m_fYPosArr[UISTATE_IDLE];
	}
	else if (m_isSelected && !m_isTapping)
	{	
		fFinalPos = m_fYPosArr[UISTATE_SELECTED];
	}
	else if (m_isSelected && m_isTapping)
	{
		fFinalPos = m_fYPosArr[UISTATE_TAB];
	}
	else
		return;

	if (m_isAnimatedUp)
	{
		if (m_fY <= fFinalPos)
		{
			m_fY = fFinalPos;
			m_isAnimatedUp = false;
			return;
		}

		m_fY -= 50.f* TimeDelta;
	}
	else if (m_isAnimatedDown)
	{
		if (m_fY >= fFinalPos)
		{
			m_fY = fFinalPos;
			m_isAnimatedDown = false;
			if (m_isFinished && !m_isMoveX)
			{
				m_eCurState = UISTATE_START;
				m_isFinished = false;
				CQuestManager::Get_Instance()->Sorting_Quest();
				CQuestManager::Get_Instance()->SettingQuestUI(m_iCurrentImgIndex);
			}
			else if (m_isFinished && m_isMoveX)
			{
				m_eCurState = UISTATE_IDLE;
				m_isMoveX = false;
				m_isFinished = false;
				CQuestManager::Get_Instance()->SettingQuestUI(m_iCurrentImgIndex);
			}
			return;
		}

		m_fY += 50.f *TimeDelta;
	}
}


CQuestUI * CQuestUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CQuestUI*	pInstance = new CQuestUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CQuestUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CQuestUI::Clone(void * pArg, _uint iCurrentScene)
{
	CQuestUI*	pInstance = new CQuestUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CQuestUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuestUI::Free()
{
	__super::Free();
	
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
