#include "stdafx.h"
#include "..\public\Choppig_PullHand.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Choppig_HandKnife.h"
#include "PPlate.h"
#include "Player.h"
#include "Camera_Fly.h"

CChoppig_PullHand::CChoppig_PullHand(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CChoppig_PullHand::CChoppig_PullHand(const CChoppig_PullHand & rhs)
	: CGameObject(rhs)
{
}

HRESULT CChoppig_PullHand::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChoppig_PullHand::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	_float3 vColSize = { 0.5f, 0.5f, 0.5f };

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

	m_fPositionLook = 0.5f;
	m_iMaxDestination = 7;

	XMStoreFloat4(&m_vBackUpPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vScale,m_pTransformCom->Get_Scale());

	return S_OK;
}

_int CChoppig_PullHand::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;


	return _int();
}

_int CChoppig_PullHand::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (0 == m_iMyCount) //¿øÅë
	{
		Cylinder_ScaleUpdate(TimeDelta);
	}
	else if (1 == m_iMyCount) //¼Õ
	{

		if (FAILED(Colider_PosUpdate()))
			return -1;

		if (FAILED(Model_PosUpdate(TimeDelta)))
			return -1;

		BackUp_PositionUpdate(TimeDelta);

	}

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CChoppig_PullHand::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}


void CChoppig_PullHand::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
		if (nullptr == pPlayerObject)
			return;

		dynamic_cast<CPlayer*>(pPlayerObject)->Set_PullSwitch(true);

		Safe_Release(pGameInstance);
	}

}

void CChoppig_PullHand::OnTriggerExit(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
		if (nullptr == pPlayerObject)
			return;

		dynamic_cast<CPlayer*>(pPlayerObject)->Set_PullSwitch(false);

		Safe_Release(pGameInstance);
	}
}



HRESULT CChoppig_PullHand::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);
	m_iMyCount = pGameInstnace->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Choppig"));
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (0 == m_iMyCount)
		m_iMyTargetCount = 1;
	else
		m_iMyTargetCount = 0;

	if (1 == m_iMyCount)
	{

		_float vLength = GET_FLOAT3LENGTH(vColSize);
		if (vLength > 0.f)
		{
			CPhysXCollider::DESC tColliderDesc;

			tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
			tColliderDesc.isVisualization = true;
			tColliderDesc.isTrigger = true;
			tColliderDesc.pGameObject = this;
			tColliderDesc.isSceneQuery = true;

			CSphereCollider::DESC tBoxColliderDesc;
			tBoxColliderDesc.tColliderDesc = tColliderDesc;
			tBoxColliderDesc.vScale = vColSize;

			if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
				return E_FAIL;

			m_eLayerType = LayerType::LAYERTYPE_TRIGGER;
			m_pColliderCom->Set_ShapeLayer(m_eLayerType);

			m_pColliderCom->Update_TransformPhysX();
			m_pColliderCom->Update_Transform();
		}


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
		tBoxColliderDesc.vScale = _float3(3.f, 3.f, 5.f);

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_PhysXCollider"), (CComponent**)&m_pPhysXColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		_matrix smatPviot = XMMatrixTranslation(0.f, 0.7f, 0.f);

		m_pPhysXColliderCom->SetPivotMatrix(smatPviot);
		m_pPhysXColliderCom->Set_ShapeLayer(m_eLayerType);
		m_pPhysXColliderCom->Reset_Power();
		m_pPhysXColliderCom->Update_TransformPhysX();
		m_pPhysXColliderCom->Update_Transform();
	}


	return S_OK;
}

HRESULT CChoppig_PullHand::SetUp_ConstantTable()
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

HRESULT CChoppig_PullHand::Colider_PosUpdate()
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	_float4x4	ColiderMatrix[2];
	_float4x4*	HandColiderMatrix = nullptr;
	_matrix		ColiderMat = m_pTransformCom->Get_WorldMatrix();
	_matrix		PhysXColiderMat = m_pTransformCom->Get_WorldMatrix();

	ColiderMat.r[3] = ColiderMat.r[3] + (XMVector3Normalize(ColiderMat.r[2]) * 0.05f);
	XMStoreFloat4x4(&ColiderMatrix[1], ColiderMat);
	m_pColiderMatrix = &ColiderMatrix[0];

	PhysXColiderMat.r[3] = PhysXColiderMat.r[3] - (XMVector3Normalize(PhysXColiderMat.r[2]) * 2.4f);
	XMStoreFloat4x4(&ColiderMatrix[1], PhysXColiderMat);
	m_pPhysXColiderMatrix = &ColiderMatrix[1];


	m_pColliderCom->Update_TransformPhysX(ColiderMat);
	m_pColliderCom->Update_Transform(m_pColiderMatrix);


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);

	if (false == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchMove(CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD) &&
		false == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchMove(CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD))
	{
		m_pPhysXColliderCom->Update_TransformPhysX(PhysXColiderMat);
		m_pPhysXColliderCom->Update_Transform(m_pPhysXColiderMatrix);
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CChoppig_PullHand::Model_PosUpdate(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
	CTransform*	pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, L"Layer_Player", L"Com_Transform");
	if (nullptr == pPlayerObject || nullptr == pPlayerTrans)
		return E_FAIL;

	_float4 vTargetPosition;
	_float4	vMyPosition;

	if (true == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchMove(CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD) ||
		true == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchMove(CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD))
	{
		XMStoreFloat4(&vTargetPosition, pPlayerTrans->Get_Position() + (XMVector4Normalize(pPlayerTrans->Get_State(CTransform::STATE_LOOK)) * m_fPositionLook));
		XMStoreFloat4(&vMyPosition, m_pTransformCom->Get_Position());
		vTargetPosition.y = vMyPosition.y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vTargetPosition));
		m_isMoveCheck = true;
	}
	else
		m_isMoveCheck = false;


	Safe_Release(pGameInstance);

	return S_OK;
}

