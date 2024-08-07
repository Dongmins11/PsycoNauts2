#include "stdafx.h"
#include "..\public\Trampoline.h"
#include "GameInstance.h"
#include "CollisionObject.h"
#include "Player.h"
#include "Camera_Fly.h"
CTrampoline::CTrampoline(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTrampoline::CTrampoline(const CTrampoline & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTrampoline::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTrampoline::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_bisCreated = true;
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;//objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;//objinfo.StrModelName;
	m_StrObjectFullName = objinfo.StrObjectFullName; //objinfo.StrObjectFullName;
	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_TRAMPOLINE;//objinfo.iPX_Index;
	m_eLayerType = (LayerType)m_iPX_TYPE;

	Loading_PlateNaviData(TEXT("../../Data/NaviTrampoline.dat"));

	if (FAILED(SetUp_Component()))
		return E_FAIL;


	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);// objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);

	m_pModelCom->Set_Animation(0);
	//_float fRandom = rand() % 5;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f + fRandom, 2.f, 5.f, 0.f));
	//m_pTransformCom->Set_MyLook(XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;
	//m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_STATIC);
	//m_pTriggerCom->Set_ShapeLayer(m_eLayerType);
	m_pNaviCollider->Set_ShapeLayer(LayerType::LAYERTYPE_PLANE);
	_matrix smatPviot = XMMatrixIdentity();//m_pTransformCom->Get_WorldMatrix();
	m_pNaviCollider->SetPivotMatrix(smatPviot);
	m_pNaviCollider->Update_TransformPhysX();
	m_pNaviCollider->Update_Transform();

	m_eCurAnimstate = ANIM_DOWN;

	return S_OK;
}

_int CTrampoline::Tick(_double TimeDelta)
{
	UpdateAnimState(TimeDelta);
	if (true == m_bIsOneFrame)
	{
		if (1 == m_iOneFrame)
		{
			m_bIsUpdate = false;
			m_bIsOneFrame = false;
			m_iOneFrame = 0;
		}
		else
		{
			++m_iOneFrame;
		}
	}
	if (true == m_bIsUpdate)
	{
		m_pModelCom->Set_Animation(m_iCurAnimation);
		m_pModelCom->Lerp_Update(TimeDelta*m_dAnimSpeed);
	}
	else
	{
		m_pModelCom->Update(0.f);

	}

	m_pTriggerCom->Update_TransformPhysX();
	//m_pNaviCollider->Update_TransformPhysX();


	return _int();
}

_int CTrampoline::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;


	m_pTriggerCom->Update_Transform();
	//m_pNaviCollider->Update_Transform(&m_pTransformCom->Get_WorldFloat4x4());


	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);


	return 0;
}

HRESULT CTrampoline::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);



	return S_OK;
}

