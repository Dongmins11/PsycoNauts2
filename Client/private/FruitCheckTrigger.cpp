#include "stdafx.h"
#include "..\public\FruitCheckTrigger.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Fruit.h"
#include "PlatingPlate_Phase0.h"
#include "PlatingPlate_Phase1.h"
#include "PlatingPlate_Phase2.h"
#include "PPlate.h"
CFruitCheckTrigger::CFruitCheckTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CFruitCheckTrigger::CFruitCheckTrigger(const CFruitCheckTrigger & rhs)
	:CGameObject(rhs)
{
}

HRESULT CFruitCheckTrigger::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}


HRESULT CFruitCheckTrigger::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;


	_matrix WorldMatrix;

	if (nullptr != pArg)
		WorldMatrix = *(_matrix*)pArg;

	/* pArg = WorldMatrix */
	// Scale = WorldMatrix Scale;
	// ObjectPosition = WorldMatrix[3]
	// FruitType = WorldMatrix.r[3].w


	m_eLayerType = LayerType::LAYERTYPE_PLATINGPLATE;
	m_iCorrectCookingNumber = (_uint)XMVectorGetW(WorldMatrix.r[2]);
	WorldMatrix.r[2] = XMVectorSetW(WorldMatrix.r[2], 0);
	_float3 _vScale = GET_SCALEFROMMAT(WorldMatrix);
	//_float3 vColSize = { 1.f, 0.25f, 0.25f };
	_float3 vColPos = { 0.f,0.f,0.f };
	m_fRangeForFrustum = MaxScaleFromFloat3(_vScale) *2.f;
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = 2.f;
	_float3 vObjScale = { 1.f,1.f,1.f };//GET_SCALEFROMMAT(objinfo.WorldMatrix);

	_float4 vPos = STOREFLOAT4(WorldMatrix.r[3]);

	if (FAILED(SetUp_Component(_vScale, vColPos, vObjScale)))
		return E_FAIL;

	_matrix matWorld = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	_float4x4 fWorldMatrix = STOREMATRIX(matWorld);
//	XMStoreFloat4x4(&fWorldMatrix, matWorld);
	//XMLoadFloat4x4(&WorldMatrix)
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_eTriggerFruitType = (FruitType)((_uint)vPos.w);

#ifdef _DEBUG
	CCollider::COLLIDERDESC		ColliderDesc;

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vExtents = _float3(_vScale.x/2.f, _vScale.y / 2.f, _vScale.z / 2.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pRenderCollider, &ColliderDesc)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_int CFruitCheckTrigger::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (!m_isRender)
	{
		m_pColliderCom->Sleep();
		return 0;
	}

	if (m_pColliderCom)
	{
		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}
	return 0;
}

_int CFruitCheckTrigger::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;
	if (!m_isRender)
	{
		m_pColliderCom->Sleep();
		return 0;
	}


#ifdef _DEBUG
	m_pRenderCollider->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return 0;
}

HRESULT CFruitCheckTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	
	return S_OK;
}


void CFruitCheckTrigger::OnTriggerEnter(CCollisionObject & collision)
{
	if (m_isPlating)
		return;

	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_MONSTER_FRUIT)
	{
		CFruit* pFruit = (CFruit*)collision.Get_Object();
		if (pFruit->Get_FruitType() == m_eTriggerFruitType)
		{
			if (pFruit->Get_IsPicking())
				return;

			if (m_iCorrectCookingNumber == pFruit->Get_CookingNumber())
			{
				_float3 vPos = STOREFLOAT3(m_pTransformCom->Get_Position() - XMVectorSet(0.f,0.3f,0.f,0.f));
				pFruit->Set_PlatePosition(vPos);
				m_isPlating = true;
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_CookingUI_OnPlate"), m_iRenderPass)->Set_IsRender(true);
				RELEASE_INSTANCE(CGameInstance);
				if (Setting_PlatingCounts())
				{
					CGameObject* pPPlate = pGameInstance->Get_GameObject(SCENE::SCENE_STAGE03, L"Layer_PPlate", (_uint)CPPlate::BUTTON_TYPE_FRUIT);
					dynamic_cast<CPPlate*>(pPPlate)->Set_ButtonUpStateCheck(CPPlate::BUTTON_TYPE::BUTTON_TYPE_FRUIT, true);
				}
				OutputDebugStringW(TEXT("Check Correct Fruit - CorrectCooking\n"));
			}
			else
				OutputDebugStringW(TEXT("Check Correct Fruit - InCorrectCooking\n"));


		}
		else
			OutputDebugStringW(TEXT("Check InCorrect Fruit\n"));

	}
}

