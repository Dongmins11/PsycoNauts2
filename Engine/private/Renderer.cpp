#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "RenderTarget.h"
CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (FAILED(Add_RenderTarget()))
		return E_FAIL;

	if (FAILED(Add_MRT()))
		return E_FAIL;

	if (FAILED(Create_RenderTargetBuffer()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Ready_Debug_Buffer_Desc()))
		return E_FAIL;
#endif // _DEBUG

	_uint            iNumViewports = 1;
	D3D11_VIEWPORT      Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f)));
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

	m_TransformMatrix._11 = Viewport.Width;
	m_TransformMatrix._22 = Viewport.Height;

	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));

	m_bRenderOption[RENDEROPTION_RIMLIGHT] = false;
	m_bRenderOption[RENDEROPTION_OUTLINE] = false;
	m_bRenderOption[RENDEROPTION_SHADOW] = true;
	m_bRenderOption[RENDEROPTION_MOTIONBLUR] = false;
	m_bRenderOption[RENDEROPTION_HDR] = true;
	m_bRenderOption[RENDEROPTION_BLOOM] = true;
	m_bRenderOption[RENDEROPTION_BLOOMEFFECT] = true;
	m_bRenderOption[RENDEROPTION_DOP] = false;
	m_bRenderOption[RENDEROPTION_FOG] = false;
	m_bRenderOption[RENDEROPTION_EMISSIVE] = true;
	m_bRenderOption[RENDEROPTION_SSAO] = true;
	m_bRenderOption[RENDEROPTION_LUT] = false;
	m_bRenderOption[RENDEROPTION_DISTORTION_POST] = false;
	m_bRenderOption[RENDEROPTION_RADIAL] = false;
	/*For.Bloom*/

	m_BloomWeight[0] = 0.4f;
	m_BloomWeight[1] = 0.1f;
	m_BloomWeight[2] = 1.f;
	m_BloomWeight[3] = 1.f;

	/*For.BloomEffect*/

	m_BloomEffectWeight[0] = 1.f;
	m_BloomEffectWeight[1] = 0.2f;
	m_BloomEffectWeight[2] = 0.5f;
	m_BloomEffectWeight[3] = 0.625f;

	m_DOFWeight[0] = 0.1f;
	m_DOFWeight[1] = 0.1f;
	m_DOFWeight[2] = 15.f;
	m_DOFWeight[3] = 20.f;

	m_vRImRatio = _float2(1.f, 1.f);

	/*For.Fog*/
	m_fFog_Values[0] = 40.f;
	m_fFog_Values[1] = 0.03f;
	m_fFog_Values[2] = 0.001f;
	return S_OK;
}



