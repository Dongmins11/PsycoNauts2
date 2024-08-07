#include "stdafx.h"
#include "..\public\NpcCoach.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include "CollisionObject.h"
#include "Player.h"
CNpcCoach::CNpcCoach(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CQuestNpc(pDevice,pDeviceContext)
{
}

CNpcCoach::CNpcCoach(const CNpcCoach & rhs)
	: CQuestNpc(rhs)
{
}

HRESULT CNpcCoach::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpcCoach::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_matrix matWorld = *(_matrix*)pArg;

	_float3 vColSize = {2.f,2.f,2.f};
	_float3 vColPos = { 0.f,0.f,0.f };
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = 1.f;

	if (FAILED(SetUp_Component(vColSize, vColPos, _float3(1.f,1.f,1.f))))
		return E_FAIL;

	m_pModelCom->Set_Animation(0);
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, matWorld);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_QUEST);
	m_pCurrentQuest = CQuestManager::Get_Instance()->Setting_QuestToNpc(0);
	m_iCurrent_QuestScriptNumber = m_pCurrentQuest->Get_QuestDesc().iScriptStartNum;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_SpinHole"),
		m_iCurrentScene, TEXT("Layer_Coach_QuestOK"),
		SCENE_STATIC, TEXT("Effect_QuestOK"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, true, m_pTransformCom, TEXT("Layer_Coach_QuestOK"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->Setting_Effect_MovePos(m_iCurrentScene, TEXT("Layer_Coach_QuestOK"), TEXT("Com_Option"), m_pTransformCom->Get_State(CTransform::STATE_UP) * 2.f);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CNpcCoach::Tick(_double TimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (true == pGameInstance->Key_Down(VK_NUMPAD5)) /* ����  ->CLOUD_START    ���� �� ������ CLOUD_11   ���۶� �浹�� �Ǹ� CLOUD_16  */
	//{
	//	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	//	static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_CLOUD);
	//	static_cast<CCamera_Fly*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_START);
	//	RELEASE_INSTANCE(CGameInstance);
	//}
	//if (true == pGameInstance->Key_Down(VK_NUMPAD6)) /* ���� ��������� 11�� */
	//{
	//	//CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	//	/*static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_11);
	//	static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_CLOUD);*/
	//	//RELEASE_INSTANCE(CGameInstance);
	//}


	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		if (!m_isRender && m_pColliderCom)
		{
			m_pColliderCom->Add_Actor();
		}
		m_isRender = true;
	}
	else
	{
		if (m_isRender && m_pColliderCom)
		{
			m_pColliderCom->Remove_Actor();
		}
		m_isRender = false;
	}


	

	if (!m_isRender)
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}

	m_pModelCom->Update(TimeDelta);

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
	}

	// �÷��̾ npc�� ��ȭ �����Ÿ� �ȿ� �ִ°�?
	if (m_isPlayerEnter && !m_isFinishQuest)
	{
		// Ű �ٿ����� 'F'�� Ŭ���Ͽ��°�?
		if (pGameInstance->Key_Down('F'))
		{
			// ���� ����Ʈ ��ũ��Ʈ â�� false�϶�
			if (!m_isScripting)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Coach_QuestOK"), 0)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return -1;
				}
				RELEASE_INSTANCE(CGameInstance);
				// ��ũ��Ʈ â true
				m_isScripting = true;
				// ���� Ŵ
				pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestScript"), 0)->Set_IsRender(true);
				pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestScript"), 1)->Set_IsRender(true);
				// ���� ��ũ��Ʈ�� number�� �Ѱ���
				pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestScript"), 1)->Set_RenderPass(m_iCurrent_QuestScriptNumber);
			}
			else
			{
				if (m_iCurrent_QuestScriptNumber != m_pCurrentQuest->Get_QuestDesc().iScriptEndNum)
					pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestScript"), 1)->Set_RenderPass(++m_iCurrent_QuestScriptNumber);
				else // �������϶� �÷��̾�� ����Ʈ�� �ָ鼭 ����Ʈâ ����
				{
					m_isFinishQuest = true;
					pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestScript"), 0)->Set_IsRender(false);
					pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_QuestScript"), 1)->Set_IsRender(false);
					((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Setting_PlayerQuest(m_pCurrentQuest->Get_QuestDesc().iQuestNumber,true);
					CQuestManager::Get_Instance()->Setting_SelectedQuest(0);
					static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_CameraType(CCamera_Fly::CAMERTYPE_CLOUD);
					static_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Set_Cloud_CAM(CCamera_Fly::CLOUD_START);
				}
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

_int CNpcCoach::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	if (!m_isRender)
		return 0;

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_Transform();
	}
	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	return 0;
}

HRESULT CNpcCoach::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CNpcCoach::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		m_isPlayerEnter = true;
	}
}

void CNpcCoach::OnTriggerExit(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		m_isPlayerEnter = false;
	}
}

void CNpcCoach::Quest_RenderScripting()
{
}

HRESULT CNpcCoach::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Coach"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
		tColliderDesc.isSceneQuery = true;
		tColliderDesc.pGameObject = this;
		tColliderDesc.isTrigger = true;
		CSphereCollider::DESC tSphereColliderDesc;
		tSphereColliderDesc.tColliderDesc = tColliderDesc;
		tSphereColliderDesc.vScale = {2.f,2.f,2.f};

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tSphereColliderDesc)))
			return E_FAIL;

		_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
		m_pColliderCom->SetPivotMatrix(smatPviot);
		m_pColliderCom->Set_ShapeLayer(m_eLayerType);
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}
	return S_OK;
}

HRESULT CNpcCoach::SetUp_ConstantTable()
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

CNpcCoach * CNpcCoach::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNpcCoach*	pInstance = new CNpcCoach(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CNpcCoach");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNpcCoach::Clone(void * pArg, _uint iCurrentScene)
{
	CNpcCoach*	pInstance = new CNpcCoach(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CNpcCoach");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNpcCoach::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
