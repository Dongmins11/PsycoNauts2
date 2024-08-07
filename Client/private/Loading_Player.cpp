#include "stdafx.h"
#include "..\Public\Loading_Player.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "CollisionObject.h"
#include "Scene_Loading.h"
#include "Player.h"
CLoading_Player::CLoading_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLoading_Player::CLoading_Player(const CLoading_Player & rhs)
	: CGameObject(rhs)

{
}

HRESULT CLoading_Player::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoading_Player::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_isRender = false;

	m_pModelCom->Set_Animation(0);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	//m_fX = g_iWinCX >> 1;
	//m_fY = g_iWinCY >> 1;

	m_pTransformCom->Scaling(_float3(0.01f, 0.01f, 0.01f));

	//XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));

	return S_OK;
}

_int CLoading_Player::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pModelCom->Lerp_Update(TimeDelta);

	return 0;
}

_int CLoading_Player::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, pGameInstance->Get_CamPosition());

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));

	Safe_Release(pGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CLoading_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iRenderPass)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoading_Player::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CModel::NODENAMEDESC tNodeDesc;
	ZeroMemory(&tNodeDesc, sizeof(CModel::NODENAMEDESC));

	tNodeDesc.szFinderBoneName = "Raz_LOBO_WholeBody.ao";
	tNodeDesc.szBottomBoneName = "hipRoot";
	tNodeDesc.szRootBoneName = "Root";

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_LoadingPlayer"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &tNodeDesc)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoading_Player::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	//_float4x4 WorldMatrix;
	//XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	//WorldMatrix._11 = g_iWinCX;
	//WorldMatrix._22 = g_iWinCY;
	//WorldMatrix._33 = 0.7f;
	//WorldMatrix._41 = m_fX - (g_iWinCX * 0.5f);
	//WorldMatrix._42 = -m_fY + (g_iWinCY * 0.5f);
	//WorldMatrix._43 = 0.2f;


	//_float4x4	ViewMatrix, ProjMatrix;
	//XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&WorldMatrix)));
	//XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)));

	//if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
	//	return E_FAIL;
	//if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	//	return E_FAIL;

	_float4x4 TransPoseWorldMat, ViewTransPoseMat, ProjectionTransPoseMat;

	XMStoreFloat4x4(&TransPoseWorldMat, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjectionTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &TransPoseWorldMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjectionTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoading_Player * CLoading_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLoading_Player*	pInstance = new CLoading_Player(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CLoading_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoading_Player::Clone(void* pArg, _uint iSceneIndex)
{
	CLoading_Player*	pInstance = new CLoading_Player(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CLoading_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoading_Player::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);


}