HRESULT CTrampoline::Loading_PlateNaviData(const _tchar * pNaviDataFilePath)
{
	HANDLE			hFile = 0;
	_uint			iCntPoint = 3;
	_ulong	dwByte = 0;
	hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	NAVIMESHDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

	_float3*	vPoints = nullptr;
	_uint			iOption = 0;

	vector<_float3*> vVec;
	while (true)
	{
		vPoints = new _float3[3];
		ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete_Array(vPoints);
			break;
		}

		//	memcpy(NaviDesc.vPoint, vPoints, sizeof(_float3) *3);
		//	NaviDesc.iByOption = iOption;

		m_vecNaviData.emplace_back(vPoints);
		//pGameInstance->Add_PointsToVector(vPoints);
		//m_vecPoints.emplace_back(vPoints);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CTrampoline::OnTriggerEnter(CCollisionObject & collision)
{
	if (LayerType::LAYERTYPE_PLAYER_TRIGGER == collision.Get_Object()->Get_ObjectLayerType())
	{
		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
		dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_TrampolineJump();
		dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Set_CollsionObject(this);
		//dynamic_cast<CPlayer*>(pGameInstace->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->set_collision();
		RELEASE_INSTANCE(CGameInstance);
		m_bIsUpdate = true;
		m_eCurAnimstate = ANIM_DOWN;
		SetUp_AnimValue(DOWN, 2.5);
		OutputDebugString(TEXT("충돌"));
	}
}

_vector CTrampoline::Get_MiddlePos()
{
	_vector vPos = XMVector3TransformCoord(m_pModelCom->Get_MiddlePos(), m_pTransformCom->Get_WorldMatrix());
	return vPos;
}

_vector CTrampoline::Get_Pos()
{
	return m_pTransformCom->Get_Position();
}

void CTrampoline::UpdateAnimState(_double TimeDelta)
{
	switch (m_eCurAnimstate)
	{
	case Client::CTrampoline::ANIM_DOWN:
		SetUp_AnimValue(DOWN, DOWN_SPEED);
		SetUp_AnimFinish(ANIM_UP, UP, UP_SPEED);
		if (15 <= m_pModelCom->Get_AnimationCurrentFrame(m_iCurAnimation))
			m_bStartPlayerJump = true;
		m_bAdjustPlayerStartPos = true;
		break; 
	case Client::CTrampoline::ANIM_UP:
		if (true == m_bStartPlayerJump)
			m_bStartPlayerJump = false;
		SetUp_AnimValue(UP, UP_SPEED);
		SetUp_AnimFinish(ANIM_SETTLE, SETTLE, SETTLE_SPEED);
		break;
	case Client::CTrampoline::ANIM_SETTLE:
		SetUp_AnimValue(SETTLE, SETTLE_SPEED);
		SetUp_AnimFinish(ANIM_DOWN, DOWN, DOWN_SPEED);

		break;
	case Client::CTrampoline::ANIM_END:
		break;
	default:
		break;
	}
}

void CTrampoline::SetUp_AnimValue(_uint iAnimIndex, _double dAnimSpeed)
{
	m_iCurAnimation = iAnimIndex;
	m_dAnimSpeed = dAnimSpeed;
}

void CTrampoline::SetUp_AnimFinish(ANIM eNextAnim, _uint iNextAnimIndex, _double dNextAnimSpeed)
{
	if (true == m_pModelCom->Get_IsAnimFinishied(m_iCurAnimation))
	{

		SetUp_AnimValue(iNextAnimIndex, dNextAnimSpeed);
		m_eCurAnimstate = eNextAnim;
		m_pModelCom->Set_IsAnimLefp(true);

		if (ANIM_DOWN == eNextAnim)
		{
			m_bIsOneFrame = true;
			m_bAdjustPlayerStartPos = false;
		}
		if (ANIM_UP == eNextAnim)
		{
			m_bStartPlayerJump = !m_bStartPlayerJump;
		}

	}
}

HRESULT CTrampoline::SetUp_Component()
{
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, m_StrModelName.c_str(), TEXT("Com_VIBuffer"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColDesc;
	tColDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColDesc.isSceneQuery = true;
	tColDesc.pGameObject = this;

	CNavMeshCollider::DESC tNavMeshDesc;
	tNavMeshDesc.tColliderDesc = tColDesc;
	tNavMeshDesc.vecPoints = m_vecNaviData;

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_Navigation"), TEXT("Com_NavigationCollider"), (CComponent**)&m_pNaviCollider, &tNavMeshDesc)))
		return E_FAIL;

	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	tColliderDesc.isGravity = false;
	tColliderDesc.isSceneQuery = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isTrigger = true;

	CBoxCollider::DESC tBoxColliderDesc;
	tBoxColliderDesc.tColliderDesc = tColliderDesc;
	// Scale에따라 scale조정필요
	tBoxColliderDesc.vScale = { 3.7f,0.05f,3.5f };

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider"), (CComponent**)&m_pTriggerCom, &tBoxColliderDesc)))
		return E_FAIL;

	_matrix smatPviot = XMMatrixTranslation(0.f, 0.0f, 0.f);
	m_pTriggerCom->SetPivotMatrix(smatPviot);
	m_pTriggerCom->Set_ShapeLayer(LayerType::LAYERTYPE_TRIGGER);
	m_pTriggerCom->Update_TransformPhysX();
	m_pTriggerCom->Update_Transform();

	//CPhysXCollider::DESC tColliderDesc_Box;

	//tColliderDesc_Box.eType = CPhysXScene_Manager::TYPE_STATIC;
	////tColliderDesc.isVisualization = true;
	//tColliderDesc_Box.isGravity = false;
	//tColliderDesc_Box.isSceneQuery = true;
	//tColliderDesc_Box.pGameObject = this;
	//CBoxCollider::DESC tBoxColliderDesc_Box;
	//tBoxColliderDesc_Box.tColliderDesc = tColliderDesc_Box;
	//tBoxColliderDesc_Box.vScale = { 4.5f,2.5f,4.5f };

	//if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxBox"), TEXT("Com_Collider_Box"), (CComponent**)&m_pColliderCom, &tBoxColliderDesc_Box)))
	//	return E_FAIL;

	//smatPviot = XMMatrixTranslation(0.f, -1.f, 0.f);

	//m_pColliderCom->SetPivotMatrix(smatPviot);
	//m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_STATIC);
	//m_pColliderCom->Update_TransformPhysX();
	//m_pColliderCom->Update_Transform();

	//m_isActorActive = false;
	//SettingActorActive();
	return S_OK;
}

HRESULT CTrampoline::SetUp_ConstantTable()
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
	_float2		vRimRatio = m_pRendererCom->Get_RimRatio();
	//노말텍스쳐 맵핑의 여부 
	_bool		bIsMapping = true;

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pModelCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	if (FAILED(m_pModelCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightDir", &_float3(0.079f, -0.997f, 0.0f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vLightPos", &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fLightRange", &fLightRange, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRGBA", &m_vRGBA, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_fPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bRimlight", &bRimlight, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bShadow", &bShadow, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_bIsNormalMapping", &bIsMapping, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_vRimRatio", &vRimRatio, sizeof(_float2))))
		return E_FAIL;

	//그림자 맵
	_float3 vLightLook, vLightPos;
	vLightLook = pGameInstance->Get_ShadowLightLook();
	vLightPos = pGameInstance->Get_ShadowLightPos();

	_float4x4 LightView = dynamic_cast<CCamera_Fly*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Camera"), 0))->Get_LightView(vLightLook, vLightPos);

	if (FAILED(m_pModelCom->Set_RawValue("g_LightViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&LightView)), sizeof(_float4x4))))
		return E_FAIL;

	/*모션블러 : 스태틱 메쉬만 모션블러 킬 것이다.*/

	//if (true == m_pRendererCom->Get_RenderOption(CRenderer::RENDEROPTION_MOTIONBLUR))
	//{
	//	m_pModelCom->Set_RawValue("g_OldWorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldWolrd[2])), sizeof(_float4x4));
	//	m_pModelCom->Set_RawValue("g_OldViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OldView[2])), sizeof(_float4x4));

	//}
	//m_OldWolrd[0] = m_pTransformCom->Get_WorldFloat4x4();
	//XMStoreFloat4x4(&m_OldView[0], pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));

	//m_OldWolrd[1] = m_OldWolrd[0];
	//m_OldView[1] = m_OldView[0];

	//m_OldWolrd[2] = m_OldWolrd[1];
	//m_OldView[2] = m_OldView[1];

	//if (FAILED(m_pModelCom->Set_RawValue("g_bMotionBlur", &bMotionBlur, sizeof(_bool))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTrampoline * CTrampoline::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTrampoline*	pInstance = new CTrampoline(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CTrampoline");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrampoline::Clone(void * pArg, _uint iCurrentScene)
{
	CTrampoline*	pInstance = new CTrampoline(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CTrampoline");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrampoline::Free()
{
	__super::Free();

	if (m_pNaviCollider)
	{
		if (m_bisCreated) /* 원본일떄 지우겠다. */
		{
			m_pNaviCollider->Remove_Actor();
			Safe_Release(m_pNaviCollider);
		}

	}
	if (m_pTriggerCom)
	{
		m_pTriggerCom->Remove_Actor();
		Safe_Release(m_pTriggerCom);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);



}
