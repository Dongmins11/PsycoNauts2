#include "stdafx.h"
#include "..\public\HaroldMouse.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "Player.h"
#include "CollisionObject.h"

CHaroldMouse::CHaroldMouse(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CHaroldMouse::CHaroldMouse(const CHaroldMouse & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHaroldMouse::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CHaroldMouse::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(1.3f, 1.3f, 1.3f));

	//m_pCharacterController->Set_FootPosition(_float3(/*-131.f, 0.8f, 107.f*/-17.f, 1.1f, 40.34f));
	m_pCharacterController->Set_FootPosition(_float3(-21.6609859f, 1.14059436f, 40.2531700f));

	m_pColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PICK);

	ZeroMemory(&m_tAnimDesc, sizeof(KEYDESC));

	m_lfMoveSpeed = 0.8f;

	m_iAnimIndex = 6;

	m_pModelCom->Set_Animation(m_iAnimIndex);

	m_pModelCom->Set_AnimBlendSpeed(6, 3.f);
	m_pModelCom->Set_AnimBlendSpeed(9, 3.f);
	m_pModelCom->Set_AnimBlendSpeed(10, 3.f);

	m_bCutSceneStart = false;

	m_isRender = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_SpinHole"),
		m_iCurrentScene, TEXT("Layer_MouseHeadIconRect"),
		SCENE_STATIC, TEXT("Effect_MouseHeadIconRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, true, m_pTransformCom, TEXT("Layer_MouseHeadIconRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->Setting_Effect_MovePos(m_iCurrentScene, TEXT("Layer_MouseHeadIconRect"), TEXT("Com_Option"), m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.f);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CHaroldMouse::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	m_pTransformCom->Set_Velocity(XMVectorZero());


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_bool bChangeMouseAnimCheck = false;

	//if (pGameInstance->Key_Down(VK_NUMPAD6))
	//{
	//	CGameObject* pCameraFly = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Camera", 0);

	//	dynamic_cast<CCamera_Fly*>(pCameraFly)->Set_CameraType(CCamera_Fly::CAMERTYPE_HAROLDMOUSE);
	//	dynamic_cast<CCamera_Fly*>(pCameraFly)->Set_HarOldMouse(CCamera_Fly::HAROLDMOUSE_START);
	//}

	if (pGameInstance->Key_Down(VK_LSHIFT))
		m_bTrigerCheck = true;

	if (bChangeMouseAnimCheck)
		m_pModelCom->Set_IsAnimLefp(true);

	RELEASE_INSTANCE(CGameInstance);


	if (FAILED(Brian_Washing(TimeDelta)))
		return -1;

	if (FAILED(Cut_Scene_Anim(TimeDelta)))
		return -1;

	//if (!m_pCharacterController->IsDown())
	//	m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -3.0f);

	m_pModelCom->Lerp_Update(TimeDelta);

	m_pCharacterController->Move(TimeDelta, m_pTransformCom->Get_Velocity());

	m_pCharacterController->Update_Transform();


	if (false == m_isPicking)
		m_pColliderCom->Update_TransformPhysX();

	return _int();
}

_int CHaroldMouse::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;


	if (false == m_isPicking)
		m_pColliderCom->Update_Transform();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CHaroldMouse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

