#include "stdafx.h"
#include "..\public\Blender2.h"
#include "GameInstance.h"
#include "Player.h"


CBlender2::CBlender2(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CBlender2::CBlender2(const CBlender2 & rhs)
	: CGameObject(rhs)
	//, m_vecNaviData(rhs.m_vecNaviData)
{
}

HRESULT CBlender2::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	//Loading_PlateNaviData(TEXT("../../Data/NaviBlender2.dat"));

	return S_OK;
}

HRESULT CBlender2::NativeConstruct(void * pArg, _uint iCurrentScene)
{

	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	m_isCreated = true;
	OBJINFO	objinfo;
	if (nullptr != pArg)
		objinfo = *(OBJINFO*)pArg;

	m_NavigationIndex = objinfo.iNavigationIndex;
	m_iCurlingIndex = objinfo.iCurlingIndex;
	m_iDeleteCurlingIndex = objinfo.iDeleteCurlingIndex;
	m_iRenderPass = objinfo.iRenderPass;

	m_StrLayerName = objinfo.StrLayerName;
	m_StrModelName = objinfo.StrModelName;
	m_StrObjectFullName = TEXT("Prototype_GameObject_Blender2"); //objinfo.StrObjectFullName;
	m_iPX_TYPE = (_uint)LayerType::LAYERTYPE_CLIMBING;//objinfo.iPX_Index;
	m_eLayerType = LayerType::LAYERTYPE_CLIMBING;
	if (FAILED(SetUp_Component()))
		return E_FAIL;

	_float4x4 fWorldMatrix;
	XMStoreFloat4x4(&fWorldMatrix, objinfo.WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(fWorldMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));

	//_float fLookScale = m_pTransformCom->Get_ScaleFloat3().z;
	//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vLook);
	m_iRenderPass = 0;
	m_iRenderPassBackUp = m_iRenderPass;

	Loading_PlateNaviData(TEXT("../../Data/NaviBlender3.dat"));


	CPhysXCollider::DESC tColDesc;
	tColDesc.eType = CPhysXScene_Manager::TYPE_STATIC;
	//tColDesc.isTrigger = false;
	tColDesc.isSceneQuery = true;
	tColDesc.pGameObject = this;

	CNavMeshCollider::DESC tNavMeshDesc;
	tNavMeshDesc.tColliderDesc = tColDesc;
	tNavMeshDesc.vecPoints = m_vecNaviData; 
	//tNavMeshDesc.m_isPlane = true;
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_Navigation"), TEXT("Com_NavigationCollider"), (CComponent**)&m_pColliderCom, &tNavMeshDesc)))
		return E_FAIL;
	
	m_pColliderCom->Set_ShapeLayer(LayerType::LAYERTYPE_CLIMBINGPLANE);
	_matrix smatPviot = XMMatrixIdentity();//m_pTransformCom->Get_WorldMatrix();
	m_pColliderCom->SetPivotMatrix(smatPviot);
	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();

	SetUp_Triggers();
	return S_OK;
}

_int CBlender2::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//
	//RELEASE_INSTANCE(CGameInstance);

	//m_pColliderCom->Update_TransformPhysX();
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CTransform* PlayerTramsform = (CTransform*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Transform"));
	//CCharacterController* PlayerController = (CCharacterController*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Controller"));
	//CSphereCollider* PlayerCollider = (CSphereCollider*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Collider"));

	//if (pGameInstance->Key_Pressing('O'))
	//{
	//	m_bMove = true;
	//	m_vDir = PlayerTramsform->Get_State_Float3(CTransform::STATE_RIGHT);
	//}
	//if (pGameInstance->Key_Pressing('I'))
	//{
	//	m_bMove = true;
	//	m_vDir = STOREFLOAT3(-1.f* XMLoadFloat3(&PlayerTramsform->Get_State_Float3(CTransform::STATE_RIGHT)));
	//}


	//if (m_bClimbing && true ==m_bMove) /* Climbing을 진행할수 있게 해주겠습니다. */
	//{
	//	CheckOnClimbing2(TimeDelta);
	//	//CheckOnClimbing(TimeDelta);
	//}
	//RELEASE_INSTANCE(CGameInstance);

	return 0;
}

_int CBlender2::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	//for (_uint i = 0; i < 2; ++i)
	//{
	//	if (m_pTriggerCom[i])
	//	{
	//m_pColliderCom->Update_Transform();
	//
	//	}
	//}
	//m_pColliderCom->Update_Transform();
	if (true != m_isDestroy)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

