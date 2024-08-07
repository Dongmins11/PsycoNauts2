#include "stdafx.h"
#include "..\public\Bag.h"
#include "GameInstance.h"
#include "Player_RightHand.h"
//#include "PhysXCollider.h"
CBag::CBag(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CBag::CBag(const CBag & rhs)
	:CGameObject(rhs)
{
}

HRESULT CBag::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBag::NativeConstruct(void * pArg, _uint iCurrentScene)
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
	m_eLayerType = (LayerType)objinfo.iPX_Index;
	
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0);
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_Position());// +XMVectorSet(0.f, 5.f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PICK);
	return S_OK;
}

_int CBag::Tick(_double TimeDelta)
{
	if (!m_isRender)
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		if (!m_isRender && m_pCollider)
		{
			m_pCollider->Add_Actor();
		}
		m_isRender = true;
	}
	else
	{
		if (m_isRender && m_pCollider)
		{
			m_pCollider->Remove_Actor();
		}
		m_isRender = false;
	}


	if (m_isPicking)
	{
		CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
		_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pCollider->SetGravity(false);
	}
	else
	{
		m_pCollider->SetGravity(true);
	}

	if (pGameInstance->Key_Down(VK_HOME))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(9.f, 0.3f, 10.f, 1.f));
	}
	RELEASE_INSTANCE(CGameInstance);


	m_pModelCom->Update(TimeDelta);
	m_pCollider->Update_TransformPhysX();

	return _int();
}

_int CBag::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pCollider->Update_Transform();


	if(true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CBag::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}
    
void CBag::Setting_Throwing(_fvector vDir)
{
	m_pCollider->Add_Force(vDir*m_pTransformCom->Get_TransFormDescSpeed() * 3.f);// +XMVectorSet(0.f, 9.8f, 0.f, 0.f));
}

HRESULT CBag::SetUp_Component()
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
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom,&TransformDesc)))
		return E_FAIL;

	///* Com_Navigation */
	//CNavigation::NAVIDESC		NaviDesc;
	//ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	//NaviDesc.iCurrentIndex = m_NavigationIndex;

	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
	//	return E_FAIL;
	
	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = true;
	
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.isTrigger = false;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = m_pTransformCom->Get_ScaleFloat3();

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pCollider, &tBoxColliderDesc)))
		return E_FAIL;
	_matrix smartPivot = XMMatrixTranslation(0.f, tBoxColliderDesc.vScale.y * 0.5f, 0.f);
	m_pCollider->SetPivotMatrix(smartPivot);

	//m_pCollider->Remove_Actor();


	return S_OK;
}

HRESULT CBag::SetUp_ConstantTable()
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

CBag * CBag::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBag*	pInstance = new CBag(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBag::Clone(void * pArg, _uint iCurrentScene)
{
	CBag*	pInstance = new CBag(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBag::Free()
{
	__super::Free();


	if(m_pCollider)
		m_pCollider->Remove_Actor();
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