HRESULT CRenderer::Add_RenderTarget()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	_uint            iNumViewports = 1;
	D3D11_VIEWPORT      Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Rom"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Rimlight"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DepthShadow"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Velocity"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Emissive"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Depth_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Rom_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Rimlight_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DepthShadow_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Velocity_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Emissive_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Reflect"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Distortion"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Irradiance"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f), CRenderTarget::TEXTURETYPE_CUBE, 1)))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_PreFilter"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f), CRenderTarget::TEXTURETYPE_CUBE, 5)))
	//   return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BRDF_LUT"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f))))
	//   return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_NonIBL"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f))))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_ShadowMap"), (_uint)Viewport.Width/**m_fMulResolution*/, (_uint)Viewport.Height/**m_fMulResolution*/, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)/*, CRenderTarget::TEXTURETYPE_DEPTH*/)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_ShadowMap_Origin"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Horizon"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Vertical"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Horizon_Brightness_2"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Vertical_Brightness_2"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Horizon_Brightness_4"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Blur_Vertical_Brightness_4"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_MotionBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DownSample_2"), (_uint)(Viewport.Width / 2.f), (_uint)(Viewport.Height / 2.f), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DownSample_4"), (_uint)(Viewport.Width / 4.f), (_uint)(Viewport.Height / 4.f), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_UpSample_2"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_UpSample_4"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Brightness"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Bloom"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_ToneMapping"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DepthBlur"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_DOF"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Fog"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Distortion_Post"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR_Origin"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)/*, CRenderTarget::TEXTURETYPE_DEPTH*/)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_HDR"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_SSAO"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect_Origin"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect_Origin_Emissive"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Effect_Emissive"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_BloomEffect"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_MSAA"), (_uint)Viewport.Width*m_fMulResolution, (_uint)Viewport.Height*m_fMulResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_RimBlur"), (_uint)(Viewport.Width*m_fMulResolution), (_uint)(Viewport.Height*m_fMulResolution), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), CRenderTarget::TEXTURETYPE_DEPTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_RimBlur_Origin"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_LUT"), (_uint)Viewport.Width, (_uint)Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Radial"), (_uint)(Viewport.Width), (_uint)(Viewport.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Add_MRT()
{

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Rimlight"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Rom"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Velocity"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_DepthShadow"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Diffuse_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Normal_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Depth_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Rimlight_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Rom_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Velocity_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_Emissive_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred_Origin"), TEXT("Target_DepthShadow_Origin"))))
		return E_FAIL;


	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_NonIBL"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ShadowMap"), TEXT("Target_ShadowMap"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ShadowMap_Origin"), TEXT("Target_ShadowMap_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Reflect"), TEXT("Target_Reflect"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Irradiance"), TEXT("Target_Irradiance"))))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_IBL"), TEXT("Target_PreFilter"))))
	//   return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LUT"), TEXT("Target_BRDF_LUT"))))
	//   return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur_Horizon"), TEXT("Target_Blur_Horizon"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur_Vertical"), TEXT("Target_Blur_Vertical"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur_Horizon_Brightness_2"), TEXT("Target_Blur_Horizon_Brightness_2"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur_Vertical_Brightness_2"), TEXT("Target_Blur_Vertical_Brightness_2"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur_Horizon_Brightness_4"), TEXT("Target_Blur_Horizon_Brightness_4"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Blur_Vertical_Brightness_4"), TEXT("Target_Blur_Vertical_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("Target_MotionBlur"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DownSample_2"), TEXT("Target_DownSample_2"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DownSample_4"), TEXT("Target_DownSample_4"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_UpSample_2"), TEXT("Target_UpSample_2"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_UpSample_4"), TEXT("Target_UpSample_4"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Brightness"), TEXT("Target_Brightness"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToneMapping"), TEXT("Target_ToneMapping"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DepthBlur"), TEXT("Target_DepthBlur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR_Origin"), TEXT("Target_HDR_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect_Origin"), TEXT("Target_Effect_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Effect_Origin"), TEXT("Target_Effect_Origin_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomEffect"), TEXT("Target_BloomEffect"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_MSAA"), TEXT("Target_MSAA"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_RimBlur"), TEXT("Target_RimBlur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_RimBlur_Origin"), TEXT("Target_RimBlur_Origin"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LUT"), TEXT("Target_LUT"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog"))))
		return E_FAIL; 
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Distortion_Post"), TEXT("Target_Distortion_Post"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Radial"), TEXT("Target_Radial"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Create_RenderTargetBuffer()
{

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer_Down_2 = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, (1.f / 2.f), (1.f / 2.f));
	if (nullptr == m_pVIBuffer_Down_2)
		return E_FAIL;

	m_pVIBuffer_Down_4 = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, (1.f / 4.f), (1.f / 4.f));
	if (nullptr == m_pVIBuffer_Down_4)
		return E_FAIL;

	m_pVIBuffer_Up_2 = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, m_fMulResolution, m_fMulResolution);
	if (nullptr == m_pVIBuffer_Up_2)
		return E_FAIL;

	//m_pTexture_LUT = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/LUT/LUT%d.dds"), 59);
	//if (nullptr == m_pTexture_LUT)
	//	return E_FAIL;
	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Ready_Debug_Buffer_Desc()
{
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Diffuse"), 0.f, 0.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Normal"), 0.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Depth"), 0.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Rom"), 0.f, 300.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Rimlight"), 100.f, 0.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_DepthShadow"), 100.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Emissive"), 100.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Velocity"), 100.f, 300.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_ShadowMap"), 200.f, 0.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_NonIBL"), 200.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Specular"), 200.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Irradiance"), 200.f, 300.f, 100.f, 100.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Blur_Horizon"), 200.f, 400.f, 100.f, 100.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Blur_Vertical"), 200.f, 500.f, 100.f, 100.f)))
	//	return E_FAIL;

	//if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_DownSample"), 300.f, 100.f, 100.f, 100.f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_UpSample"), 300.f, 200.f, 100.f, 100.f)))
	//	return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Brightness"), 200.f, 300.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Bloom"), 200.f, 400.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_ToneMapping"), 200.f, 500.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_DOF"), 300.f, 0.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_SSAO"), 300.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Effect"), 300.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_BloomEffect"), 300.f, 300.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_MSAA"), 300.f, 400.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_RimBlur"), 300.f, 500.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_LUT"), 300.f, 600.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Fog"), 400.f, 0.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Reflect"), 400.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Distortion"), 400.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Distortion_Post"), 400.f, 300.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Radial"), 400.f, 400.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_HDR"), 400.f, 500.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_HDR_Origin"), 400.f, 600.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_RimBlur_Origin"), 500.f, 000.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Diffuse_Origin"),500.f, 100.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer_Desc(TEXT("Target_Effect_Emissive"), 500.f, 200.f, 100.f, 100.f)))
		return E_FAIL;
	return S_OK;
}
#endif
HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderGroup[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	/*For.Defferd*/

	if (true == m_bRenderOption[RENDEROPTION_SHADOW])
	{
		if (FAILED(Render_Shadow()))
			return E_FAIL;
	}
	if (FAILED(Render_Reflect()))
		return E_FAIL;
	if (FAILED(Render_Distortion()))
		return E_FAIL;


	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	if (FAILED(Render_MSAA()))
		return E_FAIL;

	//if (FAILED(Render_IBL()))
	//	return E_FAIL;
	if (FAILED(Render_Rim_Blur()))
		return E_FAIL;


	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (FAILED(Render_HDR()))
			return E_FAIL;
	}

	//if (FAILED(Render_MSAA()))
	//	return E_FAIL;

	/*For.SSAO*/

	if (true == m_bRenderOption[RENDEROPTION_SSAO])
	{
		if (FAILED(Render_SSAO()))
			return E_FAIL;

		if (FAILED(Render_SSAO_Blur()))
			return E_FAIL;
	}
	/*For.Light*/

	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_Effect()))
		return E_FAIL;
	//if (FAILED(Render_TotalDiffuse()))
	//	return E_FAIL;
	/*For.Post Process*/
	if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(Render_Brightness()))
			return E_FAIL;

		if (FAILED(Render_Blur()))
			return E_FAIL;
		if (FAILED(Render_BrightnessBlur()))
			return E_FAIL;
		if (FAILED(Render_BrightnessBlur_Two()))
			return E_FAIL;
		if (FAILED(Render_Bloom()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_BLOOMEFFECT])
	{
		if (FAILED(Render_Brightness_Effect()))
			return E_FAIL;
		if (FAILED(Render_EffectBlur()))
			return E_FAIL;
		//if (FAILED(Render_BrightnessBlur_Effect()))
		//	return E_FAIL;
		if (FAILED(Render_BrightnessBlur_Two_Effect()))
			return E_FAIL;
		if (FAILED(Render_Bloom_Effect()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(Render_DOPBlur()))
			return E_FAIL;

		if (FAILED(Render_DOP()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_FOG])
	{
		if (FAILED(Render_Fog()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(Render_MotionBlur()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_DISTORTION_POST])
	{
		if (FAILED(Render_Distortion_Post()))
			return E_FAIL;
	}
	//if (true == m_bRenderOption[RENDEROPTION_LUT])
	//{
	//	if (FAILED(Render_LUT()))
	//		return E_FAIL;
	//}
	if (true == m_bRenderOption[RENDEROPTION_RADIAL])
	{
		if (FAILED(Render_Radial()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (FAILED(Render_ToneMapping()))
			return E_FAIL;
	}

	/*For.Blend*/
	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Alpha()))
		return E_FAIL;

	if (FAILED(Render_NonBloomEffect()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (m_pTarget_Manager->Get_IsRender())
	{

		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Deferred"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_LightAcc"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_ShadowMap"))))
			return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_IBL"))))
		//   return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_LUT"))))
		//   return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Irradiance"))))
		//   return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Blur_Horizon"))))
		//	return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Blur_Vertical"))))
		//	return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_DownSample"))))
		//	return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_UpSample"))))
		//	return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Brightness"))))
			return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_DepthBlur"))))
		//	return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Bloom"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_ToneMapping"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_DOF"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_SSAO"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_BloomEffect"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_MSAA"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_LUT"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Fog"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Reflect"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Radial"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_HDR"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_HDR_Origin"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_RimBlur_Origin"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Deferred_Origin"))))
			return E_FAIL;		 
	}
	//else
	//{
	//   ID3D11ShaderResourceView*      SRV[1] = { nullptr };
	//   m_pDeviceContext->PSSetShaderResources(0, 1, SRV);
	//}
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup_Tool()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	/*For.Defferd*/

	if (true == m_bRenderOption[RENDEROPTION_SHADOW])
	{
		if (FAILED(Render_Shadow()))
			return E_FAIL;
	}

	if (FAILED(Render_NonAlpha()))
		return E_FAIL;

	//if (FAILED(Render_IBL()))
	//	return E_FAIL;


	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (FAILED(Render_HDR()))
			return E_FAIL;
	}


	/*For.SSAO*/

	if (true == m_bRenderOption[RENDEROPTION_SSAO])
	{
		if (FAILED(Render_SSAO()))
			return E_FAIL;

		if (FAILED(Render_SSAO_Blur()))
			return E_FAIL;
	}
	/*For.Light*/

	if (FAILED(Render_Lights()))
		return E_FAIL;


	/*For.Post Process*/
	if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(Render_Brightness()))
			return E_FAIL;

		if (FAILED(Render_Blur()))
			return E_FAIL;
		if (FAILED(Render_BrightnessBlur()))
			return E_FAIL;
		if (FAILED(Render_BrightnessBlur_Two()))
			return E_FAIL;
		if (FAILED(Render_Bloom()))
			return E_FAIL;
	}

	if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(Render_DOPBlur()))
			return E_FAIL;

		if (FAILED(Render_DOP()))
			return E_FAIL;
	}
	if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(Render_MotionBlur()))
			return E_FAIL;
	}

	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (FAILED(Render_ToneMapping()))
			return E_FAIL;
	}
	/*For.Blend*/
	if (FAILED(Render_Blend_Tool()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Alpha()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (m_pTarget_Manager->Get_IsRender())
	{

		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Deferred"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_LightAcc"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_ShadowMap"))))
			return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_IBL"))))
		//   return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_LUT"))))
		//   return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Irradiance"))))
		//   return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Blur_Horizon"))))
		//	return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Blur_Vertical"))))
		//	return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_DownSample"))))
		//	return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_UpSample"))))
		//	return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Brightness"))))
			return E_FAIL;
		//if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_DepthBlur"))))
		//	return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_Bloom"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_ToneMapping"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_DOF"))))
			return E_FAIL;
		if (FAILED(m_pTarget_Manager->Render_Debug_Buffer(TEXT("MRT_SSAO"))))
			return E_FAIL;
	}
	//else
	//{
	//   ID3D11ShaderResourceView*      SRV[1] = { nullptr };
	//   m_pDeviceContext->PSSetShaderResources(0, 1, SRV);
	//}
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
	//if (true == m_bIsSSAA)
	//{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_ShadowMap"))))
		return E_FAIL;
	//}
	//else
	//{
	//	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_ShadowMap_Origin"))))
	//		return E_FAIL;
	//}
	for (auto& pGameObject : m_RenderGroup[RENDER_SHADOW])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Set_RenderPass(1);
			pGameObject->Render();
			pGameObject->Set_RenderPassBackUp();
		}
		//Safe_Release(pGameObject);

	}

	for (auto& pGameObject : m_RenderGroup[RENDER_PLAYER])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Set_RenderPass(1);
			pGameObject->Render();
			pGameObject->Set_RenderPassBackUp();
		}
		//Safe_Release(pGameObject);

	}
	//m_RenderGroup[RENDER_NONALPHA].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Reflect()
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Reflect"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_REFLECT])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Set_RenderPass(2);
			pGameObject->Render();
			pGameObject->Set_RenderPassBackUp();
		}
		Safe_Release(pGameObject);

	}

	m_RenderGroup[RENDER_REFLECT].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred_Origin"))))
			return E_FAIL;
	}
	for (auto& pGameObject : m_RenderGroup[RENDER_SHADOW])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
		Safe_Release(pGameObject);

	}
	m_RenderGroup[RENDER_SHADOW].clear();
	for (auto& pGameObject : m_RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
		Safe_Release(pGameObject);

	}
	m_RenderGroup[RENDER_NONALPHA].clear();

	for (auto& pGameObject : m_RenderGroup[RENDER_REFLECT_DEST])

	{
		if (nullptr != pGameObject)
		{

			pGameObject->Render();

		}
		Safe_Release(pGameObject);

	}
	m_RenderGroup[RENDER_REFLECT_DEST].clear();
	for (auto& pGameObject : m_RenderGroup[RENDER_NONSHADOW_LAST])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
		Safe_Release(pGameObject);

	}
	m_RenderGroup[RENDER_NONSHADOW_LAST].clear();

	for (auto& pGameObject : m_RenderGroup[RENDER_PLAYER])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
		Safe_Release(pGameObject);

	}
	m_RenderGroup[RENDER_PLAYER].clear();

	for (auto& pGameObject : m_RenderGroup[RENDER_TEST])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
		Safe_Release(pGameObject);

	}
	m_RenderGroup[RENDER_TEST].clear();
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Set_RenderPass(10);
			pGameObject->Render();
			pGameObject->Set_RenderPassBackUp();
		}
		Safe_Release(pGameObject);

	}

	m_RenderGroup[RENDER_DISTORTION].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Rim_Blur()
{

	////Blur - Horizon
	//CVIBuffer_Rect* pVIBuffer_Rect = nullptr;
	//_float4x4 ProjMatrix;
	//if (true == m_bIsSSAA)
	//{
	//	pVIBuffer_Rect = m_pVIBuffer_Up_2;
	//	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 * m_fMulResolution, 720 * m_fMulResolution, 0.f, 1.f)));

	//	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
	//		return E_FAIL;
	//	if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MSAA")))))
	//		return E_FAIL;
	//}
	//else
	//{
	//	pVIBuffer_Rect = m_pVIBuffer;
	//	ProjMatrix = m_ProjMatrix;
	//	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_HDR_Origin"))))
	//		return E_FAIL;
	//	if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse_Origin")))))
	//		return E_FAIL;
	//}

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	CVIBuffer_Rect* pVIBuffer_Rect = nullptr;
	_float4x4 ProjMatrix;


	if (true == m_bIsSSAA)
	{
		pVIBuffer_Rect = m_pVIBuffer_Up_2;
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 * m_fMulResolution, 720 * m_fMulResolution, 0.f, 1.f)));
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_RimBlur"))))
			return E_FAIL;
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Rimlight")))))
			return E_FAIL;
	}
	else
	{
		pVIBuffer_Rect = m_pVIBuffer;
		ProjMatrix = m_ProjMatrix;
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_RimBlur_Origin"))))
			return E_FAIL;
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Rimlight_Origin")))))
			return E_FAIL;
	}


	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX, fY;
	if (true == m_bIsSSAA)
	{
		fX =1280.f *m_fMulResolution;
		fY = 720.f *m_fMulResolution;
	}
	else
	{
		fX = 1280.f;
		fY = 720.f;
	}
	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	pVIBuffer_Rect->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical"))))
		return E_FAIL;

	if (true == m_bIsSSAA)
	{
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_RimBlur")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_RimBlur_Origin")))))
			return E_FAIL;
	}


	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	pVIBuffer_Rect->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	//Blur - Horizon
	CVIBuffer_Rect* pVIBuffer_Rect = nullptr;
	_float4x4 ProjMatrix;
	if (true == m_bIsSSAA)
	{
		pVIBuffer_Rect = m_pVIBuffer_Up_2;
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 * m_fMulResolution, 720 * m_fMulResolution, 0.f, 1.f)));

		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_HDR"))))
			return E_FAIL;
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MSAA")))))
			return E_FAIL;
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_RimlightTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
			return E_FAIL;
	}
	else
	{
		pVIBuffer_Rect = m_pVIBuffer;
		ProjMatrix = m_ProjMatrix;
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_HDR_Origin"))))
			return E_FAIL;
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse_Origin")))))
			return E_FAIL;
		if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_RimlightTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
			return E_FAIL;
	}
	//if (FAILED(pVIBuffer_Rect->Set_TextureValue("g_RimlightTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
	//	return E_FAIL;
	//if (FAILED(m_pVIBuffer_Up_2->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
	//	return E_FAIL;
	//if (FAILED(m_pVIBuffer_Up_2->Set_TextureValue("g_RimlightTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Rimlight")))))
	//	return E_FAIL;
	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pVIBuffer_Rect->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	pVIBuffer_Rect->Render(23);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Up_2->Set_TextureValue("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Up_2->Set_TextureValue("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 * m_fMulResolution, 720 * m_fMulResolution, 0.f, 1.f)));

	if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_fSSAORadious", &m_fSSAORadious, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_fBias", &m_fBias, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer_Up_2->Render(24);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_MSAA()
{
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_MSAA"))))
			return E_FAIL;

		if (FAILED(m_pVIBuffer_Up_2->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
			return E_FAIL;


		if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
			return E_FAIL;

		_float4x4   ViewMatrix;
		XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

		if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		_float4x4 ProjMatrix;
		XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 * m_fMulResolution, 720 * m_fMulResolution, 0.f, 1.f)));

		if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_fMsaaOffset", &m_fMSAA_Offset, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer_Up_2->Set_RawValue("g_iSampler", &m_iMassSampler, sizeof(_uint))))
			return E_FAIL;
		m_pVIBuffer_Up_2->Render(35);

		if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
			return E_FAIL;
	}
	return S_OK;

}
HRESULT CRenderer::Render_SSAO_Blur()
{

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Down_4->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_SSAO")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer_Down_4->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer_Down_4->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 4.f, 720 / 4.f, 0.f, 1.f)));

	if (FAILED(m_pVIBuffer_Down_4->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer_Down_4->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSample_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon_Brightness_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Up
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;


	return S_OK;
}



HRESULT CRenderer::Render_Blur()
{
	//Down
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer_Down->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonIBL")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Down->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	//if (FAILED(m_pVIBuffer_Down->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//_float4x4 ProjMatrix;
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 4.f, 720 / 4.f, 0.f, 1.f)));

	//if (FAILED(m_pVIBuffer_Down->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer_Down->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	////Up
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer_Up->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer_Up->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_BrightnessBlur()
{

	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample_2"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer_Down_2->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	//if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//_float4x4 ProjMatrix;
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 2.f, 720 / 2.f, 0.f, 1.f)));

	//if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer_Down_2->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon_Brightness_2"))))
		return E_FAIL;

	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive_Origin")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical_Brightness_2"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon_Brightness_2")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	////Up
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample_2"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer_Up->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_2")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer_Up->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_BrightnessBlur_Two()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample_2"))))
		return E_FAIL;
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer_Down_2->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer_Down_2->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive_Origin")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 2.f, 720 / 2.f, 0.f, 1.f)));

	if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer_Down_2->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSample_2")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon_Brightness_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Up
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_DOPBlur()
{

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon"))))
		return E_FAIL;
	if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	////Up
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample_4"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_4")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Bloom"))))
		return E_FAIL;

	//Diffuse¹à±â
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	//Brightness¹à±â
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_BloomColorTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
		return E_FAIL;
	//Emissive ¹à±â
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_BlurColor2Texture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_2")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_BlurColor4Texture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSample_4")))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fDiffuseWeight", &m_BloomWeight[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fBloomColorWeight", &m_BloomWeight[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fBlurColor2Weight", &m_BloomWeight[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fBlurColor4Weight", &m_BloomWeight[3], sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(19);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_EffectBlur()
{
	//Down
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer_Down->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_NonIBL")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Down->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	//if (FAILED(m_pVIBuffer_Down->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//_float4x4 ProjMatrix;
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 4.f, 720 / 4.f, 0.f, 1.f)));

	//if (FAILED(m_pVIBuffer_Down->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer_Down->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon"))))
		return E_FAIL;
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect_Origin")))))
			return E_FAIL;
	}


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	////Up
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample"))))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer_Up->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;


	//if (FAILED(m_pVIBuffer_Up->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	//m_pVIBuffer_Up->Render(0);
	//if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Brightness_Effect()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Brightness"))))
		return E_FAIL;

	//if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	//{
	//	if (FAILED(m_pVIBuffer_Brightness->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
	//		return E_FAIL;
	//}
	//else
	//{
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect_Emissive")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect_Origin_Emissive")))))
			return E_FAIL;
	}
	//}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(18);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BrightnessBlur_Effect()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample_2"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Down_2->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Brightness")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 2.f, 720 / 2.f, 0.f, 1.f)));

	if (FAILED(m_pVIBuffer_Down_2->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer_Down_2->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon_Brightness_2"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSample_2")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical_Brightness_2"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon_Brightness_2")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Up
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample_2"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_2")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_BrightnessBlur_Two_Effect()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DownSample_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer_Down_4->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Brightness")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer_Down_4->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer_Down_4->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280 / 4.f, 720 / 4.f, 0.f, 1.f)));

	if (FAILED(m_pVIBuffer_Down_4->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer_Down_4->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Horizon_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DownSample_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;
	_float fX = 1280.f;
	_float fY = 720.f;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(15);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Blur - Vertical
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Blur_Vertical_Brightness_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Horizon_Brightness_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iBlurOption", &m_iBlurOption, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCX", &fX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fWinCY", &fY, sizeof(_float))))
		return E_FAIL;
	m_pVIBuffer->Render(16);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	//Up
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_UpSample_4"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical_Brightness_4")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(0);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Bloom_Effect()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_BloomEffect"))))
		return E_FAIL;


	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect_Origin")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_BloomColorTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_BlurColor2Texture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSample_4")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_BlurColor4Texture", m_pTarget_Manager->Get_SRV(TEXT("Target_UpSample_4")))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fDiffuseWeight", &m_BloomEffectWeight[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fBloomColorWeight", &m_BloomEffectWeight[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fBlurColor2Weight", &m_BloomEffectWeight[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fBlurColor4Weight", &m_BloomEffectWeight[3], sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render(19);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ToneMapping()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_ToneMapping"))))
		return E_FAIL;
	//if (true == m_bRenderOption[RENDEROPTION_LUT])
	//{
	//	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_LUT")))))
	//		return E_FAIL;
	//}
	/*else */
	if (true == m_bRenderOption[RENDEROPTION_RADIAL])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Radial")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DISTORTION_POST])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Distortion_Post")))))
			return E_FAIL;
	}

	else if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_FOG])
	{

		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{

		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iToneOption", &m_iToneOption, sizeof(_uint))))
		return E_FAIL;

	m_pVIBuffer->Render(20);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LUT()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LUT"))))
		return E_FAIL;
	if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{

		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pTexture_LUT->SetUp_OnShader("g_LutTexture", m_pVIBuffer, m_iLUT_Index)))
		return E_FAIL;

	m_pVIBuffer->Render(35);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_DOP()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_DOF"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;
	if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_TotalBlurTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Blur_Vertical")))))
		return E_FAIL;
	_float4 vDofParam = _float4(m_DOFWeight[0], m_DOFWeight[1], m_DOFWeight[2], m_DOFWeight[3]);
	if (FAILED(m_pVIBuffer->Set_RawValue("g_vDofParam", &vDofParam, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(22);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Fog()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Fog"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_TextureValue("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;

	if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	//_float4 vFogcolor = _float4(0.3019f, 0.4f, 0.5019f, 1.f);
	//_float fFogstart = 40.f;
	//_float fFogdensity = 0.03f;
	//_float fFogfalloff = 0.001f;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fFogcolor", &m_vFog_Color, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fFogstart", &m_fFog_Values[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fFogDenstiy", &m_fFog_Values[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_fFogType", &m_bFog_Height, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_fFogfalloff", &m_fFog_Values[2], sizeof(_float))))
		return E_FAIL;

	_float4x4	ViewMatrixInverse, ProjMatrixInverse;
	_float4		vCamPosition;
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	XMStoreFloat4x4(&ViewMatrixInverse, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::D3DTS_VIEW))));
	m_pVIBuffer->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInverse, sizeof(_float4x4));

	XMStoreFloat4x4(&ProjMatrixInverse, XMMatrixTranspose(XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::D3DTS_PROJ))));
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInverse, sizeof(_float4x4));

	XMStoreFloat4(&vCamPosition, pPipeLine->Get_CamPosition());
	m_pVIBuffer->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	RELEASE_INSTANCE(CPipeLine);
	m_pVIBuffer->Render(37);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_MotionBlur"))))
		return E_FAIL;

	if (true == m_bRenderOption[RENDEROPTION_FOG])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_VelocityTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Velocity")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(17);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Distortion_Post()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Distortion_Post"))))
		return E_FAIL;

	if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_FOG])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Fog")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_VelocityTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Velocity")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	//¹Ù²ã¾ßÇÔ
	m_pVIBuffer->Render(17);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Radial()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Radial"))))
		return E_FAIL;

	if (true == m_bRenderOption[RENDEROPTION_DISTORTION_POST])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Distortion_Post")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}


	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_iRadialCnt", &m_iRadialCnt, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_iRadialBlurPower", &m_fRadialBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_vRadialDir", &m_vRadialDir, sizeof(_float2))))
		return E_FAIL;
	m_pVIBuffer->Render(38);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pLight_Manager ||
		nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;


	m_pLight_Manager->Set_IsSSAA(m_bIsSSAA);
	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (true == m_bIsSSAA)
			m_pLight_Manager->Set_DiffuseTag(TEXT("Target_HDR"));
		else
		{
			m_pLight_Manager->Set_DiffuseTag(TEXT("Target_HDR_Origin"));

		}


	}
	else
	{
		m_pLight_Manager->Set_DiffuseTag(TEXT("Target_Diffuse"));

	}

	if (true == m_bRenderOption[RENDEROPTION_EMISSIVE])
		m_pLight_Manager->Set_IsEmissive(true);
	else if (false == m_bRenderOption[RENDEROPTION_EMISSIVE])
		m_pLight_Manager->Set_IsEmissive(false);

	if (true == m_bRenderOption[RENDEROPTION_SHADOW])
		m_pLight_Manager->Set_IsShadow(true);
	else if (false == m_bRenderOption[RENDEROPTION_SHADOW])
		m_pLight_Manager->Set_IsShadow(false);

	m_pLight_Manager->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	//Blur - Horizon
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Effect"))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Effect_Origin"))))
			return E_FAIL;
	}

	if (FAILED(Render_Particle()))
		return E_FAIL;

	if (FAILED(Render_Rect_Effect()))
		return E_FAIL;

	if (FAILED(Render_Model_Effect()))
		return E_FAIL;

	if (FAILED(Render_Model_Effect_After()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_TotalDiffuse()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_TotalDiffuse"))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	if (true == m_bIsSSAA)
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_EffectTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_EffectTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect_Origin")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(18);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_Brightness()
{
	//Blur - Horizon
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Brightness"))))
		return E_FAIL;

	//if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	//{
	//	if (FAILED(m_pVIBuffer_Brightness->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
	//		return E_FAIL;
	//}
	//else
	//{
	if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	//}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(18);
	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_IBL()
{
	//Irradiance, PreFileter ..
	//for (_uint i = 0; i < 2; ++i)
	//{
	for (_uint j = 0; j < 6; ++j)
	{
		if (FAILED(m_pTarget_Manager->Begin_MRT_Arr(m_pDeviceContext, TEXT("MRT_Irradiance"), 0, j)))
			return E_FAIL;
		for (auto& pGameObject : m_RenderGroup[RENDER_IBL])
		{
			if (nullptr != pGameObject)
			{
				pGameObject->Render();
			}
			//Å¥ºê ¸é °¹¼öÀÇ ·»´õÅ¸°Ù. 6°³°¡ ÇÏ³ªÀÇ srv¿¡ ±×·ÁÁü??
			if (/*i==1 && */5 == j)
				Safe_Release(pGameObject);
		}
		if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
			return E_FAIL;
	}
	//}
	m_RenderGroup[RENDER_IBL].clear();
	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ToneMapping")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_vLightDir", &_float4(-1.f, -1.f, -1.f, 0.f), sizeof(_float4))))
		return E_FAIL;
	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(6);
	if (true == m_bRenderOption[RENDEROPTION_BLOOMEFFECT])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_EffectTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_BloomEffect")))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_EffectTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Effect")))))
			return E_FAIL;
	}
	m_pVIBuffer->Render(26);

	ID3D11ShaderResourceView* pSRV = NULL;

	m_pDeviceContext->PSSetShaderResources(3, 1, &pSRV);

	return S_OK;
}

