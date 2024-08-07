#include "stdafx.h"
#include "..\public\Cloud.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "FadeInOut.h"

CCloud::CCloud(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CCloud::CCloud(const CCloud & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCloud::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CCloud::NativeConstruct(void * pArg, _uint iSceneIndex)
{
	/* 좌표를 받아 올것이다. */

	if (FAILED(__super::NativeConstruct(pArg, iSceneIndex)))
		return E_FAIL;

	_vector pos;
	if (nullptr != pArg)
		pos = *(_vector*)pArg;

	m_StrLayerName = TEXT("Layer_Cloud");
	m_StrObjectFullName = TEXT("Prototype_GameObject_Cloud");
	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_CLOUD;
	m_eLayerType = LayerType::LAYERTYPE_CLOUD;

	m_fRangeForFrustum = 1.f;
	
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	//_float4x4 fWorldMatrix;
	//XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	//
	//m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

	SettingMovePostion();

	m_iRenderPass = 28;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;
	m_bIsRenderingShadow = true;

	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}

_int CCloud::Tick(_double TimeDelta)
{
	if (0 >(__super::Tick(TimeDelta)))
		return -1;

	m_TimeAcc += TimeDelta;

	if (0.2 <= m_TimeAcc)
	{
		m_TimeAcc = 0.f;
		m_fRatio += 120.f;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTransformCom->BillBoardY(pGameInstance->Get_CamPosition());
	RELEASE_INSTANCE(CGameInstance);
	
	_matrix     RotationMatrix = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRatio);
	_vector		vRight = XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), RotationMatrix);
	_vector		vUp = XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_UP), RotationMatrix);
	_vector		vLook = XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), RotationMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	if(m_bMove)
		Move_And_Create(TimeDelta);


	if (false == m_bMove)
	{

		if (true == m_MoveYCheck)
		{
			m_AccTimeYMove += TimeDelta;
			_vector Up_Postion;
			if (1.0 <= m_AccTimeYMove)
				m_AccTimeYMove = 1.0;

			Up_Postion = XMVectorLerp(XMLoadFloat3(&m_vEndPostion), XMVectorSet(m_vEndPostion.x, m_vEndPostion.y + 0.3f, m_vEndPostion.z, 1.f), m_AccTimeYMove);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Up_Postion);

			if (1.0 <= m_AccTimeYMove)
			{
				m_AccTimeYMove = 0.0;
				m_MoveYCheck = false;
			}
		}
		else
		{
			m_AccTimeYMove += TimeDelta;
			_vector Down_Postion;

			if (1.0 <= m_AccTimeYMove)
				m_AccTimeYMove = 1.0;

			Down_Postion = XMVectorLerp(XMVectorSet(m_vEndPostion.x, m_vEndPostion.y + 0.3f, m_vEndPostion.z, 1.f), XMLoadFloat3(&m_vEndPostion), m_AccTimeYMove);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Down_Postion);

			if (1.0 <= m_AccTimeYMove)
			{
				m_AccTimeYMove = 0.0;
				m_MoveYCheck = true;
			}
		}
	}



	if(m_pColliderCom)
		m_pColliderCom->Update_TransformPhysX();
	

	return _int();
}

_int CCloud::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	//if (m_pColliderCom)
	//	m_pColliderCom->Update_Transform();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_RECT_EFFECT, this);

	return _int();
}

HRESULT CCloud::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBuffer->Render(10);
	return S_OK;
}

void CCloud::SettingMovePostion()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_iMyCount = pGameInstance->Get_LayerSize(m_iCurrentScene, TEXT("Layer_Cloud"));
	RELEASE_INSTANCE(CGameInstance);
	
	
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE01:
	{
		SettingStage1(m_iMyCount);
		break;
	}
	case SCENE_STAGE02:
		break;
	case SCENE_STAGE03:
	{
		SettingStage3(m_iMyCount);
		break;
	}	
	case SCENE_STAGE04:
		break;
	case SCENE_STAGE05:
		break;
	}
}

