#include "stdafx.h"
#include "..\public\BlenderPath.h"
#include "CollisionObject.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Fly.h"

CBlenderPath::CBlenderPath(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CBlenderPath::CBlenderPath(const CBlenderPath & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlenderPath::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CBlenderPath::NativeConstruct(void * pArg)
{
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_iNavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;
	m_StrLayerName = TEXT("Layer_BlenderPath");
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName;
	m_eLayerType = LayerType::LAYERTYPE_BLENDER;

	_float3 vColSize = objinfo.fCollider_Extent;
	_float3 vColPos = objinfo.fCollider_Center;
	m_fRangeForFrustum = MaxScaleFromFloat3(vColSize);
	if (m_fRangeForFrustum <= 0.f)
		m_fRangeForFrustum = objinfo.fFrustumRange;
	_float3 vObjScale = GET_SCALEFROMMAT(objinfo.WorldMatrix);

	if (FAILED(SetUp_Component(objinfo.fCollider_Extent, objinfo.fCollider_Center, vObjScale)))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	
	XMStoreFloat4(&m_vMoveRight,XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))));

	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	m_isRender = true;

	m_bIsRenderingShadow = true;
	
	for (_uint i = 0; i < 3; ++i)
		m_ArrKey[i] = 0.f;

	return S_OK;
}

_int CBlenderPath::Tick(_double TimeDelta)
{
	if (0 >(__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), m_fRangeForFrustum))
	{
		_int iPlayerCurlingIndex = 0;
		_int iDeleteCurlingIndex = 0;
		if (Setting_CurlingIndex(&iPlayerCurlingIndex, &iDeleteCurlingIndex))
		{
			if (m_iDeleteCurlingIndex == iDeleteCurlingIndex)
			{
				m_isDestroy = true;
				RELEASE_INSTANCE(CGameInstance);
				return 0;
			}
			Stage_RenderBranch(iPlayerCurlingIndex);
		}
	}
	else
	{
		if (m_isRender && m_pColliderCom[COLL_UP] && m_pColliderCom[COLL_RIGHT] && m_pColliderCom[COLL_DOWN] && m_pColliderCom[COLL_LEFT])
		{
			for (_uint i = 0; i < COLL_END; ++i)
				m_pColliderCom[i]->Remove_Actor();
		}
		m_isRender = false;
	}


	RELEASE_INSTANCE(CGameInstance);

	if (!m_isRender)
	{
		return 0;
	}
	
	MoveRoation(TimeDelta);

	if (true == m_bTriggerCheck) /* Player�� �ȿ� ���Դ�.*/
		PlayerMoveCheck(TimeDelta);

	if (m_pColliderCom[COLL_UP] && m_pColliderCom[COLL_RIGHT] && m_pColliderCom[COLL_DOWN] && m_pColliderCom[COLL_LEFT])
	{
		for (_uint i = 0; i < COLL_END; ++i)
			m_pColliderCom[i]->Update_TransformPhysX();
	}


	HITCheck(TimeDelta);
	

	return 0;
}

_int CBlenderPath::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	//if (m_pColliderCom[COLL_UP] && m_pColliderCom[COLL_RIGHT] && m_pColliderCom[COLL_DOWN] && m_pColliderCom[COLL_LEFT])
	//{
	//	for (_uint i = 0; i < COLL_END; ++i)
	//		//m_pColliderCom[i]->Update_Transform();
	//}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	return 0;
}

HRESULT CBlenderPath::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);
	return S_OK;
}

void CBlenderPath::Add_Force(_fvector vDir, PxForceMode::Enum eMode)
{
	for (_uint i = 0; i < COLL_END; ++i)
		m_pColliderCom[i]->Add_Force(vDir, eMode);
}

void CBlenderPath::Setting_Throwing(_fvector vDir)
{
	m_isColController = false;
	for (_uint i = 0; i < COLL_END; ++i)
		m_pColliderCom[i]->Add_Force(vDir*50.f, PxForceMode::eVELOCITY_CHANGE);
	m_isThrowing = true;
}

void CBlenderPath::OnCollisionEnter(CCollisionObject & collision)
{
	if (!m_isMove)
		return;
}

void CBlenderPath::OnCollisionStay(CCollisionObject & collision)
{
}

