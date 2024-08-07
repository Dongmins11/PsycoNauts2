#include "stdafx.h"
#include "..\public\DoubleDoorBlocker.h"
#include "GameInstance.h"
#include "Player.h"

CDoubleDoorBlocker::CDoubleDoorBlocker(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CDoubleDoorBlocker::CDoubleDoorBlocker(const CDoubleDoorBlocker & rhs)
	:CGameObject(rhs)
{
}

HRESULT CDoubleDoorBlocker::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CDoubleDoorBlocker::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_iPX_TYPE = objinfo.iPX_Index;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	StageStartAnim();
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	return S_OK;
}

_int CDoubleDoorBlocker::Tick(_double TimeDelta)
{
	if(m_AnimStart)
		PlayAnimation(TimeDelta);
	else
		m_pModelCom->Update(0.f);

	return _int();
}

_int CDoubleDoorBlocker::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	//m_pCollider->Update_Transform();
	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CDoubleDoorBlocker::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CDoubleDoorBlocker::Setting_Throwing(_fvector vDir)
{
}

void CDoubleDoorBlocker::StageStartAnim()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint LayerSize = pGameInstance->Get_LayerSize(m_iCurrentScene, TEXT("Layer_DoubleDoorBlocker"));

	switch (m_iCurrentScene)
	{
	case SCENE_DEMO:
	{
		if (4 != LayerSize)
		{
			m_pModelCom->Set_Animation(STATE_OPEN_CLOSE);
			m_MYObjectCount = LayerSize;
			m_AnimationSTATE = STATE_OPEN_CLOSE;
		}
		else
		{
			m_pModelCom->Set_Animation(STATE_CLOSE_OPEN);
			m_MYObjectCount = LayerSize;
			m_AnimationSTATE = STATE_CLOSE_OPEN;
		}
	}
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CDoubleDoorBlocker::PlayAnimation(_double TimeDelta)
{
	_double  AimtionTime = TimeDelta;
	if (true == m_pModelCom->Get_IsAnimFinishied(m_AnimationSTATE))
	{
		if (4 != m_MYObjectCount && false == m_Finish)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
			if (!pPlayer)
				return;
			pPlayer->Set_Delete_CurlingIndex(m_MYObjectCount);
			RELEASE_INSTANCE(CGameInstance);
		}
		AimtionTime = 0.0;
		m_Finish = true;
	}
	if(false == m_Finish)
		m_pModelCom->Lerp_Update(AimtionTime);
}

HRESULT CDoubleDoorBlocker::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CDoubleDoorBlocker::SetUp_ConstantTable()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDoubleDoorBlocker * CDoubleDoorBlocker::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDoubleDoorBlocker*	pInstance = new CDoubleDoorBlocker(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CDoubleDoorBlocker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDoubleDoorBlocker::Clone(void * pArg, _uint iCurrentScene)
{
	CDoubleDoorBlocker*	pInstance = new CDoubleDoorBlocker(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CDoubleDoorBlocker");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDoubleDoorBlocker::Free()
{
	__super::Free();

	//	Safe_Release(m_pCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