void CCloud::SettingStage1(_uint iCount)
{
	switch (iCount)
	{
	case 0:
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(61.28f, 4.98f, 62.57f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 1:
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(61.62f, 8.33f, 59.05f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 2:
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(61.09f,13.73f,55.56f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 3:
		//XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		//XMStoreFloat3(&m_vEndPostion, XMVectorSet(44.10f,8.81f,63.53f, 1.f));
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(60.96f, 13.26f, 49.35f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 4:
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(56.75f, 14.08f, 43.53f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 5:
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(56.66f, 14.37f, 35.42f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 6: /* 이구름 생성시 좌물쇠 생성이 가능한 구름 */
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(50.89f, 14.94f, 28.97f, 1.f));
		m_bMove = true;
		m_bNextCreate = true;
		break;
	case 7: /* 이구름 생성시 도착후 바로 위에 문고리 생성  */
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position());
		XMStoreFloat3(&m_vEndPostion, XMVectorSet(44.27f, 14.30f, 31.67f, 1.f));
		m_bMove = true;
		m_bNextCreate = false;
		break;
	case 8: /* 이후 부터는 Stage1에 타고 다닐만한 것들을 여기 저기에다가 만들것입니다. */
	case 9:
	case 10: 
	case 11: 
	case 12:
		XMStoreFloat3(&m_vStartPostion, m_pTransformCom->Get_Position()); 
		m_bMove = false;
		m_bNextCreate = false;
		break;
	}
}

void CCloud::SettingStage3(_uint iCount)
{
	switch (iCount)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		XMStoreFloat3(&m_vEndPostion, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		break;
	}
}

void CCloud::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
}

void CCloud::Setting_Throwing(_fvector vDir)
{
}

void CCloud::OnCollisionEnter(CCollisionObject & collision)
{
}

void CCloud::OnCollisionStay(CCollisionObject & collision)
{
}

void CCloud::OnCollisionExit(CCollisionObject & collision)
{
}

void CCloud::OnTriggerEnter(CCollisionObject & collision)
{
}

void CCloud::SettingActorActive()
{
}

_bool CCloud::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
{
	return _bool();
}

void CCloud::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
}

void CCloud::MoveRoation(_double TimeDelta)
{
}

void CCloud::Move_And_Create(_double TimeDelta)
{
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE01:
	{
		Stage1_MoveLerp(TimeDelta);
		break;
	}
	case SCENE_STAGE02:
		break;
	case SCENE_STAGE03:
		break;
	case SCENE_STAGE04:
		break;
	case SCENE_STAGE05:
		break;
	}
}

void  CCloud::Stage1_MoveLerp(_double TimeDelta)
{
	if (2 == m_iMyCount)
		m_AccTime += TimeDelta * 0.5;
	else if (3 == m_iMyCount || 4 == m_iMyCount || 5 == m_iMyCount || 6 == m_iMyCount)
		m_AccTime += TimeDelta * 2.0;
	else
		m_AccTime += TimeDelta;


	if (1.0 <= m_AccTime)
		m_AccTime = 1.0;

	_vector vPostion = XMVectorLerp(XMLoadFloat3(&m_vStartPostion), XMLoadFloat3(&m_vEndPostion), m_AccTime);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPostion);

	if (1.0 <= m_AccTime && true == m_bNextCreate)
	{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
		_float3 vpos;

		XMStoreFloat3(&vpos, m_pTransformCom->Get_Position());
		pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), SCENE_STAGE01, TEXT("Layer_Cloud"), &XMVectorSet(vpos.x, vpos.y, vpos.z, 1.f));

		if (m_iMyCount == 1) /* 1번쨰 객체가 만들어지면 카메라 움직여라 */
		{
			((CFadeInOut*)pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Fade"), 0))->Active(0.5, _float4(0.f, 0.f, 0.f, 0.f), CFadeInOut::INOUT_OUT);
			static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_3);
		}

		if (m_iMyCount == 5) /* 5번쨰 객체가 만들어지면 카메라 문고리 쪽으로 가게할것이다.*/
			static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_5);


		m_AccTime = 0.0;
		m_bMove = false;
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (1.0 <= m_AccTime && 7 == m_iMyCount)
	{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);

		_float3 vpos;
		XMStoreFloat3(&vpos, m_pTransformCom->Get_Position());
		pGameInstnace->Add_ObjectToLayer(SCENE::SCENE_STAGE01, TEXT("Prototype_GameObject_Stage1File_Zip"), SCENE_STAGE01, TEXT("Layer_Zip"), &XMVectorSet(vpos.x, vpos.y + 1.f, vpos.z, 1.f));
		
		RELEASE_INSTANCE(CGameInstance);

		m_AccTime = 0.0;
		m_bMove = false;
	}
}

_vector CCloud::Get_CloudPos()
{
	return m_pTransformCom->Get_Position();
}

HRESULT CCloud::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Cloud"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_EffectRect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	//tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = false;
	tColliderDesc.isTrigger = true;/* 트리거 출동*/
	tColliderDesc.isSceneQuery = true; /* 물리충돌을 끄겠다. */
	//tColliderDesc.fRestitution = 0.f;

	tColliderDesc.pGameObject = this;
	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = MULTI_FLOAT3(_float3(0.5f, 0.5f, 0.5f), 1.99f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider0"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_CLOUD); /* 타입 추가 */
	_matrix smatPviot = XMMatrixIdentity();
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	m_isActorActive = false;

	SettingActorActive();
	return S_OK;
}

HRESULT CCloud::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBuffer->Set_RawValue("g_WorldMatrix",&WorldMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBuffer, 0);
	m_pVIBuffer->Set_RawValue("g_fRatio", &m_fRatio, sizeof(_float));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CCloud::OnControllerCollisionHit(CCollisionObject & collision)
{
}

CCloud * CCloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCloud*	pInstance = new CCloud(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CCloud");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCloud::Clone(void * pArg, _uint iSceneIndex)
{
	CCloud*	pInstance = new CCloud(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CCloud");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCloud::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransformCom);
}