void CBlenderPath::OnCollisionExit(CCollisionObject & collision)
{
}

void CBlenderPath::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType())
	{
		CGameInstance* pGameInstnace = GET_INSTANCE(CGameInstance);

		if (false == m_HitCheck) /* �����̸� �ֱ� ���ؼ� */
		{
			m_HitCheck = true;
			static_cast<CPlayer*>(pGameInstnace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_HitCheck(true);
			/*OutputDebugString(TEXT("BlenderPath Ʈ���� �浹!!!!"));*/
		}
		RELEASE_INSTANCE(CGameInstance);
		/* Player�� �浹�� �Ǿ���.*/
	}
}

void CBlenderPath::SettingActorActive()
{
	if (!m_pColliderCom[COLL_UP] && !m_pColliderCom[COLL_RIGHT] && !m_pColliderCom[COLL_DOWN] && !m_pColliderCom[COLL_LEFT])
		return;

	for (_uint i = 0; i < COLL_END; ++i)
	{
		if (m_isActorActive)
		{
			m_pColliderCom[i]->WakeUp();
		}
		else
		{
			m_pColliderCom[i]->Sleep();
		}
	}
}

_bool CBlenderPath::Setting_CurlingIndex(_int * pCurlingIndex, _int * pDeleteIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* Player = (CPlayer*)pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0);
	if (Player)
	{
		*pCurlingIndex = Player->Get_CurlingIndex();
		*pDeleteIndex = Player->Get_Delete_CurlingIndex();
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	else
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}
}