#ifdef _DEBUG
	//for(_uint i=0;i<2;++i)
	//	m_pTriggerRenderCom[i]->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG
	return 0;
}

HRESULT CBlender2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iRenderPass);

	return S_OK;
}

void CBlender2::CheckOnClimbing(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* PlayerTramsform = (CTransform*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Transform"));
	CCharacterController* PlayerController = (CCharacterController*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Controller"));
	CSphereCollider* PlayerCollider = (CSphereCollider*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Collider"));
	_vector vDir;
	_vector vUpDir;

	/* 안되면 풀어주세요 */
	//vDir = XMVector3Normalize(PlayerTramsform->Get_State(CTransform::STATE_RIGHT)); /* Player 이동을 위해서 */
	//vUpDir= XMVector3Normalize(PlayerTramsform->Get_State(CTransform::STATE_UP)); /* 위로 이동을 하기 위해서 */


	_matrix TriggerRight = XMLoadFloat4x4(&m_Move4x4);
	_vector vRight = TriggerRight.r[0];
	_vector vUp = TriggerRight.r[1];
	_vector vLook = TriggerRight.r[2];
	//_vector JumpLook = XMVector3Normalize(vLook);
	//JumpLook = JumpLook*-1.f;
	vDir = XMVector3Normalize(vRight);
	vLook = XMVector3Normalize(vLook);
	vDir = -1.f * vDir;
	vLook = -1.f * vLook;
	//vUpDir = XMVector3Normalize(PlayerTramsform->Get_State(CTransform::STATE_UP));
	//if (pGameInstance->Key_Pressing('A')) /* 반대쪽 반향으로 이동을 해야하기 때문에 */
	//	vDir *= -1.f;
	//if (pGameInstance->Key_Pressing('S')) /* 아래 방향으로 이동을 해줘야 한다.*/
	//	vUpDir *= -1.f;

	/* 확인용으로 사용할것입니다, */
	//if (pGameInstance->Key_Pressing(VK_LEFT)) /* 반대쪽 반향으로 이동을 해야하기 때문에 */
	//	vDir *= -1.f;
	//if (pGameInstance->Key_Pressing(VK_DOWN)) /* 아래 방향으로 이동을 해줘야 한다.*/
	//	vUpDir *= -1.f;

	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
	_float3 vVelocity;
	_float4 vUpRayDirection;
	// 내 현재 좌표 + 한틱 이동한 만큼의 값 더해(한틱 앞에있는 포지션값)
	XMStoreFloat3(&tDescIn.vOrigin, PlayerTramsform->Get_Position() + XMVector3Normalize(vDir)* TimeDelta);
	// 레이 Direction을 Look 
	//XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(PlayerTramsform->Get_State(CTransform::STATE_LOOK)));
	XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(vLook));

	//_vector newpos = -1.f*(m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	//XMStoreFloat3(&tDescIn.vUnitDir, XMVector3Normalize(newpos));
	// raycast 길이
	//tDescIn.fDistance = 1.f;
	tDescIn.fDistance = 0.5f;

	tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
	// 어떤 레이어오브젝트랑만 충돌할 것인지 ( 네비메쉬의 layerType을 Plane으로 설정해서 Plane만 체킹해줌)
	// 어떤레이어로 설정하고 싶으면 Set_ShapeLayer() 함수 사용
	tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
	// 부딪힌 오브젝트를 받아오기 위한 변수
	CGameObject* pHitObject = nullptr;
	// Out에 받아오기 때문에 주소값 던져줌
	tDescOut.ppHitObject = &pHitObject;

	if (false == pGameInstance->Key_Down(VK_SPACE)) /* Player의 움직임이 있어도 Raycast 해야되기 때문에 */
	{
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (pHitObject) /* Raycast 가 true -> 원통에 있는 네비 매쉬에 근처에 있다라는것 입니다. */
			{
				_float4 vClimbingPos;
				_float4 vPlayerPos;
				_vector vLook;
				XMStoreFloat4(&vClimbingPos, m_pTransformCom->Get_Position());
				_vector vNewPos = XMVectorSet(vClimbingPos.x, vPlayerPos.y, vClimbingPos.z, 1.f); /* y값을 죽인 좌표를 생성하고 */
				vLook = XMVector3Normalize(vNewPos - XMLoadFloat4(&vPlayerPos));
				PlayerTramsform->Set_MyLook(vLook);

				if (pGameInstance->Key_Pressing('O'))
				{

					vVelocity = STOREFLOAT3(XMLoadFloat3(&tDescOut.vHitPos) - PlayerTramsform->Get_Position()); /* 충돌되었을때의 벡터일것이다. */

					XMStoreFloat4(&vUpRayDirection, XMVector4Normalize(XMLoadFloat3(&tDescOut.vHitPos) - PlayerTramsform->Get_Position()));
					// 좌표 세팅
					OutputDebugString(TEXT("ㅅㅂ !!\n"));

					PlayerTramsform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&tDescOut.vHitPos));
					_vector vPlayerLook = PlayerTramsform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook = XMVector3Normalize(vPlayerLook * -1.f); /* 노말로 만들고 */
					_float PlayerLookScale = PlayerTramsform->Get_ScaleFloat3().z / 2.f;
					_vector Pos = vPlayerLook * PlayerLookScale;
					PlayerTramsform->Set_State(CTransform::STATE_POSITION, PlayerTramsform->Get_Position() + Pos);
					// 원래 있었던 이동값을 다 0으로 초기화
					_double dAdjustSpeed = 0.2f;
					//_float3 vVelocity = STOREFLOAT3(XMLoadFloat4(&vUpRayDirection)* dAdjustSpeed);
					//PlayerTramsform->Set_Velocity(XMVectorZero());
					PlayerController->Move(TimeDelta*3.0, vVelocity);
					PlayerCollider->Update_TransformPhysX();
					PlayerCollider->Update_Transform();
					/*_float3 fFootPos;
					XMStoreFloat3(&fFootPos, PlayerTramsform->Get_Position());
					PlayerController->Set_FootPosition(fFootPos);*/
				}
			}
		}
	}
	else /* 스페이바를 눌렀을때면 Player가 점프 진행된것이기 때문에 점프중에서도 어색함을 없애기위해서는 어느정도 해당 방향으로 이동값을 줘야 관성의 힘을 받는것 같은 착시를 줄수있다. */
	{
		_double dAdjustSpeed = 1.f;
		//_float3 vVelocity = STOREFLOAT3(XMVector3Normalize(JumpLook)* dAdjustSpeed);
		_float3 vVelocity = STOREFLOAT3(XMVector3Normalize(vDir)* dAdjustSpeed);
		PlayerController->Move(TimeDelta, vVelocity);
		PlayerCollider->Update_TransformPhysX();
		PlayerCollider->Update_Transform();
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBlender2::CheckOnClimbing2(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* PlayerTramsform = (CTransform*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Transform"));
	CCharacterController* PlayerController = (CCharacterController*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Controller"));
	CSphereCollider* PlayerCollider = (CSphereCollider*)static_cast<CPlayer*>(pGameInstance->Get_GameObject(SCENE_STATIC, TEXT("Layer_Player"), 0))->Get_Components(TEXT("Com_Collider"));
	_vector vDir;
	_vector vUpDir;
	_matrix TriggerRight = XMLoadFloat4x4(&m_Move4x4);
	_vector vRight = TriggerRight.r[0];
	_vector vUp = TriggerRight.r[1];
	_float3 vLook =STOREFLOAT3(TriggerRight.r[2]);

	vLook =STOREFLOAT3(XMVector3Normalize(-1.f*XMLoadFloat3(&vLook)));

	CPhysXScene_Manager::RAYCASTDESCIN tDescIn;
	CPhysXScene_Manager::RAYCASTDESCOUT tDescOut;
	_float3 vVelocity;
	_float4 vUpRayDirection;

	XMStoreFloat3(&tDescIn.vOrigin, PlayerTramsform->Get_Position() + XMVector3Normalize(XMLoadFloat3(&m_vDir))*TimeDelta*0.3f);
	XMStoreFloat3(&tDescIn.vUnitDir,XMVector3Normalize(PlayerTramsform->Get_State(CTransform::STATE_LOOK)));

	tDescIn.fDistance = 1.f;

	tDescIn.pxFilterData.flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eSTATIC;
	tDescIn.pxLayerMask = (1 << (_uint)LayerType::LAYERTYPE_PLANE);
	CGameObject* pHitObject = nullptr;
	tDescOut.ppHitObject = &pHitObject;

	if (false == pGameInstance->Key_Down(VK_SPACE)) /* Player의 움직임이 있어도 Raycast 해야되기 때문에 */
	{
		if (pGameInstance->Raycast(tDescIn, tDescOut))
		{
			if (pHitObject) /* Raycast 가 true -> 원통에 있는 네비 매쉬에 근처에 있다라는것 입니다. */
			{
				_float4 vClimbingPos;
				_float4 vPlayerPos;
				_vector vPlayerLook;
				XMStoreFloat4(&vClimbingPos, m_pTransformCom->Get_Position());
				XMStoreFloat4(&vPlayerPos, PlayerTramsform->Get_Position());
				_vector vNewPos = XMVectorSet(vClimbingPos.x, vPlayerPos.y, vClimbingPos.z, 1.f); /* y값을 죽인 좌표를 생성하고 */
				vPlayerLook = XMVector3Normalize(vNewPos - XMLoadFloat4(&vPlayerPos));
				PlayerTramsform->Set_MyLook(vPlayerLook);
				PlayerTramsform->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
				PlayerController->Set_FootPosition(tDescOut.vHitPos);
				
				PlayerTramsform->Set_State_Float3(CTransform::STATE_POSITION, tDescOut.vHitPos);
				_double dAdjustSpeed = 0.001;
				_float3 vVelocity = STOREFLOAT3(XMVector3Normalize(XMLoadFloat3(&m_vDir))* dAdjustSpeed);
				PlayerController->Move(TimeDelta, vVelocity);
				PlayerCollider->Update_TransformPhysX();
				PlayerCollider->Update_Transform();
			}
		}
	}
	else /* 스페이바를 눌렀을때면 Player가 점프 진행된것이기 때문에 점프중에서도 어색함을 없애기위해서는 어느정도 해당 방향으로 이동값을 줘야 관성의 힘을 받는것 같은 착시를 줄수있다. */
	{
		_double dAdjustSpeed = 1.f;
		//_float3 vVelocity = STOREFLOAT3(XMVector3Normalize(JumpLook)* dAdjustSpeed);
		_float3 vVelocity = STOREFLOAT3(XMVector3Normalize(vDir)* dAdjustSpeed);
		PlayerController->Move(TimeDelta, vVelocity);
		PlayerCollider->Update_TransformPhysX();
		PlayerCollider->Update_Transform();
	}
	RELEASE_INSTANCE(CGameInstance);

	m_bMove = false;
}

void CBlender2::Set_Climbing(_bool ClimbingCheck, _matrix Matrix)
{
	m_bClimbing = ClimbingCheck;
	XMStoreFloat4x4(&m_Move4x4, Matrix);
}

HRESULT CBlender2::SetUp_Component()
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_iMyCount = pGameInstance->Get_LayerSize(SCENE_STAGE03, TEXT("Layer_Blender2"));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBlender2::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 TransPoseWorldMat, ViewTransPoseMat, ProjectionTransPoseMat;

	XMStoreFloat4x4(&TransPoseWorldMat, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjectionTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &TransPoseWorldMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjectionTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBlender2::Loading_PlateNaviData(const _tchar * pNaviDataFilePath)
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

		for (_uint i = 0; i < 3; ++i)
			vPoints[i] = STOREFLOAT3(XMLoadFloat3(&vPoints[i]) * 0.8f);

		m_vecNaviData.emplace_back(vPoints);
		//pGameInstance->Add_PointsToVector(vPoints);
		//m_vecPoints.emplace_back(vPoints);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CBlender2::SetUp_Triggers()
{
	return S_OK;
}

CBlender2 * CBlender2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBlender2*	pInstance = new CBlender2(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CBlender2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBlender2::Clone(void * pArg, _uint iCurrentScene)
{
	CBlender2*	pInstance = new CBlender2(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CBlender2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBlender2::Free()
{
	__super::Free();

	if (m_pColliderCom)
	{
		m_pColliderCom->Remove_Actor();
		//else
		//{
		if (m_isCreated) /* 원본일떄 지우겠다. */
		{
			//for (auto& iter : m_vecNaviData)
			//	Safe_Delete(iter);
			//
			//m_vecNaviData.clear();

			//	Safe_Release(m_pColliderCom);
			Safe_Release(m_pColliderCom);
		}
		//}
	}
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
