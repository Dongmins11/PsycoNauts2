#include "stdafx.h"
#include "..\public\Spatula.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Sequence.h"
#include "Idle_Spatula.h"
#include "Attack_Spatula.h"
#include "Taunt_Spatula.h"
#include "Intro_Spatula.h"
#include "Boss_Goat.h"

CSpatula::CSpatula(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CEnemy(pDevice, pDeviceContext)
{
}

CSpatula::CSpatula(const CSpatula & rhs)
	: CEnemy(rhs)
{
}

HRESULT CSpatula::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CSpatula::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CBoss_Goat::BOSSGOATINFO tBossGoatInfo;
		ZeroMemory(&tBossGoatInfo, sizeof(CBoss_Goat::BOSSGOATINFO));
		tBossGoatInfo = *(CBoss_Goat::BOSSGOATINFO*)pArg;
		m_pParent = tBossGoatInfo.pParent;
		m_pTransformCom->Set_WorldMatrix(tBossGoatInfo.WorldMatrix);
	}

	m_pModelCom->Set_Animation(3);

	m_pRoot = CSequence::Create();
	m_pIdleSpatula = CIdle_Spatula::Create(this);
	m_pAttackSpatula = CAttack_Spatula::Create(this);
	m_pTauntSpatula = CTaunt_Spatula::Create(this);
	m_pIntroSpatula = CIntro_Spatula::Create(this);

	m_pRoot->AddChild(m_pIntroSpatula);
	m_pRoot->AddChild(m_pTauntSpatula);
	m_pRoot->AddChild(m_pAttackSpatula);
	m_pRoot->AddChild(m_pIdleSpatula);

	m_pCombinedMatrixPtr = m_pModelCom->Get_CombinedMatrixPtr("AttackPoint");
	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_OffsetMatrix = m_pModelCom->Get_OffsetMatrix("AttackPoint");

	return S_OK;
}

_int CSpatula::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_pModelCom->Lerp_Update(TimeDelta);

	if (true == m_pRoot->Invoke(TimeDelta, m_pModelCom, m_pTransformCom, nullptr))
		m_isDestroy = true;

	XMStoreFloat4x4(&m_BoneMatrix, XMLoadFloat4x4(m_pCombinedMatrixPtr)*XMLoadFloat4x4(&m_PivotMatrix)*m_pTransformCom->Get_WorldMatrix());

	m_pAttColliderCom->Update_TransformPhysX(XMLoadFloat4x4(&m_BoneMatrix));

	return _int();
}

_int CSpatula::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (0 > m_iHp)
		m_isDestroy = true;

	return 0;
}

HRESULT CSpatula::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CSpatula::Reset()
{
	m_pAttackSpatula->Reset_AttackState();
}

void CSpatula::OnTriggerEnter(CCollisionObject & collision)
{
}

void CSpatula::OnCollisionEnter(CCollisionObject & collision)
{
}

void CSpatula::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CSpatula::OnTriggerExit(CCollisionObject & collision)
{
}

void CSpatula::Update_LookVector(_double TimeDelta)
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

void CSpatula::LockBosses(_bool * _bArr)
{
	static_cast<CBoss_Goat*>(m_pParent)->Lock_Boss(_bArr);
}


HRESULT CSpatula::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Spatula"), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
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
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = false;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	tBoxColliderDesc.vScale = { 5.f,5.f,5.f }; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_HitCollider"), (CComponent**)&m_pAttColliderCom, &tBoxColliderDesc)))
		return E_FAIL;

	m_pAttColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER_ATTACK);

	return S_OK;
}

HRESULT CSpatula::SetUp_ConstantTable()
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

CSpatula * CSpatula::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CSpatula*	pInstance = new CSpatula(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg, SCENE_STATIC)))
	{
		MSGBOX("Failed to Created : CBog");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSpatula::Clone(void * pArg, _uint iCurrentScene)
{
	return nullptr;
}

void CSpatula::Free()
{
	__super::Free();
	Safe_Release(m_pRoot);
	Safe_Release(m_pAttackSpatula);
	Safe_Release(m_pTauntSpatula);
	Safe_Release(m_pIdleSpatula);
	Safe_Release(m_pIntroSpatula);
	if (m_pAttColliderCom)
	{
		m_pAttColliderCom->Remove_Actor();
		Safe_Release(m_pAttColliderCom);
	}
}
