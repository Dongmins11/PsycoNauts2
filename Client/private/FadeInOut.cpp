#include "stdafx.h"
#include "..\public\FadeInOut.h"
#include "GameInstance.h"
#include "Player.h"
CFadeInOut::CFadeInOut(ID3D11Device * _pGraphic_Device, ID3D11DeviceContext * _pGraphic_DeviceContext)
	:CGameObject(_pGraphic_Device, _pGraphic_DeviceContext)
{
}

CFadeInOut::CFadeInOut(const CFadeInOut & _rhs)
	: CGameObject(_rhs)
{
}

void CFadeInOut::Set_Type(_double _Speed, TYPE_COLOR _eColor, TYPE_INOUT _eInOut, _bool _bKeeping)
{
	
	m_dSpeed = _Speed, m_eType_Color = _eColor, m_eType_InOut = _eInOut,m_bKeeping = _bKeeping;
	
}

void CFadeInOut::Set_Reverse()
{
	//_double _Speed, TYPE_COLOR _eColor, TYPE_INOUT _eInOut, _bool _bKeeping = false

	if (INOUT_IN == m_eType_InOut)
	{
		m_eType_InOut = INOUT_OUT;
		m_dTimeAcc = 0.0;
		m_fAlpha = 0.f;
	}
	if (INOUT_OUT == m_eType_InOut)
	{
		m_eType_InOut = INOUT_IN;
		m_dTimeAcc = 0.0;
		m_fAlpha = 1.f;
	}

	m_bKeeping = !m_bKeeping;
}

void CFadeInOut::Active(_double Speed, _float4 vColor, TYPE_INOUT _eInOut)
{
	m_dSpeed = Speed, m_vColor = vColor, m_eType_InOut = _eInOut , m_dTimeAcc=0.0,m_bActive=true;
}

HRESULT CFadeInOut::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFadeInOut::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;



	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	m_fSizeX = Viewport.Width;
	m_fSizeY = Viewport.Height;
	m_fStartX = 0.f;
	m_fStartY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 2.f));

	m_eType_InOut = INOUT_OUT;
	m_eType_Color = TYPE_COLOR_BLACK;
	return S_OK;
}

_int CFadeInOut::Tick(_double _dTimeDelta)
{
	if (0 > __super::Tick(_dTimeDelta))
		return -1;

	if (nullptr == m_pTransform)
		return -1;

	m_dTimeAcc += _dTimeDelta*m_dSpeed;
	return _int();
}

_int CFadeInOut::LateTick(_double _dTimeDelta)
{
	if (0 > __super::LateTick(_dTimeDelta))
		return -1;

	if (nullptr == m_pRenderer)
		return -1;

	if (false == m_bActive)
		return 0;

	if (false == m_isDestroy)
	{
		if (FAILED(m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this)))
			return -1;
	}

	return _int();
}

HRESULT CFadeInOut::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Rect->Render(30)))
		return E_FAIL;
	return S_OK;
}

HRESULT CFadeInOut::SetUp_Component()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*For.VIBuffer_Rect*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer_Rect)))
		return E_FAIL;
	if (nullptr == m_pVIBuffer_Rect)
		return E_FAIL;

	/*For.Renderer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;
	if (nullptr == m_pRenderer)
		return E_FAIL;

	/*For.Texture*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Fade"), TEXT("Com_Texture_FadeInOut"), (CComponent**)&m_pTexture)))
		return E_FAIL;
	if (nullptr == m_pTexture)
		return E_FAIL;

	/*For.Transform*/
	CTransform::TRANSFORMDESC tTransformDest;
	ZeroMemory(&tTransformDest, sizeof(CTransform::TRANSFORMDESC));

	tTransformDest.SpeedPerSec = 5.0;
	tTransformDest.RotationPerSec = XMConvertToRadians(60.f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransform, &tTransformDest)))
		return E_FAIL;
	if (nullptr == m_pTransform)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CFadeInOut::SetUp_ConstantTable()
{
	if (nullptr == m_pTransform || nullptr == m_pVIBuffer_Rect)
		return E_FAIL;
	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);
	_float4x4 WorldMatrix = m_pTransform->Get_WorldFloat4x4();

	WorldMatrix._11 = m_fSizeX;
	WorldMatrix._22 = m_fSizeY;
	WorldMatrix._41 = (m_fStartX + m_fSizeX * 0.5f - (Viewport.Width * 0.5f));
	WorldMatrix._42 = -(m_fStartY + m_fSizeY * 0.5f) + (Viewport.Height * 0.5f);

	Compute_Alpha();
	if (FAILED(m_pVIBuffer_Rect->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Rect->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Rect->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Rect->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	m_vColor.w = m_fAlpha;
	if (FAILED(m_pVIBuffer_Rect->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pVIBuffer_Rect, 0)))
		return E_FAIL;

	return S_OK;
}


void CFadeInOut::Compute_Alpha()
{
	if (INOUT_OUT == m_eType_InOut)
	{
		m_fAlpha = m_dTimeAcc;
		if (1 <= m_fAlpha)
		{
			m_fAlpha = 1.f;
			//m_bActive = false;
		}
	}
	else if (INOUT_IN == m_eType_InOut)
	{
		m_fAlpha =1.f- m_dTimeAcc;
		if (0 >= m_fAlpha)
		{
			m_fAlpha = 0.f;
			m_bActive = true;
		}
	}
}

CFadeInOut * CFadeInOut::Create(ID3D11Device * _pGraphic_Device, ID3D11DeviceContext * _pGraphic_DeviceContext)
{
	CFadeInOut* pInstance = new CFadeInOut(_pGraphic_Device, _pGraphic_DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CFadeInOut");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFadeInOut::Clone(void* pArg, _uint iSceneIndex)
{
	CFadeInOut* pInstance = new CFadeInOut(*this);
	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed To Create CFadeInOut Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFadeInOut::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer_Rect);

}
