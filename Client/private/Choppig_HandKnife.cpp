#include "stdafx.h"
#include "..\public\Choppig_HandKnife.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"
#include "Camera_Fly.h"

CChoppig_HandKnife::CChoppig_HandKnife(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CChoppig_HandKnife::CChoppig_HandKnife(const CChoppig_HandKnife & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChoppig_HandKnife::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChoppig_HandKnife::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float3 vColSize = { 1.f, 0.5f, 0.5f };

	_float3 vObjScale = { 1.f,1.f,1.f };

	m_vColPos = { 0.f, 0.f, 1.0f };

	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;


	if (FAILED(SetUp_Component(vColSize, m_vColPos, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	if (FAILED(SetUp_MyBoneMatrices("KnifeBone")))
		return E_FAIL;

	m_iCurKeyFrame = 0;

	m_bFinishied = false;

	m_pModelCom->Set_AnimBlendSpeed(2, 5.0);

	m_pModelCom->Set_Animation(2);

	return S_OK;
}

_int CChoppig_HandKnife::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;


	if (false == m_bButtonEnable)
		m_pModelCom->SetKeyFrame_Update(TimeDelta, m_iCurKeyFrame, &m_bFinishied);

	if (true == m_bButtonCheck)
	{
		m_pModelCom->Set_ResetAnimation_KeyFrame(0);
		m_pModelCom->Set_IsAnimLefp(true);
		m_iCurKeyFrame = 0;

		m_bButtonEnable = true;
		m_bButtonCheck = false;
	}


	if (true == m_bButtonEnable)
	{
		if (1 != m_pModelCom->Get_AnimIndex() && 0 != m_pModelCom->Get_AnimIndex())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->StopSound(CHANNELID::PLAYER_CHOP_PIG)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return 0;
			}

			if (FAILED(pGameInstance->PlaySoundOnce(TEXT("PigChopSound.wav"), CHANNELID::PLAYER_CHOP_PIG)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return 0;
			}
			RELEASE_INSTANCE(CGameInstance);


			m_pModelCom->Set_Animation(1);
		}

		if (true == m_pModelCom->Get_IsAnimFinishied(1))
		{
			m_pModelCom->Set_Animation(0);


		}
		else if (true == m_pModelCom->Get_IsAnimFinishied(0))
		{
			m_bButtonEnable = false;
			m_pModelCom->Set_Animation(2);

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
			if (nullptr == pPlayerObject)
			{
				Safe_Release(pGameInstance);
				return 0;
			}

			if (true == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchingEnd())
				dynamic_cast<CPlayer*>(pPlayerObject)->Set_PullSwitchingEnd(false);

			Safe_Release(pGameInstance);
		}

		if (0 == m_pModelCom->Get_AnimIndex())
			m_pModelCom->Lerp_Update(TimeDelta * 2);
		else
			m_pModelCom->Lerp_Update(TimeDelta * 2);

	}



	return _int();
}

_int CChoppig_HandKnife::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CChoppig_HandKnife::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

const _double CChoppig_HandKnife::Get_HandAnimDuration() const
{
	if (nullptr == m_pModelCom)
		return 0.0;

	const _double Duration = m_pModelCom->Get_Animation_Duration();

	return Duration;
}

const _bool CChoppig_HandKnife::Get_KnifeAnimFinished() const
{
	if (nullptr == m_pModelCom)
		return false;

	return m_pModelCom->Get_AnimNextFinish(1);
}


HRESULT CChoppig_HandKnife::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
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


	/*CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;

	m_eLayerType = LayerType::LAYERTYPE_TRIGGER;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = { 3.f,2.f,3.f };

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_PhysXpColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.0f, 0.f);

	m_PhysXpColliderCom->SetPivotMatrix(smatPviot);
	m_PhysXpColliderCom->Set_ShapeLayer(m_eLayerType);
	m_PhysXpColliderCom->Reset_Power();
	m_PhysXpColliderCom->Update_TransformPhysX();
	m_PhysXpColliderCom->Update_Transform();
*/


	return S_OK;
}

HRESULT CChoppig_HandKnife::SetUp_ConstantTable()
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

HRESULT CChoppig_HandKnife::SetUp_MyBoneMatrices(const char * pBoneName)
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

HRESULT CChoppig_HandKnife::Colider_PosUpdate()
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;


	return S_OK;
}

HRESULT CChoppig_HandKnife::Model_PosUpdate()
{


	return S_OK;
}

HRESULT CChoppig_HandKnife::Bone_MatrixCalculation()
{
	if (nullptr != m_pCombinedMatrix)
	{
		_matrix ColiderMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_pTransformCom->Get_WorldFloat4x4());
		XMStoreFloat4x4(&m_ColiderMatirx, ColiderMatrix);
		m_pBoneMatrix = &m_ColiderMatirx;

		m_PhysXpColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
		m_PhysXpColliderCom->Update_Transform(m_pBoneMatrix);
	}

	return S_OK;
}


CChoppig_HandKnife * CChoppig_HandKnife::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChoppig_HandKnife*	pInstance = new CChoppig_HandKnife(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CChoppig_HandKnife");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChoppig_HandKnife::Clone(void * pArg, _uint iCurrentScene)
{
	CChoppig_HandKnife*	pInstance = new CChoppig_HandKnife(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CChoppig_HandKnife");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChoppig_HandKnife::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pModelCom);

	Safe_Release(m_PhysXpColliderCom);
}