void CFruitCheckTrigger::OnTriggerExit(CCollisionObject & collision)
{
	if (m_isPlating)
		return;
}

void CFruitCheckTrigger::OnControllerCollisionHit(CCollisionObject & collision)
{
	
}



HRESULT CFruitCheckTrigger::SetUp_Component(_float3 vColSize, _float3 vColPos,_float3 vObjScale)
{
	m_vColPos = vColPos;
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

	_float vLength = GET_FLOAT3LENGTH(vColSize);
	if (vLength > 0.f)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
		tColliderDesc.isVisualization = true;
		tColliderDesc.isTrigger = true;
		tColliderDesc.pGameObject = this;

		tColliderDesc.isSceneQuery = true;


		CBoxCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = vColSize; //MULTI_FLOAT3(vColSize, 2.0);

		if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc)))
			return E_FAIL;

		_matrix smatPviot = XMMatrixTranslation(vColPos.x, vColPos.y, vColPos.z);
		m_pColliderCom->SetPivotMatrix(smatPviot);
		m_pColliderCom->Set_ShapeLayer(m_eLayerType);


		m_pColliderCom->Update_TransformPhysX();
		m_pColliderCom->Update_Transform();
	}

	return S_OK;
}

HRESULT CFruitCheckTrigger::SetUp_ConstantTable()
{

	return S_OK;
}

_bool CFruitCheckTrigger::Setting_PlatingCounts()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer =(CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	_bool isFinish = false;
	_uint PlateNumber = pPlayer->Get_Stage03PhaseNum();
	switch (PlateNumber)
	{
	case 0:
	{
		CPlatingPlate_Phase0* pPlate = (CPlatingPlate_Phase0*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), PlateNumber);
		pPlate->Add_PlatingCount();
		if (2 == pPlate->Get_PlatingCount())
			isFinish = true;
		break;
	}
	case 1:
	{
		CPlatingPlate_Phase1* pPlate = (CPlatingPlate_Phase1*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), PlateNumber);
		pPlate->Add_PlatingCount();
		if (3 == pPlate->Get_PlatingCount())
			isFinish = true;
		break;
	}
	case 2:
	{
		CPlatingPlate_Phase2* pPlate = (CPlatingPlate_Phase2*)pGameInstance->Get_GameObject(SCENE_STAGE03, TEXT("Layer_Plating_Plate"), PlateNumber);
		pPlate->Add_PlatingCount();
		if (4 == pPlate->Get_PlatingCount())
			isFinish = true;
		break;
	}
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);

	return isFinish;
}


CFruitCheckTrigger * CFruitCheckTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CFruitCheckTrigger*	pInstance = new CFruitCheckTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CFruitCheckTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFruitCheckTrigger::Clone(void * pArg, _uint iSceneIndex)
{
	CFruitCheckTrigger*	pInstance = new CFruitCheckTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CFruitCheckTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFruitCheckTrigger::Free()
{
	__super::Free();

	if(m_pColliderCom)
		m_pColliderCom->Remove_Actor();

#ifdef _DEBUG
	Safe_Release(m_pRenderCollider);
#endif
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
