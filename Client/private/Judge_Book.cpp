#include "stdafx.h"
#include "..\public\Judge_Book.h"
#include "GameInstance.h"

CJudge_Book::CJudge_Book(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CJudge_Book::CJudge_Book(const CJudge_Book & rhs)
	: CGameObject(rhs)
{
}

HRESULT CJudge_Book::NativeConstruct(void* _pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(_pArg, iCurrentScene)))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		ZeroMemory(&m_tWeaponInfo, sizeof(WEAPONINFO));
		m_tWeaponInfo = *(WEAPONINFO*)_pArg;
	}

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	XMStoreFloat4x4(&m_NewPivotMatrix, XMMatrixIdentity()*XMMatrixScaling(2.f, 2.f, 2.f)*XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))*XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f))*XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(330.f)));

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_pBoxCollider->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PICK);
	return S_OK;
}

HRESULT CJudge_Book::NativeConstruct_Prototype()
{
	return S_OK;
}

_int CJudge_Book::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_IsDrop)
	{
		_float4x4 CalculateMatrix;
		XMStoreFloat4x4(&CalculateMatrix, XMLoadFloat4x4(&m_NewPivotMatrix)*XMLoadFloat4x4(m_tWeaponInfo.pParentMatrix));
		m_pTransformCom->Set_WorldMatrix(CalculateMatrix);
		m_pBoxCollider->SetGravity(false);
		m_pBoxCollider->Sleep();
	}
	else
	{
		if (!m_IsThrow)
		{
			CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
			_vector vPlayerPos = pPlayerTrans->Get_Position() + XMVectorSet(0.f, 1.7f, 0.f, 0.f);
			_vector vBookPos = m_pTransformCom->Get_Position();
			_vector vDir = XMVector3Normalize(vPlayerPos - vBookPos);
			m_pBoxCollider->Add_Force(vDir*(_float)m_pTransformCom->Get_TransFormDescSpeed()*3.f);
			m_IsThrow = true;
			XMStoreFloat3(&m_vPrePos, vBookPos);

			/* 이펙트 레이어 생성 -> 이펙트 테그를 맞춰줘야함 */
			if (FAILED(pGameInstance->Add_ObjectToParticle_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Repeat_PaperRect"),
				SCENE_STATIC, TEXT("Effect_Repeat_PaperRect"), &m_pEffect)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			Safe_AddRef(m_pEffect);

			/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
			if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), false, false, m_pTransformCom, TEXT("Layer_Repeat_PaperRect"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
		}
		else
		{
			_vector vCurPos = m_pTransformCom->Get_Position();
			_vector vCalculateDir = vCurPos - XMLoadFloat3(&m_vPrePos);
			XMStoreFloat3(&m_vPrePos, vCurPos);
			vCalculateDir = XMVector3Normalize(vCalculateDir);


			XMStoreFloat4(&m_vColliderDir, XMVectorSet(XMVectorGetX(vCalculateDir), XMVectorGetY(vCalculateDir), XMVectorGetZ(vCalculateDir), 1.f));
		}

		if (m_isPicking)
		{
			CTransform* pPlayerTrans = (CTransform*)pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_RightHand"), TEXT("Com_Transform"), 0);
			_vector vPos = pPlayerTrans->Get_Position() - pPlayerTrans->Get_State(CTransform::STATE_UP) * 0.7f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			m_pBoxCollider->SetGravity(false);
		}
		else
		{
			m_pBoxCollider->SetGravity(true);
		}
		m_pBoxCollider->Update_TransformPhysX();
	}



	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

_int CJudge_Book::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	}

	if (true == m_isInteraction)
	{
		m_isInteraction = false;
		m_isDestroy = true;

		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			Safe_Release(m_pEffect);
			m_pEffect = nullptr;
		}
	}

	if (m_IsDrop&&m_IsThrow)
		m_pBoxCollider->Update_Transform();

	return 0;
}

HRESULT CJudge_Book::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CJudge_Book::Setting_Throwing(_fvector vDir)
{
	m_pBoxCollider->Reset_Power();
	m_isThrowing = true;
	m_pBoxCollider->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
}

void CJudge_Book::OnTriggerEnter(CCollisionObject & collision)
{
	if (m_isThrowing || m_IsThrow)
	{
		m_isInteraction = true;
		//해당위치에 종이 퍼뜨려지는 이펙트
	}
}

void CJudge_Book::OnCollisionEnter(CCollisionObject & collision)
{
	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}

void CJudge_Book::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CJudge_Book::OnTriggerExit(CCollisionObject & collision)
{
}

void CJudge_Book::Set_Drop(_bool _bDrop)
{
	m_IsDrop = _bDrop;
	m_pBoxCollider->SetTrigger(!_bDrop);
}

void CJudge_Book::SetOn_Rotate(_bool _bRotate)
{
	m_IsRotation = _bRotate;
}

HRESULT CJudge_Book::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	_tchar szModelPrototypeTag[MAX_PATH] = TEXT("");
	swprintf_s(szModelPrototypeTag, MAX_PATH, TEXT("Prototype_Component_Model_JudgeBook%d"), m_tWeaponInfo.iWeaponType);

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, szModelPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;
	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isGravity = true;

	tColliderDesc.isSceneQuery = true;
	tColliderDesc.isTrigger = false;

	tColliderDesc.pGameObject = this;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pBoxCollider, &tBoxColliderDesc)))
		return E_FAIL;

	m_pBoxCollider->Update_TransformPhysX();

	return S_OK;
}

HRESULT CJudge_Book::SetUp_ConstantTable()
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


CJudge_Book * CJudge_Book::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CJudge_Book*	pInstance = new CJudge_Book(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CJudge_Book");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CJudge_Book::Clone(void * pArg, _uint iSceneIndex)
{
	CJudge_Book*	pInstance = new CJudge_Book(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CJudge_Book");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJudge_Book::Free()
{
	__super::Free();
	if (nullptr != m_pBoxCollider)
	{
		m_pBoxCollider->Remove_Actor();
		Safe_Release(m_pBoxCollider);
	}
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if (nullptr != m_pEffect)
	{
		m_pEffect->Set_IsDestroy(true);
		Safe_Release(m_pEffect);
		m_pEffect = nullptr;
	}
}
