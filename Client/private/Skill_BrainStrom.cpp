#include "stdafx.h"
#include "..\public\Skill_BrainStrom.h"
#include "GameInstance.h"
#include "Camera_Fly.h"
#include "CollisionObject.h"
#include "Player.h"


CSkill_BrainStrom::CSkill_BrainStrom(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSkill_BrainStrom::CSkill_BrainStrom(const CSkill_BrainStrom & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkill_BrainStrom::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_BrainStrom::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_BoneTargetMatrix("goggles1")))
		return E_FAIL;

	if (FAILED(Frist_SetPosition()))
		return E_FAIL;

	m_fScale = 20.f;

	m_pTransformCom->Scaling(_float3(m_fScale, m_fScale, m_fScale));

	//m_pColliderCom->Set_ShapeLayer(m_eLayerType = LayerType::LAYERTYPE_PLAYER_SKILL);

	return S_OK;
}

_int CSkill_BrainStrom::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	m_TimeAcc += TimeDelta * 0.5;

	Scale_Up(TimeDelta);

	if (false == m_bDeadCheck)
		Bone_MatrixCalculation(TimeDelta);

	BrainStrom_Collide();


	if (nullptr != m_pColliderCom && true == m_bDeadCheck)
	{
		m_lfDeadTime += TimeDelta;

		if (2.f <= m_lfDeadTime)
		{
			m_isDestroy = true;
			m_pColliderCom->Remove_Actor();
		}
		else
		{
			m_pColliderCom->Add_Actor();
			m_pColliderCom->Update_TransformPhysX();
			m_pColliderCom->Update_Transform();
		}
	}

	return _int();
}

_int CSkill_BrainStrom::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);
	
	return _int();
}

HRESULT CSkill_BrainStrom::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(CEffect_Manager::PASS_BRAINSTORM);

#ifdef _DEBUG
	m_pOBBCom->Render();
#endif // _DEBUG

	return S_OK;
}


HRESULT CSkill_BrainStrom::Frist_SetPosition()
{
	if (nullptr != m_pCombinedMatrix || nullptr != m_pTargetWorldMatrix)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iCurrentScene, L"Layer_Camera", 0);
		if (nullptr == pCameraFly)
		{
			MSGBOX("Failed to Finder CamFly : Function CSkill_BrainStrom : Bone_MatrixCalculation ");
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		_matrix			CamWorldMat;
		_float4			CalculationPos;
		_vector			PlayerPosition;
		_vector			PlayerToCamDirection;
		_vector			PlayerToCamPositon;

		CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();

		PlayerPosition = XMLoadFloat4(&*(_float4*)&m_pTargetWorldMatrix->m[3]);

		XMStoreFloat4(&CalculationPos, PlayerPosition);

		CalculationPos.y += 1.5f;

		PlayerToCamDirection = XMVector3Normalize(XMLoadFloat4(&CalculationPos) - CamWorldMat.r[3]);

		PlayerToCamPositon = XMLoadFloat4(&CalculationPos) + (PlayerToCamDirection * 5.f);

		_float4 LerpPos;
		_float4 PlayerPos;

		XMStoreFloat4(&PlayerPos, PlayerPosition);
		XMStoreFloat4(&LerpPos, PlayerToCamPositon);

		if (PlayerPos.y >= LerpPos.y)
			LerpPos.y = PlayerPos.y;

		if (false == m_bMoveCheck)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, PlayerToCamPositon);

		Safe_Release(pGameInstance);
	}


	return S_OK;
}

void CSkill_BrainStrom::Scale_Up(_double TimeDelta)
{
	if (40.f >= m_fScale)
	{
		m_fScale += (_float)TimeDelta * 10.f;
		m_pTransformCom->Scaling(_float3(m_fScale, m_fScale, m_fScale));
	}
}

