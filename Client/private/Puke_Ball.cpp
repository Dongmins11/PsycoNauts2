#include "stdafx.h"
#include "..\public\Puke_Ball.h"
#include "GameInstance.h"
#include "CollisionObject.h"

CPuke_Ball::CPuke_Ball(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CPuke_Ball::CPuke_Ball(const CPuke_Ball & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPuke_Ball::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPuke_Ball::NativeConstruct(void * _pArg, _uint iSceneIndex)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (nullptr != _pArg)
	{
		_uint iRandType = rand() % 10;
		_matrix RotationMatrix = XMMatrixIdentity();
		_float  fPower = GET_RANDOMFLOAT(3.f, 4.f);
		_float  fDegree = GET_RANDOMFLOAT(-15.f, 15.f);
		_float fSpeed = GET_RANDOMFLOAT(1.f, 3.5f);
		m_dSpeed = (_double)fSpeed;
		RotationMatrix = XMMatrixRotationY(XMConvertToRadians(fDegree));
		
		BallWorldDesc tWorldDesc = *(BallWorldDesc*)_pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tWorldDesc.vPos));
		_vector vDir = XMLoadFloat3(&tWorldDesc.vDir);
		vDir = XMVector3TransformNormal(vDir, RotationMatrix);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vDir);
		m_pTransformCom->Set_MyLook(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		ZeroMemory(&m_tBallDesc, sizeof(BALLDESC));
		m_tBallDesc.fStartMotionY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_tBallDesc.bMotion = true;
		XMStoreFloat3(&m_tBallDesc.vDir, vDir);
		m_tBallDesc.dMotionPower = (_double)tWorldDesc.fDistance / fPower;

		XMStoreFloat3(&m_vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_tBallDesc.vDir)*tWorldDesc.fDistance);

	}
	m_pTransformCom->Scaling(_float3(7.f, 7.f, 7.f));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_iLayerSize = pGameInstance->Get_LayerSize(SCENE_STAGE05, TEXT("Layer_Ball"));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CPuke_Ball::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_TimeAcc += TimeDelta * 0.5;

	CalculateDistance();

	ParabolicMotion(TimeDelta);

	m_dAccTime += TimeDelta;

	m_pSphereColliderCom->Update_TransformPhysX();

	if (10.0 < m_dAccTime)
	{
		m_isDestroy = true;
		Create_Effect();
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;                                                                                                                                  
	// raycasting 방향 ( 플레이어의 look방향이 기본(벽탈때 보통 플레이어 기준 앞쪽을 바라보기 때문)

	XMStoreFloat3(&tDescIn.vOrigin, m_pTransformCom->Get_Position());// ;// 레이 Direction을 -Up
	XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(-m_pTransformCom->Get_State(CTransform::STATE_UP)));
	// raycast 길이
	tDescIn.fDistance = 0.2f;
	// 어떤 오브젝트와 부딪힐 것인지에 대한 정보 (전부, 그리고 static만 부딪힌다(현재 네비메쉬만 static으로 되어있음)
	tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
	// 어떤 레이어오브젝트랑만 충돌할 것인지 ( 네비메쉬의 layerType을 Plane으로 설정해서 Plane만 체킹해줌)
	// 어떤레이어로 설정하고 싶으면 Set_ShapeLayer() 함수 사용
	tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
	// 부딪힌 오브젝트를 받아오기 위한 변수
	CGameObject* pHitObject = nullptr;
	// Out에 받아오기 때문에 주소값 던져줌
	tDescOut.ppHitObject = &pHitObject;
	if (pGameInstance->Raycast(tDescIn, tDescOut))
	{
		if (pHitObject)
		{
			m_isDestroy = true;
			Create_Effect();
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

_int CPuke_Ball::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);

	return 0;
}

HRESULT CPuke_Ball::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_BRAINSTORM);
	return S_OK;
}

void CPuke_Ball::OnTriggerEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLAYER_TRIGGER)
	{
		m_isDestroy = true;
		//여기 터지는 이펙트
		Create_Effect(); //플레이어가 맞는거라서 다른이펙트로 ㄱ
	}

}

void CPuke_Ball::OnCollisionEnter(CCollisionObject & collision)
{
	if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_PLANE)
	{
		m_isDestroy = true;
		//여기 터지는 이펙트
		Create_Effect();
	}
}

void CPuke_Ball::OnControllerCollisionHit(CCollisionObject & collision)
{
}

void CPuke_Ball::OnTriggerExit(CCollisionObject & collision)
{
}

void CPuke_Ball::ParabolicMotion(_double TimeDelta)
{
	if (!m_tBallDesc.bMotion)
		return;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_tBallDesc.dMotionTime += TimeDelta;
	m_tBallDesc.fBallPosY = m_tBallDesc.fStartMotionY + (_float)((((m_tBallDesc.dMotionTime * m_tBallDesc.dMotionTime) * (-18.8f * 0.5f)) + m_tBallDesc.dMotionTime * (m_tBallDesc.dMotionPower))*0.3);

	vPos = XMVectorSet(XMVectorGetX(vPos), m_tBallDesc.fBallPosY, XMVectorGetZ(vPos), 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Go_ByAxis(XMLoadFloat3(&m_tBallDesc.vDir), TimeDelta*m_dSpeed, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

}

void CPuke_Ball::CalculateDistance()
{
	m_fDistance = XMVectorGetX(XMVector3Length(XMVectorSetY(XMLoadFloat3(&m_vTargetPos), 0.f) - XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f)));
}

void CPuke_Ball::Create_Effect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/*const _tchar* pEffectTag = TEXT("Effect_Boss_BallDieParticle%d");

	_tchar		szSize[MAX_PATH] = TEXT("");
	wsprintf(szSize, pEffectTag, m_iLayerSize);

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_FoodDieStar"),
		SCENE_STATIC, Effect_Boss_FoodDieStar)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_Boss_FoodDieStar"), m_pTransformCom->Get_Position(), true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}*/

	if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
		pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BallDieRect"),
		SCENE_STATIC, TEXT("Effect_BallDieRect"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (FAILED(pGameInstance->Effect_Call_WorldPos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_BallDieRect"),
		m_pTransformCom->Get_Position(), true)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPuke_Ball::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_BrainStorm"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_AttCollider"), (CComponent**)&m_pSphereColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	m_pSphereColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_MONSTER_ATTACK);

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPuke_Ball::SetUp_ConstantTable()
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

	_float fTimeAcc = (_float)m_TimeAcc;
	m_pModelCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(1.f, 0.f, 1.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(0.25f, 0.f, 1.f);
	m_pModelCom->Set_RawValue("g_vEndColor", &vEndColor, sizeof(_float3));

	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pTextureCom->Get_SRV(6));

	_float fDissolveTime = (_float)m_lfDeadTime * 0.5f;

	if (FAILED(m_pModelCom->Set_RawValue("g_fDissolveTime", &fDissolveTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bDissolve", &m_bDeadCheck, sizeof(_bool))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CPuke_Ball * CPuke_Ball::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPuke_Ball*	pInstance = new CPuke_Ball(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPuke_Ball");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPuke_Ball::Clone(void * pArg, _uint iSceneIndex)
{
	CPuke_Ball*	pInstance = new CPuke_Ball(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iSceneIndex)))
	{
		MSGBOX("Failed to Created : CPuke_Ball");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPuke_Ball::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);

	if (nullptr != m_pSphereColliderCom)
	{
		m_pSphereColliderCom->Remove_Actor();
		Safe_Release(m_pSphereColliderCom);
	}
}
