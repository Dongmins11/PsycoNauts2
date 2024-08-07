#include "stdafx.h"
#include "..\public\NpcLoboto.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include "CollisionObject.h"
#include "Player.h"

CNpcLoboto::CNpcLoboto(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CQuestNpc(pDevice,pDeviceContext)
{
}

CNpcLoboto::CNpcLoboto(const CNpcLoboto & rhs)
	: CQuestNpc(rhs)
{
}

HRESULT CNpcLoboto::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpcLoboto::NativeConstruct(void * pArg, _uint iCurrentScene)
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

	m_pModelCom->Set_Animation(1);
	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, matWorld);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_QUEST);
	m_pCurrentQuest = CQuestManager::Get_Instance()->Setting_QuestToNpc(2);
	m_iCurrent_QuestScriptNumber = m_pCurrentQuest->Get_QuestDesc().iScriptStartNum;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_SpinHole"),
		m_iCurrentScene, TEXT("Layer_Lobo_QuestOK"),
		SCENE_STATIC, TEXT("Effect_QuestOK"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Effect_Call_Inheritance(m_iCurrentScene, false, true, m_pTransformCom, TEXT("Layer_Lobo_QuestOK"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pGameInstance->Setting_Effect_MovePos(m_iCurrentScene, TEXT("Layer_Lobo_QuestOK"), TEXT("Com_Option"), m_pTransformCom->Get_State(CTransform::STATE_UP) * 3.5f);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int CNpcLoboto::Tick(_double TimeDelta)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
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


	//// �÷��̾ npc�� ��ȭ �����Ÿ� �ȿ� �ִ°�?
	if (m_isPlayerEnter && !m_isFinishQuest)
	{
		// Ű �ٿ����� 'F'�� Ŭ���Ͽ��°�?
		if (pGameInstance->Key_Down('F'))
		{
			// ���� ����Ʈ ��ũ��Ʈ â�� false�϶�
			if (!m_isScripting)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Delete_GameObject(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Lobo_QuestOK"), 0)))
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
				CQuestManager::Get_Instance()->Clear_Quest(1);
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
					((CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Setting_PlayerQuest(m_pCurrentQuest->Get_QuestDesc().iQuestNumber);
					
					/* ���⼭ ���� ����1 */
					/* ���� ���� ���ָ�ɵ� �����ɋ� �� �ѹ��� UI ����ϰ� �ϸ�ɵ� */
					m_IsReady = true;
				}
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

_int CNpcLoboto::LateTick(_double TimeDelta)
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

HRESULT CNpcLoboto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CNpcLoboto::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		m_isPlayerEnter = true;
	}
}


void CNpcLoboto::OnTriggerExit(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		m_isPlayerEnter = false;
	}
}

HRESULT CNpcLoboto::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Loboto"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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

HRESULT CNpcLoboto::SetUp_ConstantTable()
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

CNpcLoboto * CNpcLoboto::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNpcLoboto*	pInstance = new CNpcLoboto(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CNpcLoboto");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNpcLoboto::Clone(void * pArg, _uint iCurrentScene)
{
	CNpcLoboto*	pInstance = new CNpcLoboto(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CNpcLoboto");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNpcLoboto::Free()
{
	__super::Free();

	if (m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