void CChoppig_PullHand::BackUp_PositionUpdate(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
	if (nullptr == pPlayerObject)
		return;

	if (true == m_bSwitchButtonCheck)
	{
		if (false == m_bFirstBackUpCheck)
		{
			XMStoreFloat4(&m_vCurrentBackUpPosition, m_pTransformCom->Get_Position());
			m_bFirstBackUpCheck = true;
		}


		m_lfBackUpTimeAcc += TimeDelta * 0.2;

		_vector vPosition = XMVectorLerp(XMLoadFloat4(&m_vCurrentBackUpPosition), XMLoadFloat4(&m_vBackUpPosition), (_float)m_lfBackUpTimeAcc);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);


		if (1.0 <= m_lfBackUpTimeAcc)
		{
			m_bSwitchButtonCheck = false;
			m_bFirstBackUpCheck = false;
			m_lfBackUpTimeAcc = 0.0;
		}
	}

	Safe_Release(pGameInstance);
}

void CChoppig_PullHand::Cylinder_ScaleUpdate(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform* pTargetTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STAGE03, L"Layer_Choppig", L"Com_Transform",m_iMyTargetCount));
	CGameObject* pTargetObejct = pGameInstance->Get_GameObject(SCENE_STAGE03, L"Layer_HandKnife", 0);
	CGameObject* pPlayerObject = pGameInstance->Get_GameObject(SCENE_STATIC, L"Layer_Player", 0);
	if (nullptr == pTargetTransform || nullptr == pTargetObejct)
	{
		Safe_Release(pGameInstance);
		return;
	}

	_vector	vLength = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - (pTargetTransform->Get_State(CTransform::STATE_POSITION) - (XMVector4Normalize(pTargetTransform->Get_State(CTransform::STATE_LOOK)) * 1.5));
	_float	fLength = XMVectorGetX(XMVector3Length(vLength));

	if (true == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchMove(CPlayer::SWITCHMOVE::SWITCHMOVE_FORWARD) ||
		true == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchMove(CPlayer::SWITCHMOVE::SWITCHMOVE_BACKWARD))
	{
		_double lfHandDuration = dynamic_cast<CChoppig_HandKnife*>(pTargetObejct)->Get_HandAnimDuration();
		_float fRatio = (_float)(m_iMaxDestination / lfHandDuration);
		_uint iKeyFrame = (_uint)(fLength / fRatio);

		if (50 > iKeyFrame)
			dynamic_cast<CChoppig_HandKnife*>(pTargetObejct)->Set_KeyFrameIndex(iKeyFrame);

		if (iKeyFrame >= lfHandDuration)
		{
			if (false == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchingEnd())
			{
				dynamic_cast<CPlayer*>(pPlayerObject)->Set_PullSwitchingEnd(true);

				CGameObject* pPPlate = pGameInstance->Get_GameObject(SCENE::SCENE_STAGE03, L"Layer_PPlate", (_uint)CPPlate::BUTTON_TYPE_KNIFE);

				if (nullptr == pPPlate)
					return;

				dynamic_cast<CPPlate*>(pPPlate)->Set_ButtonUpStateCheck(CPPlate::BUTTON_TYPE::BUTTON_TYPE_KNIFE, true);
			}
		}
		else
		{
			if (true == dynamic_cast<CPlayer*>(pPlayerObject)->Get_PullSwitchingEnd())
				dynamic_cast<CPlayer*>(pPlayerObject)->Set_PullSwitchingEnd(false);
		}
	}


	m_vScale.z = fLength * 1.5f;

	m_pTransformCom->Scaling(m_vScale);

	Safe_Release(pGameInstance);

}

CChoppig_PullHand * CChoppig_PullHand::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CChoppig_PullHand*	pInstance = new CChoppig_PullHand(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CChoppig_PullHand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChoppig_PullHand::Clone(void * pArg, _uint iCurrentScene)
{
	CChoppig_PullHand*	pInstance = new CChoppig_PullHand(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CChoppig_PullHand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChoppig_PullHand::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pModelCom);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);

	if (nullptr != m_pPhysXColliderCom)
		m_pPhysXColliderCom->Remove_Actor();

	Safe_Release(m_pPhysXColliderCom);

}
