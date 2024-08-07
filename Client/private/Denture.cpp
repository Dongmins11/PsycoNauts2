#include "stdafx.h"
#include "Denture.h"
#include "Cracked_Denture.h"
#include "GameInstance.h"
#include "CollisionObject.h"

CDenture::CDenture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CDenture::CDenture(const CDenture & rhs)
	: CEnemy(rhs)
{
}

HRESULT CDenture::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDenture::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	//m_iPX_TYPE = objinfo.iPX_Index;
	m_eLayerType = (LayerType)objinfo.iPX_Index;

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = 1.f;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0);
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	_vector WorldPos = m_pTransformCom->Get_Position();
	m_pCharacterController->Set_Position(_float3(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos)+1.f, XMVectorGetZ(WorldPos)));

	return S_OK;
}

_int CDenture::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pTransformCom->Set_Velocity(XMVectorZero());

	if (!m_pCharacterController->IsDown())
		m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -0.5f);

	Update_LookVector(TimeDelta);

	m_pModelCom->Lerp_Update(TimeDelta);

	if (10 >= Get_Distance())
	{
		m_pTransformCom->Add_Velocity(CTransform::STATE_LOOK, (_float)TimeDelta);

		m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

		m_pCharacterController->Update_Transform();

		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.r[3] += m_pTransformCom->Get_State(CTransform::STATE_UP);
		m_pHitColliderCom->Update_TransformPhysX(WorldMatrix);
	}

	if (m_isDestroy)
		m_pHitColliderCom->Remove_Actor();

	return _int();
}

_int CDenture::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (0 > m_iHp)
		m_isDestroy = true;
	return 0;
}

HRESULT CDenture::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CDenture::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
	{
		m_isDestroy = true;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CCracked_Denture::DENTUREINFO tDentureInfo;
		tDentureInfo.MeshType = rand() % 3;
		tDentureInfo.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

		pGameInstance->Add_ObjectToLayer(SCENE_STATIC, TEXT("Prototype_GameObject_CrackedDenture"), m_iCurrentScene, TEXT("Layer_CrackedDenture"), &tDentureInfo);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CDenture::OnCollisionEnter(CCollisionObject & collision)
{
}

void CDenture::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CDenture::OnTriggerExit(CCollisionObject & collision)
{
}

void CDenture::Update_LookVector(_double TimeDelta)
{
	if (!m_pModelCom->Get_IsAnimLerp())
	{
		if (1.0 > m_dLerpTime)
			m_dLerpTime += TimeDelta;
		else
			m_dLerpTime = 0.0;
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vAimLook = XMVector4Normalize(XMVectorLerp(vLook, XMLoadFloat4(&Get_TargetDir()), (_float)m_dLerpTime));
		_vector vRight = XMVector4Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vAimLook));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vAimLook);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	}
	else
		m_dLerpTime = 0.0;
}

HRESULT CDenture::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Denture0"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_CCharacterController */

	CCharacterController::DESC tCharacterControllerDesc;
	tCharacterControllerDesc.fHeight = 0.5f;
	tCharacterControllerDesc.fRadius = 0.5f;
	tCharacterControllerDesc.fStepOffset = 0.3f;
	tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
	tCharacterControllerDesc.fStaticFriction = 0.5f;
	tCharacterControllerDesc.fDynamicFriction = 0.5f;
	tCharacterControllerDesc.fRestitution = 0.f;
	tCharacterControllerDesc.vPosition = { 0.f, 1.f, 0.f };
	tCharacterControllerDesc.pGameObject = this;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_ChracterController"), TEXT("Com_Controller"), (CComponent**)&m_pCharacterController, &tCharacterControllerDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = false;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = { 1.f,1.f,1.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pHitColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pHitColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER);

	return S_OK;
}

HRESULT CDenture::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	_float fLightRange = 30.f;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
	{
		m_vRGBA = m_pRendererCom->Get_RimLightColorFloat4();
		m_fRimPower = m_pRendererCom->Get_RimLightPower();
	}
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &XMLoadFloat4x4(&WorldMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &XMLoadFloat4x4(&ViewMatrix), sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &XMLoadFloat4x4(&ProjMatrix), sizeof(_float4x4));

	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float4(-1.f, -1.f, -1.f, 0.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDenture * CDenture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDenture*	pInstance = new CDenture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDenture::Clone(void * pArg, _uint iCurrentScene)
{
	CDenture*	pInstance = new CDenture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDenture::Free()
{
	__super::Free();
	if (m_pHitColliderCom)
	{
		m_pHitColliderCom->Remove_Actor();
		Safe_Release(m_pHitColliderCom);
	}
}
