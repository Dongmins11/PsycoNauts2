#include "stdafx.h"
#include "..\public\Balloon.h"
#include "GameInstance.h"

CBalloon::CBalloon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBalloon::CBalloon(const CBalloon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}
HRESULT CBalloon::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
	if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
		m_iCurrentScene, TEXT("Layer_Repeat_BalloonEffect"),
		SCENE_STATIC, TEXT("Effect_Repeat_BalloonEffect"), &m_pEffect)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	Safe_AddRef(m_pEffect);

	/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
	if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, true, m_pTransformCom, TEXT("Layer_Repeat_BalloonEffect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->Setting_Effect_MovePos(m_iCurrentScene, TEXT("Layer_Repeat_BalloonEffect"), TEXT("Com_Option"), m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CBalloon::Tick(_double TimeDelta)
{
	m_fTimeAcc += (_float)TimeDelta;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTransform* pTransform = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));

	_matrix PlayerMat = pTransform->Get_WorldMatrix();
	PlayerMat.r[3] += pTransform->Get_State(CTransform::STATE_UP) * 1.75f;

	_float4x4 WorldMat;
	XMStoreFloat4x4(&WorldMat, PlayerMat);
	m_pTransformCom->Set_WorldMatrix(WorldMat);
	RELEASE_INSTANCE(CGameInstance);

	if (true == m_isInteraction)
	{
		m_isDissolve = true;
		m_DissolveTime += TimeDelta;
		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}

		if (2.f <= m_DissolveTime)
		{
			m_isDestroy = true;

		}
	}

	return _int();
}

_int CBalloon::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CBalloon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_BALLOON);
	return S_OK;
}

HRESULT CBalloon::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Balloon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBalloon::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	
	m_pModelCom->Set_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(0.f, 1.f, 1.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(0.f, 1.f, 0.25f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pAlphaTexture->Get_SRV(6));

	_float fDissolveTime = (_float)m_DissolveTime * 5.f;

	if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_isDissolve, sizeof(_bool))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBalloon * CBalloon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBalloon*	pInstance = new CBalloon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBalloon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBalloon::Clone(void * pArg, _uint iCurrentScene)
{
	CBalloon*	pInstance = new CBalloon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBalloon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBalloon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAlphaTexture);

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