void CSkill_BrainStrom::Bone_MatrixCalculation(_double TimeDelta)
{
	if (nullptr != m_pCombinedMatrix || nullptr != m_pTargetWorldMatrix)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CGameObject* pCameraFly = pGameInstance->Get_GameObject(m_iCurrentScene, L"Layer_Camera", 0);
		if (nullptr == pCameraFly)
		{
			MSGBOX("Failed to Finder CamFly : Function CSkill_BrainStrom : Bone_MatrixCalculation ");
			Safe_Release(pGameInstance);
			return;
		}

		_matrix			CamWorldMat;
		_float4			CalculationPos;
		_vector			PlayerPosition;
		_vector			PlayerToCamDirection;
		_vector			LerpPosition;


		CamWorldMat = dynamic_cast<CCamera_Fly*>(pCameraFly)->Get_WorldMatrixCam();

		PlayerPosition = XMLoadFloat4(&*(_float4*)&m_pTargetWorldMatrix->m[3]);

		XMStoreFloat4(&CalculationPos, PlayerPosition);

		CalculationPos.y += 1.5f;

		PlayerToCamDirection = XMVector3Normalize(XMLoadFloat4(&CalculationPos) - CamWorldMat.r[3]);

		PlayerToCamDirection = XMLoadFloat4(&CalculationPos) + (PlayerToCamDirection * 5.f);

		LerpPosition = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), PlayerToCamDirection, 0.2f);

		_float4 LerpPos;
		_float4 PlayerPos;

		XMStoreFloat4(&PlayerPos, PlayerPosition);
		XMStoreFloat4(&LerpPos, LerpPosition);

		if (PlayerPos.y >= LerpPos.y)
			LerpPos.y = PlayerPos.y;

		if (false == m_bMoveCheck)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&LerpPos));

		Safe_Release(pGameInstance);
	}

}

void CSkill_BrainStrom::BrainStrom_Collide()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
	// raycasting 의 시작지점 ( 지금 player의 발쪽에 아마 되어있을 가능성이 높음 아마 몸통부분으로 수정해줘야할듯 )
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pPlayerTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	_float3 vPlayerPos = pPlayerTransform->Get_State_Float3(CTransform::STATE_POSITION);
	vPlayerPos.y += 0.5f;


	tDescIn.vOrigin = vPlayerPos;// +XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))*0.35f);// ;
								 // raycasting 방향 ( 플레이어의 look방향이 기본(벽탈때 보통 플레이어 기준 앞쪽을 바라보기 때문)
	XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(m_pTransformCom->Get_Position() - XMLoadFloat3(&vPlayerPos)));
	// raycast 길이
	tDescIn.fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - XMLoadFloat3(&vPlayerPos)));
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
		m_pTransformCom->Set_State_Float3(CTransform::STATE_POSITION, tDescOut.vHitPos);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CSkill_BrainStrom::SetUp_Component()
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

	CCollider::COLLIDERDESC ColliderDesc;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.0f, 0.04f, 0.0f);
	ColliderDesc.vExtents = _float3(0.03f, 0.03f, 0.03f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;


	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;

	tColliderDesc.pGameObject = this;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = m_pTransformCom->Get_ScaleFloat3();

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tSphereColliderDesc)))
		return E_FAIL;

	m_eLayerType = LayerType::LAYERTYPE_BRAINSTROM;

	m_pColliderCom->Set_ShapeLayer(m_eLayerType);

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_BrainStrom::SetUp_ConstantTable()
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

	_float fTimeAcc = (_float)m_TimeAcc;
	m_pModelCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));

	_float3 vColor = _float3(1.f, 1.f, 0.f);
	m_pModelCom->Set_RawValue("g_vColor", &vColor, sizeof(_float3));
	_float3	vEndColor = _float3(1.f, 0.25f, 0.f);
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

HRESULT CSkill_BrainStrom::SetUp_BoneTargetMatrix(const char * pBoneName)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CTransform*	pTargetTransform = (CTransform*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTargetTransform)
	{
		MSGBOX("Failed to SetUp_BoneTargetMatrix : Funtion CSkill_BrainStrom");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	CModel*		pModel = (CModel*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Model"), 0);
	if (nullptr == pModel)
	{
		MSGBOX("Failed to SetUp_BoneTargetMatrix : Funtion CSkill_BrainStrom");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_pMyWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	m_OffsetMatrix = pModel->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = pModel->Get_PivotMatrix();
	m_pCombinedMatrix = pModel->Get_CombinedMatrixPtr(pBoneName);
	m_pTargetWorldMatrix = pTargetTransform->Get_WorldMatrixPtr();

	Safe_Release(pGameInstance);

	return S_OK;
}

CSkill_BrainStrom * CSkill_BrainStrom::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSkill_BrainStrom*	pInstance = new CSkill_BrainStrom(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CSkill_BrainStrom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkill_BrainStrom::Clone(void * pArg, _uint iSceneIndex)
{
	CSkill_BrainStrom*	pInstance = new CSkill_BrainStrom(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CSkill_BrainStrom");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_BrainStrom::Free()
{
	__super::Free();


	if (nullptr != m_pColliderCom)
		m_pColliderCom->Remove_Actor();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pTextureCom);
}