HRESULT CHaroldMouse::Key_Check(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing('W'))
		m_pTransformCom->Go_Straight(TimeDelta * 0.3, m_pCharacterController);
	if (pGameInstance->Key_Pressing('S'))
		m_pTransformCom->Go_Backward(TimeDelta * 0.3, m_pCharacterController);
	if (pGameInstance->Key_Pressing('A'))
		m_pTransformCom->RotationPerSec(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta);
	if (pGameInstance->Key_Pressing('D'))
		m_pTransformCom->RotationPerSec(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CHaroldMouse::Brian_Washing(_double TimeDelta)
{
	if (false == m_bCutSceneStart)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == m_isPicking)
		{
			CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
			if (nullptr == pPlayerObject)
			{
				MSGBOX("Failed to Finder pPlayerObject : Function CHaroldMouse : Tick");
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			dynamic_cast<CPlayer*>(pPlayerObject)->Set_MoveCheck(true);
			m_pCharacterController->SetHeight(0.006f);

			if (FAILED(Key_Check(TimeDelta)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
		}

		if (true == m_bTrigerCheck)
		{
			CGameObject* pCameraFly = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Camera", 0);
			CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);

			if (nullptr == pCameraFly || nullptr == pPlayerObject)
			{
				MSGBOX("Failed to Finder CamFly or  pPlayerObject : Function CHaroldMouse : Tick");
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			dynamic_cast<CCamera_Fly*>(pCameraFly)->Set_CameraType(CCamera_Fly::CAMERATYPE_DEPENDENT);
			dynamic_cast<CCamera_Fly*>(pCameraFly)->Set_Fovy(70.f);
			dynamic_cast<CPlayer*>(pPlayerObject)->Set_RenderCheck(false);
			dynamic_cast<CPlayer*>(pPlayerObject)->Set_MoveCheck(false);

			m_bTrigerCheck = false;
			m_isPicking = false;

			m_pRendererCom->Set_IsSSAA(true);
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

HRESULT CHaroldMouse::Cut_Scene_Anim(_double TimeDelta)
{
	//if (true == m_bCutSceneStart)
	//{





	//}

	return S_OK;
}


HRESULT CHaroldMouse::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_HaroldMouse"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.0f, 0.75f, 0.0f);
	ColliderDesc.vExtents = _float3(0.35f, 0.75f, 0.35f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;



	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.0f, 1.0f, 0.0f);
	ColliderDesc.vExtents = _float3(0.4f, 1.0f, 0.4f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For. CCT*/
	//CCharacterController::DESC tCharacterControllerDesc;
	//tCharacterControllerDesc.fHeight = m_fHeight = 0.5f;
	//tCharacterControllerDesc.fRadius = 0.5f;
	//tCharacterControllerDesc.fStepOffset = 0.1f;
	//tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
	//tCharacterControllerDesc.fStaticFriction = 0.5f;
	//tCharacterControllerDesc.fDynamicFriction = 0.5f;
	//tCharacterControllerDesc.fRestitution = 0.f;
	//tCharacterControllerDesc.vPosition = { 0.f, 0.f, 0.f };
	//tCharacterControllerDesc.pGameObject = this;
	//만약안되면 전꺼로 돌리고
	//if (!m_pCharacterController->IsDown())
	//m_pTransformCom->Add_Velocity(CTransform::STATE_UP, -3.0f);

	CCharacterController::DESC tCharacterControllerDesc;
	tCharacterControllerDesc.fHeight = m_fHeight = 0.1f;
	tCharacterControllerDesc.fRadius = 0.1f;
	tCharacterControllerDesc.fStepOffset = 0.0f;
	tCharacterControllerDesc.fContactOffset = tCharacterControllerDesc.fRadius * 0.01f;
	tCharacterControllerDesc.fStaticFriction = 0.5f;
	tCharacterControllerDesc.fDynamicFriction = 0.5f;
	tCharacterControllerDesc.fRestitution = 0.f;
	tCharacterControllerDesc.vPosition = { 0.f, 0.5f, 0.f };
	tCharacterControllerDesc.pGameObject = this;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_ChracterController"), TEXT("Com_Controller"), (CComponent**)&m_pCharacterController, &tCharacterControllerDesc)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = m_pTransformCom->Get_ScaleFloat3();

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();


	return S_OK;
}

HRESULT CHaroldMouse::SetUp_ConstantTable()
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

HRESULT CHaroldMouse::SetUp_TargetBoneMatrices(const char * pBoneName)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4				OffsetMatrix;
	_float4x4				PivotMatrix;
	const _float4x4*		pCombinedMatrix = nullptr;
	const _float4x4*		pTargetWorldMatrix;
	_matrix					CamWorldMat;

	CGameObject* pCameraFly = pGameInstance->Get_GameObject(/*m_iCurrentScene*/SCENE_STATIC, L"Layer_Camera", 0);
	if (nullptr == pCameraFly)
	{
		MSGBOX("Failed to Finder CamFly : Function CHaroldMouse : SetUp_TargetBoneMatrices");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	CTransform*	pTargetTransform = (CTransform*)pGameInstance->Get_Components(/*m_iCurrentScene*/SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTargetTransform)
	{
		MSGBOX("Failed to Finder pTargetTransform : Function CHaroldMouse : SetUp_TargetBoneMatrices");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	CModel*		pModel = (CModel*)pGameInstance->Get_Components(/*m_iCurrentScene*/SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Model"), 0);
	if (nullptr == pModel)
	{
		MSGBOX("Failed to Finder pModel : Function CHaroldMouse : SetUp_TargetBoneMatrices");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	OffsetMatrix = pModel->Get_OffsetMatrix(pBoneName);
	PivotMatrix = pModel->Get_PivotMatrix();
	pCombinedMatrix = pModel->Get_CombinedMatrixPtr(pBoneName);
	pTargetWorldMatrix = pTargetTransform->Get_WorldMatrixPtr();

	CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHaroldMouse * CHaroldMouse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHaroldMouse*	pInstance = new CHaroldMouse(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CHaroldMouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHaroldMouse::Clone(void * pArg, _uint iSceneIndex)
{
	CHaroldMouse*	pInstance = new CHaroldMouse(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CHaroldMouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHaroldMouse::Free()
{
	__super::Free();

	if (m_pCharacterController)
		m_pCharacterController->Remove_CharacterController();
	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pCharacterController);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
