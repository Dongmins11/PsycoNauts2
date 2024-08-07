#include "stdafx.h"
#include "..\public\PPlate.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"
#include "CollisionObject.h"

#include "Choppig_PullHand.h"
#include "Choppig_HandKnife.h"

#include "Scene_Stage03.h"
#include "Blender.h"
#include "Fruit.h"
#include "GiantCloche.h"

CPPlate::CPPlate(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPPlate::CPPlate(const CPPlate & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPPlate::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPPlate::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;


	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_MyBoneMatrices("button")))
		return E_FAIL;


	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_eButtonState = BUTTON_STATE_DOWNIDLE;

	return S_OK;
}

_int CPPlate::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	PPlate_Animation();

	Colider_PosUpdate();

	m_pModelCom->Update_Finishied(TimeDelta * 0.5);

	return _int();
}

_int CPPlate::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CPPlate::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CPPlate::OnTriggerEnter(CCollisionObject & collision)
{
	if (BUTTON_STATE::BUTTON_STATE_UPIDLE == m_eButtonState && collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_SKILL)
	{
		if (BUTTON_TYPE_END != m_eButtonType)
		{
			switch (m_eButtonType)
			{
			case Client::CPPlate::BUTTON_TYPE_FRUIT:
				Button_MoveStage();
				break;
			case Client::CPPlate::BUTTON_TYPE_KNIFE:
				Button_KnifeType_Interaction();
				break;
			case Client::CPPlate::BUTTON_TYPE_MIXER:
				Button_BlenderStart();
				break;
			}


			m_eButtonState = BUTTON_STATE_DOWNANIM;
			m_eButtonType = BUTTON_TYPE_END;
		}
	}
}



HRESULT CPPlate::SetUp_Component()
{
	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	m_iMyCount = pGameInstnace->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_PPlate"));
	RELEASE_INSTANCE(CGameInstance);
	

	/* Com_Renderer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = false;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.fMass = PX_MAX_F32;
	m_eLayerType = LayerType::LAYERTYPE_PLANE;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(2.f, 2.f, 1.5f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_PhysXCollider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.4f, 0.f);

	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Set_ShapeLayer(m_eLayerType);
	m_pColliderCom->Reset_Power();
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();


	return S_OK;
}

HRESULT CPPlate::SetUp_ConstantTable()
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

HRESULT CPPlate::SetUp_MyBoneMatrices(const char * pBoneName)
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("Failed to CPlayer : Funtion SetUp_MyBoneMatrices");
		return E_FAIL;
	}

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_pCombinedMatrix = m_pModelCom->Get_CombinedMatrixPtr(pBoneName);
	m_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPPlate::Colider_PosUpdate()
{
	Bone_MatrixCalculation();

	m_pColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
	m_pColliderCom->Update_Transform(m_pBoneMatrix);

	return S_OK;
}

HRESULT CPPlate::Bone_MatrixCalculation()
{
	if (nullptr != m_pCombinedMatrix)
	{
		_matrix ColiderMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_pTransformCom->Get_WorldFloat4x4());
		XMStoreFloat4x4(&m_ColiderMatirx, ColiderMatrix);
		m_pBoneMatrix = &m_ColiderMatirx;
	}

	return S_OK;
}

HRESULT CPPlate::PPlate_Animation()
{
	if (FAILED(Animation_StateCheck()))
		return E_FAIL;

	switch (m_eButtonState)
	{
	case Client::CPPlate::BUTTON_STATE_DOWNIDLE:
		m_iCurAnimIndex = BUTTON_DOWNIDLE;
		break;

	case Client::CPPlate::BUTTON_STATE_DOWNANIM:
		m_iCurAnimIndex = BUTTON_DOWNANIM;

		if (m_pModelCom->Get_IsAnimFinishied(BUTTON_DOWNANIM))
		{
			m_eButtonState = BUTTON_STATE_DOWNIDLE;
			m_iCurAnimIndex = BUTTON_DOWNIDLE;
		}
		break;

	case Client::CPPlate::BUTTON_STATE_UPIDLE:
		m_iCurAnimIndex = BUTTON_UPIDLE;
		break;

	case Client::CPPlate::BUTTON_STATE_UPANIM:
		m_iCurAnimIndex = BUTTON_UPANIM;

		if (m_pModelCom->Get_IsAnimFinishied(BUTTON_UPANIM))
		{
			m_eButtonState = BUTTON_STATE_UPIDLE;
			m_iCurAnimIndex = BUTTON_UPIDLE;
		}
		break;
	}



	//if (m_iPreAnimIndex != m_iCurAnimIndex)
	//{
	//	m_pModelCom->Set_IsAnimLefp(true);
	//	m_iPreAnimIndex = m_iCurAnimIndex;
	//}
	m_pModelCom->Set_Animation(m_iCurAnimIndex);

	return S_OK;
}

HRESULT CPPlate::Animation_StateCheck()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (true == m_IsUpStateCheck && BUTTON_STATE_DOWNIDLE == m_eButtonState)
	{
		if (m_eButtonType == m_iMyCount)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_BUTTON)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}
			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("ButtonOn.wav"), CHANNELID::PLAYER_BUTTON)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return E_FAIL;
			}

			RELEASE_INSTANCE(CGameInstance);

			m_eButtonState = BUTTON_STATE_UPANIM;
			m_IsUpStateCheck = false;
		}
	}
	
	return S_OK;
}

HRESULT CPPlate::Button_KnifeType_Interaction()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pHandKnifeObject = pGameInstance->Get_GameObject(SCENE_STAGE03, L"Layer_HandKnife", 0);
	CGameObject* pHandSwitchObject = pGameInstance->Get_GameObject(SCENE_STAGE03, L"Layer_Choppig", 1);

	if (nullptr == pHandKnifeObject || nullptr == pHandSwitchObject)
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	dynamic_cast<CChoppig_PullHand*>(pHandSwitchObject)->Set_SwitchButtonCheck(true);
	dynamic_cast<CChoppig_HandKnife*>(pHandKnifeObject)->Set_ButtonCheck(true);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPPlate::Button_MoveStage()
{
	if (m_isMoveScene)
		return S_OK;
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//CScene_Stage03* pStage = (CScene_Stage03*)pGameInstance->Get_CurrentScene();
	//if (((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Stage03PhaseNum() != 2)
	//{
	//	pStage->Setting_StageUpdate(SCENE_STAGE04);
	//}
	//else
	//	pStage->Setting_StageUpdate(SCENE_STAGE05);
	//
	//m_isMoveScene = true;
	//RELEASE_INSTANCE(CGameInstance);
	
	/* ƒ∆æ¿ ¿·±Ò ∏ÿ√„*/
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	//static_cast<CGiantCloche*>(pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_GiantCloche"), 0))->Set_Giant_Move(true);
	static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Stage3_CUT(CCamera_Fly::STAGE3_CUT_START);
	static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_STAGE3);
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPPlate::Button_BlenderStart()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CBlender* pBlender = (CBlender*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Blender6"), 0);
	if (pBlender)
	{
		pBlender->Get_Blender_Fruit()->Set_StartBlending();
		pBlender->Set_Create_Effect(true);
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CPPlate * CPPlate::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPPlate*	pInstance = new CPPlate(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPPlate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPPlate::Clone(void * pArg, _uint iCurrentScene)
{
	CPPlate*	pInstance = new CPPlate(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CPPlate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPPlate::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pModelCom);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
}