void CBlenderPath::Stage_RenderBranch(_int iPlayerCurlingIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	switch (m_iCurrentScene)
	{
	case SCENE_STAGE03:
	{
		_int NextIndex = iPlayerCurlingIndex + 1;
		if (iPlayerCurlingIndex == m_iCurlingIndex || NextIndex == m_iCurlingIndex)
		{
			if (!m_isRender && m_pColliderCom[COLL_UP] && m_pColliderCom[COLL_RIGHT] && m_pColliderCom[COLL_DOWN] && m_pColliderCom[COLL_LEFT])
			{
				for (_uint i = 0; i < COLL_END; ++i)
					m_pColliderCom[i]->Add_Actor();
			}
			m_isRender = true;
		}
		else
		{
			if (m_isRender && m_pColliderCom[COLL_UP] && m_pColliderCom[COLL_RIGHT] && m_pColliderCom[COLL_DOWN] && m_pColliderCom[COLL_LEFT])
			{
				for (_uint i = 0; i < COLL_END; ++i)
					m_pColliderCom[i]->Remove_Actor();
			}
			m_isRender = false;
		}
		break;
	}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBlenderPath::MoveRoation(_double TimeDelta)
{
	if (true == m_RoationCheck)
	{
		m_AccTime += TimeDelta; /* ������ ��Ű�鼭 */
		if (10.f > m_AccTime)
			m_pTransformCom->RotationPerSec(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMConvertToRadians(m_fBlender_RotationMove[0]));
		else
		{
			m_AccTime = 0.0;
			m_RoationCheck = false; /* false �� �Ǹ鼭 �ݴ�� ���� �ɰ��Դϴ�. */
		}
	}
	else
	{
		m_AccTime += TimeDelta;
		if (10.f > m_AccTime)
			m_pTransformCom->RotationPerSec(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMConvertToRadians(m_fBlender_RotationMove[1]));
		else
		{
			m_AccTime = 0.0;
			m_RoationCheck = true; /* �ٽ� �ݴ�� ���Ŷ� ȣ�� */
		}
	}
}

void CBlenderPath::HITCheck(_double TimeDelta)
{
	if (true == m_HitCheck) /* �浹�� �Ǿ���. */
	{
		m_HitTimer += TimeDelta;
		if (1.0 < m_HitTimer)
		{
			m_HitCheck = false;
			m_HitTimer = 0.0;
		}
	}
}

void CBlenderPath::PlayerMoveCheck(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* PlayerTramsform = (CTransform*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Transform"));
	CCharacterController* PlayerController= (CCharacterController*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Controller"));
	CSphereCollider* PlayerCollider = (CSphereCollider*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Collider"));
	_vector vDir;

	vDir = XMVector3Normalize(XMLoadFloat4(&m_vMoveRight));

	if (m_RoationCheck)
		vDir *= -1;

	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
	_float3 vVelocity;
	_float4 vUpRayDirection;
	// �� ���� ��ǥ + ��ƽ �̵��� ��ŭ�� �� ����(��ƽ �տ��ִ� �����ǰ�)
	XMStoreFloat3(&tDescIn.vOrigin, PlayerTramsform->Get_Position() + XMVector3Normalize(vDir)* TimeDelta);
    // ���� Direction�� -Up
	XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(-PlayerTramsform->Get_State(CTransform::STATE_UP)));
	// raycast ����
	tDescIn.fDistance = 0.15f;
	// � ������Ʈ�� �ε��� �������� ���� ���� (����, �׸��� static�� �ε�����(���� �׺�޽��� static���� �Ǿ�����)
	tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
	// � ���̾������Ʈ���� �浹�� ������ ( �׺�޽��� layerType�� Plane���� �����ؼ� Plane�� üŷ����)
	// ����̾�� �����ϰ� ������ Set_ShapeLayer() �Լ� ���
	tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
	// �ε��� ������Ʈ�� �޾ƿ��� ���� ����
	CGameObject* pHitObject = nullptr;
	// Out�� �޾ƿ��� ������ �ּҰ� ������
	tDescOut.ppHitObject = &pHitObject;
	if (false == pGameInstance->Key_Down(VK_SPACE)) /* Player�� �������� �־ Raycast �ؾߵǱ� ������ */
	{
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (pHitObject) /* Raycast �� true -> ���뿡 �ִ� �׺� �Ž��� ��ó�� �ִٶ�°� �Դϴ�. */
			{
				vVelocity = STOREFLOAT3(XMLoadFloat3(&tDescOut.vHitPos) - PlayerTramsform->Get_Position());
				XMStoreFloat4(&vUpRayDirection, XMVector4Normalize(XMLoadFloat3(&tDescOut.vHitPos) - PlayerTramsform->Get_Position()));
				// ��ǥ ����
				PlayerTramsform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tDescOut.vHitPos));
				// ���� �־��� �̵����� �� 0���� �ʱ�ȭ
				PlayerTramsform->Set_Velocity(XMVectorZero());
				PlayerController->Move(TimeDelta * 3.0, vVelocity);
				PlayerCollider->Update_TransformPhysX();
				PlayerCollider->Update_Transform();
			}
		}
		else /* Raycast �� ���� �ʾƵ� �������� �������ߵȴ� �������� �ʾ��� ��� ȸ�� �Ǵ� ������ ���������ʴ� ���װ� �־��� */
		{
			_double dAdjustSpeed = 0.03;
			_float3 vVelocity = STOREFLOAT3(vDir * dAdjustSpeed);
			PlayerController->Move(TimeDelta, vVelocity);
			PlayerCollider->Update_TransformPhysX();
			PlayerCollider->Update_Transform();
		}
	}
	else /* �����̹ٸ� ���������� Player�� ���� ����Ȱ��̱� ������ �����߿����� ������� ���ֱ����ؼ��� ������� �ش� �������� �̵����� ��� ������ ���� �޴°� ���� ���ø� �ټ��ִ�. */
	{
		_double dAdjustSpeed = 0.03;
		_float3 vVelocity = STOREFLOAT3(vDir * dAdjustSpeed);
		PlayerController->Move(TimeDelta, vVelocity);
		PlayerCollider->Update_TransformPhysX();
		PlayerCollider->Update_Transform();
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBlenderPath::SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale)
{
	m_vColPos = vColPos;
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* ���� ���� */
	ColliderPos[COLL_UP]	= _float3(0.f, 4.4f, 0.f);
	ColliderPos[COLL_RIGHT] = _float3(4.4f, 0.f, 0.f);
	ColliderPos[COLL_DOWN]	= _float3(0.f, -4.4f, 0.f);
	ColliderPos[COLL_LEFT]	= _float3(-4.4f, 0.f, 0.f);

	for (_uint i = 0; i < COLL_END; ++i)
	{
		CPhysXCollider::DESC tColliderDesc;

		tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
		//tColliderDesc.isVisualization = true;
		tColliderDesc.isGravity = false;
		tColliderDesc.isTrigger = true;/* Ʈ���� �⵿*/
		tColliderDesc.isSceneQuery = false; /* �����浹�� ���ڴ�. */
		tColliderDesc.fRestitution = 0.f;
		
		tColliderDesc.pGameObject = this;
		CBoxCollider::DESC tBoxColliderDesc;
		tBoxColliderDesc.tColliderDesc = tColliderDesc;
		tBoxColliderDesc.vScale = MULTI_FLOAT3(_float3(1.f, 1.f, 6.8f), 1.99f);

		switch (i)
		{
		case 0:
			if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider0"), (CComponent**)&m_pColliderCom[i], &tBoxColliderDesc)))
				return E_FAIL;
			break;
		case 1:
			if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider1"), (CComponent**)&m_pColliderCom[i], &tBoxColliderDesc)))
				return E_FAIL;
			break;
		case 2:
			if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider2"), (CComponent**)&m_pColliderCom[i], &tBoxColliderDesc)))
				return E_FAIL;
			break;
		case 3:
			if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider3"), (CComponent**)&m_pColliderCom[i], &tBoxColliderDesc)))
				return E_FAIL;
			break;
		}
		_matrix smatPviot = XMMatrixTranslation(ColliderPos[i].x, ColliderPos[i].y, ColliderPos[i].z);
		m_pColliderCom[i]->SetPivotMatrix(smatPviot);
		m_pColliderCom[i]->Set_ShapeLayer(m_eLayerType);
		m_pColliderCom[i]->Update_TransformPhysX();
		m_pColliderCom[i]->Update_Transform();

		m_isActorActive = false;
		if (!m_pColliderCom[i])
			return E_FAIL;

		if (m_isActorActive)
		{
			m_pColliderCom[i]->WakeUp();
		}
		else
		{
			m_pColliderCom[i]->Sleep();
		}
		
	}
		return S_OK;
}

HRESULT CBlenderPath::SetUp_ConstantTable()
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
	_bool		bMotionBlur = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR);
	_bool		bShadow = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_SHADOW);
	_bool		bRimlight = m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_RIMLIGHT);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	_bool		bCheck = true;
	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.079f, -0.997f, 0.0f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;

	/*For. Shader On/Off*/

	//if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bCheck, sizeof(_bool))))
	//	return E_FAIL;
	if (false == m_bIsRenderingShadow)
	{
		bShadow = false;
	}
	if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
	if (false == m_bIsNormalMapping)
	{
		if (FAILED(m_pModelCom->Set_RawValue("g_bIsNormalMapping", &m_bIsNormalMapping, sizeof(_bool))))
			return E_FAIL;
	}
	/*For. ShadowMap*/
	_matrix PlayerWorld = XMMatrixIdentity();
	PlayerWorld.r[3] = dynamic_cast<CTransform*>(pGameInstance->Get_Components(SCENE_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

	_vector vPos = XMVector3TransformCoord(XMVectorSet(m_ArrKey[0], m_ArrKey[1], m_ArrKey[2], 1.f), PlayerWorld);
	_float3 vPosFloat;
	XMStoreFloat3(&vPosFloat, vPos);

	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	/*For. MotionBlur*/
	if (true == m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR))
	{
		m_pModelCom->Set_RawValue("g_OldWorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldWolrd)), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldView)), sizeof(_float4x4));

	}
	m_OldWolrd = m_pTransformCom->Get_WorldFloat4x4();
	XMStoreFloat4x4(&m_OldView, pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBlenderPath::OnControllerCollisionHit(CCollisionObject & collision)
{
	if (m_eLayerType == LayerType::LAYERTYPE_PICK)
	{
		m_isMove = true;
		m_isColController = true;
	}
}

CBlenderPath * CBlenderPath::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBlenderPath*	pInstance = new CBlenderPath(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBlenderPath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBlenderPath::Clone(void * pArg, _uint iSceneIndex)
{
	CBlenderPath*	pInstance = new CBlenderPath(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CBlenderPath");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlenderPath::Free()
{
	__super::Free();

	for (_uint i = 0; i < COLL_END; ++i)
	{
		if(m_pColliderCom[i])
			m_pColliderCom[i]->Remove_Actor();
	
		Safe_Release(m_pColliderCom[i]);
	}
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
