#include "stdafx.h"
#include "..\public\Bulb.h"
#include "GameInstance.h"
#include "CollisionObject.h"

CBulb::CBulb(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CBulb::CBulb(const CBulb & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBulb::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBulb::NativeConstruct(void * _pArg, _uint iSceneIndex)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		BulbWorldDesc tWorldDesc = *(BulbWorldDesc*)_pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tWorldDesc.vPos));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&tWorldDesc.vDir));
		m_pTransformCom->Set_MyLook(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		ZeroMemory(&m_tBulbDesc, sizeof(BULBDESC));
		m_tBulbDesc.fStartMotionY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_tBulbDesc.bMotion = true;
		m_tBulbDesc.vDir = tWorldDesc.vDir;
		m_tBulbDesc.dMotionPower = (_double)tWorldDesc.fDistance;

		XMStoreFloat3(&m_vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_tBulbDesc.vDir)*tWorldDesc.fDistance);
	}

	if (!m_bTrailCreate)
	{
		m_bTrailCreate = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_Bulb_Trail"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Bulb_Trail"), &m_pEffectTrail, nullptr)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}
		Safe_AddRef(m_pEffectTrail);

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, true, m_pTransformCom, TEXT("Layer_Bulb_Trail"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}

		pGameInstance->PlaySoundOnce(L"BadIdea_Thorw.ogg",CHANNELID::MONSTERATT);
		RELEASE_INSTANCE(CGameInstance);

	}

	return S_OK;
}

_int CBulb::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	CalculateDistance();

	ParabolicMotion(TimeDelta);
	
	if (!m_tBulbDesc.bMotion)
	{

		//장판이펙트 이시점에 최초 1회 생성하면 되고 트리거 발동시 소멸되게 만들 것. << 10초 이상 지속시 터지는 방식 이전까지는 트리거 작동으로 소멸되게 처리할 것 
		//제식이형님은 생성만해주시고 트랜스폼에 맞춰서 << 소멸타이밍은 제가 잡을 수 있게 
		if (!m_isInteraction)
		{
			m_isInteraction = true;
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->StopSound( CHANNELID::MONSTERATT);
			if (FAILED(pGameInstance->Add_ObjectToLayer_GetImmediately(SCENE_STATIC, TEXT("Prototype_GameObject_PurseRing"), pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_PurseRing"), &m_pEffect, &m_pTransformCom->Get_Position())))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			Safe_AddRef(m_pEffect);

			if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
				pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bulb"),
				SCENE_STATIC, TEXT("Effect_Smoke_Bulb"))))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}

			/* 이펙트 상속 -> 생성 직후 바로 해줘야함 세트메뉴같은거임 */
			if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Smoke_Bulb"), m_pTransformCom->Get_Position() + m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.05f, true)))
			{
				RELEASE_INSTANCE(CGameInstance);
				return -1;
			}
			RELEASE_INSTANCE(CGameInstance);
		}
		//이펙트 객체 생성해서 지우는건 조건을 걸어줘서 따로 설정해서 제거해주면 됨 
		//객체별 이펙트 인덱스 값들 설정하고 싶으면 생성과 동시에 게임인스턴스에서 LayerSize받아온걸로 처리하면 됨

		m_dAccTime += TimeDelta;

		if (10.0 < m_dAccTime)
		{
			if (nullptr != m_pEffectTrail)
			{
				m_pEffectTrail->Set_IsDestroy(true);
				Safe_Release(m_pEffectTrail);
				m_pEffectTrail = nullptr;
			}

			if (nullptr != m_pEffect)
			{
				m_pEffect->Set_IsDestroy(true);
				Safe_Release(m_pEffect);
				m_pEffect = nullptr;
			}
			m_isDestroy = true;			
		}
	}
	if(!m_isDestroy)
		m_pSphereColliderCom->Update_TransformPhysX();

	return 0;
}

_int CBulb::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CBulb::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CBulb::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		m_isDestroy = true;
		m_isPulling = true;

		if (nullptr != m_pEffectTrail)
		{
			m_pEffectTrail->Set_IsDestroy(true);
			m_pEffectTrail = nullptr;
		}

		if (nullptr != m_pEffect)
		{
			m_pEffect->Set_IsDestroy(true);
			m_pEffect = nullptr;
		}
		//여기 터지는 이펙트
	}

}

void CBulb::OnCollisionEnter(CCollisionObject & collision)
{
}

void CBulb::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CBulb::OnTriggerExit(CCollisionObject & collision)
{
}

void CBulb::ParabolicMotion(_double TimeDelta)
{
	if (!m_tBulbDesc.bMotion)
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_tBulbDesc.dMotionTime += TimeDelta;
	m_tBulbDesc.fBulbPosY = m_tBulbDesc.fStartMotionY + (((m_tBulbDesc.dMotionTime * m_tBulbDesc.dMotionTime) * (-9.8f * 0.5f)) + m_tBulbDesc.dMotionTime * (m_tBulbDesc.dMotionPower))*0.3;

	if ((m_tBulbDesc.fStartMotionY >= m_tBulbDesc.fBulbPosY))
	{
		m_tBulbDesc.bMotion = false;
		return;
	}

	if (1.0f < m_fDistance && !m_bStopRotate)
	{
		m_pTransformCom->RotationPerSec(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta*10.0);
	}
	else
	{
		m_bStopRotate = true;
		m_dLerpTime += TimeDelta * 1000;
		if (1.0 <= m_dLerpTime)
			m_dLerpTime = 1.0;
		_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		_vector vAimUp = XMVector4Normalize(XMVectorLerp(vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f), (_float)m_dLerpTime));
		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vAimUp);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(XMVector3Cross(vRight, vAimUp)));
	}

	vPos = XMVectorSet(XMVectorGetX(vPos), m_tBulbDesc.fBulbPosY, XMVectorGetZ(vPos), 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Go_ByAxis(XMLoadFloat3(&m_tBulbDesc.vDir), TimeDelta*0.8, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

}

void CBulb::CalculateDistance()
{
	m_fDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_vTargetPos), 0.f) - XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f)));
}

HRESULT CBulb::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Bulb0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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

	tColliderDesc.isSceneQuery = false;
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = _float3(1.3f, 1.3f, 1.3f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_SphereCollider"), (CComponent**)&m_pSphereColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	m_pSphereColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_MONSTER_ATTACK);

	return S_OK;
}

HRESULT CBulb::SetUp_ConstantTable()
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


CBulb * CBulb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBulb*	pInstance = new CBulb(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBulb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBulb::Clone(void * pArg, _uint iSceneIndex)
{
	CBulb*	pInstance = new CBulb(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CBulb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBulb::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if(nullptr != m_pEffect)
		Safe_Release(m_pEffect);

	if(nullptr != m_pEffectTrail)
		Safe_Release(m_pEffectTrail);


	if (nullptr != m_pSphereColliderCom)
	{
		m_pSphereColliderCom->Remove_Actor();
		Safe_Release(m_pSphereColliderCom);
	}
}