HRESULT CRenderer::Render_Blend_Tool()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	if (true == m_bRenderOption[RENDEROPTION_HDR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ToneMapping")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_MOTIONBLUR])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_MotionBlur")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_DOP])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_DOF")))))
			return E_FAIL;
	}
	else if (true == m_bRenderOption[RENDEROPTION_BLOOM])
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Bloom")))))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pVIBuffer->Set_TextureValue("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Set_RawValue("g_WorldMatrix", &m_TransformMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Set_RawValue("g_vLightDir", &_float4(-1.f, -1.f, -1.f, 0.f), sizeof(_float4))))
		return E_FAIL;
	_float4x4   ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	m_pVIBuffer->Render(6);
	ID3D11ShaderResourceView* pSRV = NULL;

	m_pDeviceContext->PSSetShaderResources(3, 1, &pSRV);

	return S_OK;
}


HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{
	//m_RenderGroup[RENDER_ALPHA].sort([&](CGameObject* pSour, CGameObject* pDest) 
	//{
	//   return pSour->Get_ViewZ() > pDest->Get_ViewZ();
	//});

	for (auto& pGameObject : m_RenderGroup[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBloomEffect()
{
	m_RenderGroup[RENDER_NONBLOOM_EFFECT].sort([&](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_ViewZ() > pDest->Get_ViewZ();
	});

	for (auto& pGameObject : m_RenderGroup[RENDER_NONBLOOM_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONBLOOM_EFFECT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Model_Effect()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_MODEL_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_MODEL_EFFECT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Model_Effect_After()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_MODEL_EFFECT_AFTER])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_MODEL_EFFECT_AFTER].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Rect_Effect()
{
	m_RenderGroup[RENDER_RECT_EFFECT].sort([&](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_ViewZ() > pDest->Get_ViewZ();
	});

	for (auto& pGameObject : m_RenderGroup[RENDER_RECT_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_RECT_EFFECT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Particle()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PARTICLE])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PARTICLE].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderGroup[RENDER_UI].sort([&](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_ViewZ() < pDest->Get_ViewZ();
	});

	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Tool()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_TOOL])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_TOOL].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer*   pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderGroup)
	{
		for (auto& pGameObject : RenderList)
			Safe_Release(pGameObject);

		RenderList.clear();
	}
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBuffer_Down_2);
	Safe_Release(m_pVIBuffer_Down_4);
	Safe_Release(m_pVIBuffer_Up_2);
	//Safe_Release(m_pTexture_LUT);
}