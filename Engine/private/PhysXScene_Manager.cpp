#include "..\Public\PhysXScene_Manager.h"

#include "GameObject.h"
#include "Transform.h"

#include "ControllerHitReport.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "NavMeshCollider.h"
#include "MeshCollider.h"
#include "CharacterController.h"
#include "QueryFilterCallback.h"
#include "SimulationEventCallBack.h"
#include "ContactReportFilterShader.h"
#include "SimulationFilterCallback.h"
#include "ControllerBehaviorCallback.h"

#include "Model.h"
IMPLEMENT_SINGLETON(CPhysXScene_Manager)

CPhysXScene_Manager::CPhysXScene_Manager()
{

}

HRESULT CPhysXScene_Manager::Initialize_PhysX()
{
	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_Allocator, m_ErrorCallback);
	if (!m_pFoundation)
		return E_FAIL;

#ifdef _DEBUG
	m_pPVD = PxCreatePvd(*m_pFoundation);
	m_pPvdTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, PVD_PORT, PVD_DEFAULT_TIMEOUT);
	bool	bPVDConnectionResult = m_pPVD->connect(*m_pPvdTransport, PxPvdInstrumentationFlag::eALL);
	
	if (!bPVDConnectionResult)
	{
		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);
	}
	else
		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);
#else
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);
#endif _DEBUG

	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	if (!m_pDispatcher)
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysXScene_Manager::Create_Scene()
{
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());

	sceneDesc.gravity = PxVec3(0.f, -9.8f, 0.f);
	sceneDesc.cpuDispatcher = m_pDispatcher;

	sceneDesc.filterShader = ContactReportFilterShader;
	m_pSimulationEventCallBack = new CSimulationEventCallBack();
	sceneDesc.simulationEventCallback = m_pSimulationEventCallBack;
	m_pSimulationFilterCallBack = new CSimulationFilterCallback();
	sceneDesc.filterCallback = m_pSimulationFilterCallBack;

	sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
	sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	sceneDesc.flags |= PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS;
	sceneDesc.flags |= PxSceneFlag::eDEPRECATED_TRIGGER_TRIGGER_REPORTS;

	m_pScene = m_pPhysics->createScene(sceneDesc);

	if (!m_pScene)
		return E_FAIL;

#ifdef _DEBUG
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
#endif // _DEBUG


	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_pDefaultMaterial = m_pPhysics->createMaterial(0.5f, 0.5, 0.6f);

	PxCookingParams cookingParams(m_pPhysics->getTolerancesScale());
	cookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, cookingParams);
	if (!m_pCooking)
		return E_FAIL;

	m_pControllerManager = PxCreateControllerManager(*m_pScene);
	m_pControllerBehaviorCallBack = new CControllerBehaviorCallback();
	m_pHitCallback = new CControllerHitReport();

	m_iLayerMaskSize = (_uint)LayerType::LAYERTYPE_END;
	return S_OK;
}

void CPhysXScene_Manager::Simulate(_double dTimeDelta)
{
	if (!m_pScene)
		return;

	//PX_UNUSED(true);
	m_pScene->simulate(dTimeDelta);
	m_pScene->fetchResults(true);
	if (m_isReadyToPhysX)
	{
		m_pSimulationEventCallBack->UpdateCollsionResult();
		m_pSimulationEventCallBack->UpdateTriggerResult();
	}
	else
		m_pSimulationEventCallBack->NotReadyToCollision();

}

void CPhysXScene_Manager::Add_Actor(PxActor * _pActor)
{
	if (_pActor&&
		nullptr == _pActor->getScene())
		m_pScene->addActor(*_pActor);
}

void CPhysXScene_Manager::Remove_Actor(PxActor * _pActor)
{
	if (_pActor &&
		nullptr != _pActor->getScene())
		m_pScene->removeActor(*_pActor);
}

_uint CPhysXScene_Manager::Add_LayerMask(const _uint iLayer1,const _uint iLayer2)
{
	PxU32 uLayerMask = (1 << iLayer1) + (1 << iLayer2);
	_uint index =0;
	for (auto layer : m_listLayerMask)
	{
		if (layer == uLayerMask)
			return -1;

		++index;
	}
	m_listLayerMask.emplace_back(uLayerMask);
	return index;
}

void CPhysXScene_Manager::Set_ShapeLayer(PxShape * pShape, const _uint iLayer)
{
	PxU32 uShapeLayer = 1 << iLayer;
	PxU32 uLayerMasks = 0;

	for (_uint i = 0; i < m_iLayerMaskSize; ++i)
	{
		PxU32 layerMask = 1 << i;
		for (auto& layer : m_listLayerMask)
		{
			if (layer == (unsigned long long)uShapeLayer + layerMask)
			{
				uLayerMasks |= layerMask;
			}
		}
	}
	PxFilterData pxFilterData;
	pxFilterData.word0 = uShapeLayer;
	pxFilterData.word1 = uLayerMasks;

	pShape->setSimulationFilterData(pxFilterData);
	pShape->setQueryFilterData(pxFilterData);

}

PxMaterial * CPhysXScene_Manager::Create_Material(const PxReal _staticFriction, const PxReal _dynamicFriction, const PxReal _restitution)
{
	PxMaterial* pMaterial = m_pPhysics->createMaterial(_staticFriction, _dynamicFriction, _restitution);
	if (nullptr == pMaterial)
		return nullptr;
	return pMaterial;
}

PxRigidActor * CPhysXScene_Manager::Create_RigidActor(CPhysXCollider * _pCollider)
{
	CPhysXCollider::DESC tDesc = _pCollider->Get_Desc();

	PxRigidActor* pRigidActor = nullptr;
	PxTransform pxTransform;
	pxTransform.p = PxVec3(0.f, 0.f, 0.f);
	pxTransform.q = PxQuat(0.f, 0.f, 0.f, 1.f);
	switch (tDesc.eType)
	{
	case TYPE::TYPE_STATIC:
		pRigidActor = m_pPhysics->createRigidStatic(pxTransform);
		break;
	case TYPE::TYPE_DYNAMIC:
		pRigidActor = m_pPhysics->createRigidDynamic(pxTransform);
		static_cast<PxRigidBody*>(pRigidActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, tDesc.isKinematic);
		static_cast<PxRigidBody*>(pRigidActor)->setMass(tDesc.fMass);
		break;
	}
	pRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !tDesc.isGravity);
#ifdef _DEBUG
	pRigidActor->setActorFlag(PxActorFlag::eVISUALIZATION, !tDesc.isVisualization);
#endif _DEBUG

	return pRigidActor;
}

PxRigidActor * CPhysXScene_Manager::Create_RigidActor(const TYPE _eType, const _bool _isGravity, const _bool _isKinematic, const _bool _isVisualization, const _float _fMass, const PxVec3 & _pxvPosition)
{
	PxRigidActor* pRigidActor = nullptr;
	PxTransform pxTransform;
	pxTransform.p = _pxvPosition;
	pxTransform.q = PxQuat(0.f, 0.f, 0.f, 1.f);
	switch (_eType)
	{
	case TYPE::TYPE_STATIC:
		pRigidActor = m_pPhysics->createRigidStatic(pxTransform);
		break;
	case TYPE::TYPE_DYNAMIC:
		pRigidActor = m_pPhysics->createRigidDynamic(pxTransform);
		static_cast<PxRigidBody*>(pRigidActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, _isKinematic);
		static_cast<PxRigidBody*>(pRigidActor)->setMass(_fMass);
		break;
	}
	pRigidActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_isGravity);
	pRigidActor->setActorFlag(PxActorFlag::eVISUALIZATION, _isVisualization);

	return pRigidActor;
}

HRESULT CPhysXScene_Manager::Create_Box(CBoxCollider * _pCollider)
{
	if (nullptr == m_pPhysics)
		return E_FAIL;

	CPhysXCollider::DESC	tColliderDesc = _pCollider->Get_Desc();

	PxMaterial* pMaterial = Create_Material(tColliderDesc.fStaticFriction, tColliderDesc.fDynamicFriction, tColliderDesc.fRestitution);
	if (nullptr == pMaterial)
		return E_FAIL;

	PxRigidActor*	pRigidActor = Create_RigidActor(_pCollider);
	pRigidActor->userData = tColliderDesc.pGameObject;

	if (nullptr == pRigidActor)
		return E_FAIL;

	PxVec3 pxvExtents = ToPxVec3(_pCollider->Get_Scale());
	pxvExtents *= 0.5f;

	PxShape* pShape = m_pPhysics->createShape(PxBoxGeometry(pxvExtents), *pMaterial, true); //세 번째 인자 Actor가 오브젝트를 독점할것인가 유무를 지정하는것.
	if (nullptr == pShape)
		return E_FAIL;

	pShape->setFlag(PxShapeFlag::eVISUALIZATION, tColliderDesc.isVisualization);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, tColliderDesc.isSceneQuery);
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !tColliderDesc.isTrigger);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, tColliderDesc.isTrigger);
	pShape->userData = tColliderDesc.pGameObject;

	pRigidActor->attachShape(*pShape);
	m_pScene->addActor(*pRigidActor);

	_pCollider->SetMaterial(pMaterial);
	_pCollider->SetShape(pShape);
	_pCollider->SetRigidActor(pRigidActor);

	return S_OK;
}

HRESULT CPhysXScene_Manager::Create_Sphere(CSphereCollider * _pCollider)
{
	if (nullptr == m_pPhysics)
		return E_FAIL;

	CPhysXCollider::DESC	tColliderDesc = _pCollider->Get_Desc();

	PxMaterial* pMaterial = Create_Material(tColliderDesc.fStaticFriction, tColliderDesc.fDynamicFriction, tColliderDesc.fRestitution);
	if (nullptr == pMaterial)
		return E_FAIL;

	PxRigidActor*	pRigidActor = Create_RigidActor(_pCollider);
	pRigidActor->userData = tColliderDesc.pGameObject;

	if (nullptr == pRigidActor)
		return E_FAIL;

	_float fRadius = MaxScaleFromFloat3(_pCollider->Get_Scale());

	PxShape* pShape = m_pPhysics->createShape(PxSphereGeometry(fRadius), *pMaterial, true); //세 번째 인자 Actor가 오브젝트를 독점할것인가 유무를 지정하는것.
	if (nullptr == pShape)
		return E_FAIL;

	pShape->setFlag(PxShapeFlag::eVISUALIZATION, tColliderDesc.isVisualization);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, tColliderDesc.isSceneQuery);
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !tColliderDesc.isTrigger);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, tColliderDesc.isTrigger);
	pShape->userData = tColliderDesc.pGameObject;

	pRigidActor->attachShape(*pShape);
	m_pScene->addActor(*pRigidActor);

	_pCollider->SetMaterial(pMaterial);
	_pCollider->SetShape(pShape);
	_pCollider->SetRigidActor(pRigidActor);

	return S_OK;
}

HRESULT CPhysXScene_Manager::Create_Capsule(CCapsuleCollider * _pCollider)
{
	return E_NOTIMPL;
}

HRESULT CPhysXScene_Manager::Create_NaviMesh(CNavMeshCollider* pCollider,_bool isPlane)
{
	if (nullptr == m_pPhysics)
		return E_FAIL;

	CPhysXCollider::DESC tColDesc = pCollider->Get_Desc();
	PxMaterial* pMaterial = Create_Material(0.5f, 0.5f, 0.5f);
	if (nullptr == pMaterial)
		return E_FAIL;

	PxRigidActor* pRigidActor = Create_RigidActor(pCollider);

	if (nullptr == pRigidActor)
		return E_FAIL;	

	pRigidActor->userData = tColDesc.pGameObject;

	

	if (isPlane)
	{
		_uint iSize = (_uint)m_NaviPoints.size();
		_float3* pPoints = new _float3[iSize];
		for (_uint i = 0; i < iSize; i += 3)
		{
			pPoints[i] = m_NaviPoints[i];
			pPoints[i + 1] = m_NaviPoints[i + 1];
			pPoints[i + 2] = m_NaviPoints[i + 2];
		}

		FACEINDICES32* Indices = new FACEINDICES32[iSize / 3];
		for (_uint i = 0; i < iSize / 3; i++)
		{
			Indices[i]._1 = i * 3;
			Indices[i]._2 = i * 3 + 1;
			Indices[i]._3 = i * 3 + 2;
		}


		PxTriangleMeshDesc meshDesc;
		meshDesc.points.data = pPoints;
		meshDesc.points.count = iSize;
		meshDesc.points.stride = sizeof(_float3);
		meshDesc.triangles.count = iSize / 3;
		meshDesc.triangles.data = Indices;
		meshDesc.triangles.stride = sizeof(FACEINDICES32);



		PxTriangleMesh* pTriMesh = m_pCooking->createTriangleMesh(meshDesc, m_pPhysics->getPhysicsInsertionCallback());
		if (nullptr == pTriMesh)
			return E_FAIL;

		PxShape* pShape = m_pPhysics->createShape(PxTriangleMeshGeometry(pTriMesh), *pMaterial, true);
		if (nullptr == pShape)
			return E_FAIL;

		pShape->setFlag(PxShapeFlag::eVISUALIZATION, tColDesc.isVisualization);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, tColDesc.isSceneQuery);
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !tColDesc.isTrigger);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, tColDesc.isTrigger);
		pShape->userData = tColDesc.pGameObject;
	
		pRigidActor->attachShape(*pShape);
		m_pScene->addActor(*pRigidActor);

		pCollider->SetMaterial(pMaterial);
		pCollider->SetShape(pShape);
		pCollider->SetRigidActor(pRigidActor);

		Safe_Delete_Array(pPoints);
		Safe_Delete_Array(Indices);
	}
	else
	{
		vector<_float3*> vecPoints = pCollider->Get_NaviPoints();
		_uint iSize = (_uint)vecPoints.size();
		_float3* pPoints = new _float3[iSize * 3];
		for (_uint i = 0; i < iSize;++i)
		{
			pPoints[i * 3] = vecPoints[i][0];
			pPoints[i * 3+1] = vecPoints[i][1];
			pPoints[i*3+2] = vecPoints[i][2];
		}

		FACEINDICES32* Indices = new FACEINDICES32[iSize];
		for (_uint i = 0; i < iSize; i++)
		{
			Indices[i]._1 = i * 3;
			Indices[i]._2 = i * 3 + 1;
			Indices[i]._3 = i * 3 + 2;
		}


		PxTriangleMeshDesc meshDesc;
		meshDesc.points.data = pPoints;
		meshDesc.points.count = iSize *3;
		meshDesc.points.stride = sizeof(_float3);
		meshDesc.triangles.count = iSize;
		meshDesc.triangles.data = Indices;
		meshDesc.triangles.stride = sizeof(FACEINDICES32);



		PxTriangleMesh* pTriMesh = m_pCooking->createTriangleMesh(meshDesc, m_pPhysics->getPhysicsInsertionCallback());
		if (nullptr == pTriMesh)
			return E_FAIL;

		PxShape* pShape = m_pPhysics->createShape(PxTriangleMeshGeometry(pTriMesh), *pMaterial, true);
		if (nullptr == pShape)
			return E_FAIL;

		pShape->setFlag(PxShapeFlag::eVISUALIZATION, tColDesc.isVisualization);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, tColDesc.isSceneQuery);
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !tColDesc.isTrigger);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, tColDesc.isTrigger);
		pShape->userData = tColDesc.pGameObject;

		pRigidActor->attachShape(*pShape);
		m_pScene->addActor(*pRigidActor);

		pCollider->SetMaterial(pMaterial);
		pCollider->SetShape(pShape);
		pCollider->SetRigidActor(pRigidActor);

		Safe_Delete_Array(pPoints);
		Safe_Delete_Array(Indices);
	}
	return S_OK;
}

HRESULT CPhysXScene_Manager::Create_MeshCollider(CMeshCollider * pCollider, const _float3* pVerticesPos, _uint VerticesSize, const FACEINDICES32* pIndices, _uint IndiciesSize)
{
	CPhysXCollider::DESC tColDesc = pCollider->Get_Desc();

	PxTriangleMeshDesc meshDesc;

	meshDesc.points.data = pVerticesPos;
	meshDesc.points.count = VerticesSize;
	meshDesc.points.stride = sizeof(_float3);
	meshDesc.triangles.count = IndiciesSize;
	meshDesc.triangles.data = pIndices;
	meshDesc.triangles.stride = sizeof(FACEINDICES32);
	
	PxMaterial* pMaterial = Create_Material(tColDesc.fStaticFriction, tColDesc.fDynamicFriction, tColDesc.fRestitution);
	if (nullptr == pMaterial)
		return E_FAIL;

	PxTriangleMesh* pTriMesh = m_pCooking->createTriangleMesh(meshDesc, m_pPhysics->getPhysicsInsertionCallback());
	if (nullptr == pTriMesh)
		return E_FAIL;
	
	PxRigidActor*	pRigidActor = Create_RigidActor(pCollider);
	pRigidActor->userData = tColDesc.pGameObject;

	PxShape* pShape = m_pPhysics->createShape(PxTriangleMeshGeometry(pTriMesh), *pMaterial, true);
	if (nullptr == pShape)
		return E_FAIL;
	
	pShape->setFlag(PxShapeFlag::eVISUALIZATION, tColDesc.isVisualization);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, tColDesc.isSceneQuery);
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !tColDesc.isTrigger);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, tColDesc.isTrigger);
	pShape->userData = tColDesc.pGameObject;
	
	pRigidActor->attachShape(*pShape);
	m_pScene->addActor(*pRigidActor);
	
	pCollider->SetMaterial(pMaterial);
	pCollider->SetShape(pShape);
	pCollider->SetRigidActor(pRigidActor);

	Safe_Delete_Array(pVerticesPos);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CPhysXScene_Manager::Create_CharacterController(CCharacterController * pCharacterController)
{
	PxCapsuleControllerDesc ControllerDesc;
	CCharacterController::DESC tCharacterControllerDesc = pCharacterController->Get_Desc();

	PxMaterial* pMaterial = Create_Material(tCharacterControllerDesc.fStaticFriction, tCharacterControllerDesc.fDynamicFriction, tCharacterControllerDesc.fRestitution);

	ControllerDesc.climbingMode = tCharacterControllerDesc.eClimbingMode;
	ControllerDesc.contactOffset = tCharacterControllerDesc.fContactOffset;
	ControllerDesc.height = tCharacterControllerDesc.fHeight;
	ControllerDesc.radius = tCharacterControllerDesc.fRadius;
	ControllerDesc.stepOffset = tCharacterControllerDesc.fStepOffset;
	ControllerDesc.slopeLimit = XMConvertToRadians(tCharacterControllerDesc.fSlopeLimit);
	ControllerDesc.upDirection = ToPxVec3(tCharacterControllerDesc.vUpDirection);
	ControllerDesc.material = pMaterial;
	ControllerDesc.behaviorCallback = m_pControllerBehaviorCallBack;
	ControllerDesc.reportCallback = m_pHitCallback;
	//ControllerDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;

	_vector vPosition = pCharacterController->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vPosition += XMLoadFloat3(&tCharacterControllerDesc.vPosition);
	_float3	vPos;
	XMStoreFloat3(&vPos, vPosition);
	ControllerDesc.position = PxExtendedVec3(vPos.x, vPos.y, vPos.z);
	PxController*	pPxController = m_pControllerManager->createController(ControllerDesc);
	pPxController->setUserData(tCharacterControllerDesc.pGameObject);

	vector<PxShape*> vecShapes;
	const PxU32 numShapes = pPxController->getActor()->getNbShapes();
	vecShapes.resize(numShapes);

	PxShape** ppShape = (PxShape**)m_Allocator.allocate(sizeof(PxShape*)*numShapes, 0, __FILE__, __LINE__);
	pPxController->getActor()->getShapes(ppShape, numShapes);
	pPxController->getActor()->userData = tCharacterControllerDesc.pGameObject;

	for (PxU32 i = 0; i < numShapes; ++i)
	{
		PxShape* pShape = ppShape[i];
		pShape->userData = tCharacterControllerDesc.pGameObject;
		vecShapes[i] = pShape;
	}

	if (ppShape)
	{
		m_Allocator.deallocate(ppShape);
		ppShape = NULL;
	}

	pCharacterController->SetPxController(pPxController);
	pCharacterController->SetShapes(vecShapes);
	pCharacterController->SetMaterial(pMaterial);
	
	return S_OK;
}

_bool CPhysXScene_Manager::Raycast(RAYCASTDESCIN tDesc, RAYCASTDESCOUT & tOutDesc)
{
	PxVec3 pxOrigin = ToPxVec3(tDesc.vOrigin);
	PxVec3 pxUnitDir = ToPxVec3(tDesc.vUnitDir);

	tDesc.pxFilterData.flags |= PxQueryFlag::ePREFILTER;


	if (PxQueryFlag::eANY_HIT & tDesc.pxFilterData.flags)
	{
		PxRaycastBuffer pxBufferHit;
		CQueryFilterCallback filterCallback(tDesc.pxLayerMask, true);

 		if (m_pScene->raycast(pxOrigin, pxUnitDir, tDesc.fDistance, pxBufferHit, tDesc.pxHitFlags, tDesc.pxFilterData, &filterCallback))
		{
			if (pxBufferHit.hasBlock)
			{
				PxRaycastHit hitInfo = pxBufferHit.block;
				tOutDesc.iHitCounts = 1;
				tOutDesc.vecHitObjects.emplace_back((CGameObject*)hitInfo.actor->userData);
				tOutDesc.vecHitObjectsPos.emplace_back(FromPxVec3(hitInfo.position));
				if (tOutDesc.ppHitObject)
					*tOutDesc.ppHitObject = (CGameObject*)hitInfo.actor->userData;
				tOutDesc.vHitPos = FromPxVec3(hitInfo.position);
				return true;
			}
		}
	}
	else
	{
		const PxU32 bufferSize = 64;
		PxRaycastHit hitBuffer[bufferSize];
		PxRaycastBuffer buf(hitBuffer, bufferSize);
		CQueryFilterCallback filterCallback(tDesc.pxLayerMask, true);

		if (m_pScene->raycast(pxOrigin, pxUnitDir, tDesc.fDistance, buf, tDesc.pxHitFlags, tDesc.pxFilterData, &filterCallback))
		{
			tOutDesc.iHitCounts = buf.nbTouches;
			tOutDesc.vecHitObjects.reserve(tOutDesc.iHitCounts);
			tOutDesc.vecHitObjectsPos.reserve(tOutDesc.iHitCounts);
			for (PxU32 i = 0; i < tOutDesc.iHitCounts; ++i)
			{
				PxRaycastHit hitInfo = buf.touches[i];
				tOutDesc.vecHitObjects.emplace_back(static_cast<CGameObject*>(hitInfo.actor->userData));
				tOutDesc.vecHitObjectsPos.emplace_back(FromPxVec3(hitInfo.position));
			}

			// 혹시 하나만 검출하고 싶은데 eANY_HIT 를 안넣었을 경우를 위한 대비
			if (tOutDesc.ppHitObject)
				*tOutDesc.ppHitObject = tOutDesc.vecHitObjects[0];
			tOutDesc.vHitPos = tOutDesc.vecHitObjectsPos[0];
			return true;
		}
	}
	return false;
}

const _bool CPhysXScene_Manager::Overlap(OVERLAPDESC & _desc)
{
	PxTransform origin;
	origin.p = ToPxVec3(_desc.vOrigin);
	origin.q = ToPxQuat(_desc.vQuat);
	_desc.filterData.flags |= PxQueryFlag::ePREFILTER;

	if (PxQueryFlag::eANY_HIT & _desc.filterData.flags)
	{
		PxOverlapBuffer hit;
		CQueryFilterCallback filterCallback(_desc.layerMask, true);
		if (m_pScene->overlap(_desc.geometry.any(), origin, hit, _desc.filterData, &filterCallback))
		{
			if (hit.hasBlock)
			{
				PxOverlapHit hitInfo = hit.block;
				_desc.iHitNum = 1;
				_desc.vecHitObjects.emplace_back(static_cast<CGameObject*>(hitInfo.actor->userData));
				if (_desc.ppOutHitObject)
					*_desc.ppOutHitObject = static_cast<CGameObject*>(hitInfo.actor->userData);
				return true;
			}
		}
	}
	else
	{
		const PxU32 bufferSize = 128;
		PxOverlapHit hitBuffer[bufferSize];
		PxOverlapBuffer buf(hitBuffer, bufferSize);
		CQueryFilterCallback filterCallback(_desc.layerMask, false);
		if (m_pScene->overlap(_desc.geometry.any(), origin, buf, _desc.filterData, &filterCallback))
		{
			_desc.iHitNum = buf.nbTouches;
			_desc.vecHitObjects.reserve(_desc.iHitNum);
			for (PxU32 i = 0; i < _desc.iHitNum; ++i)
			{
				PxOverlapHit hitInfo = buf.touches[i];
				_desc.vecHitObjects.emplace_back(static_cast<CGameObject*>(hitInfo.actor->userData));
			}
			if (_desc.ppOutHitObject)
				*_desc.ppOutHitObject = _desc.vecHitObjects[0];
			return true;
		}
	}
	return false;
}

void CPhysXScene_Manager::Add_Points(const _float3 & _vPoint)
{
	_float3 vPoint = _vPoint;

	m_vecPoints.push_back(vPoint);
}

void CPhysXScene_Manager::Clear_PointsList()
{
	m_vecPoints.clear();

	m_vecPoints.swap(vector<_float3>());
}

void CPhysXScene_Manager::Add_PointsToVector(_float3* vPoint)
{
	_float3 vecPoint = vPoint[0];
	m_NaviPoints.emplace_back(vecPoint);
	vecPoint = vPoint[1];
	m_NaviPoints.emplace_back(vecPoint);
	vecPoint = vPoint[2];
	m_NaviPoints.emplace_back(vecPoint);
}

void CPhysXScene_Manager::Clear_NaviPoints()
{
	m_NaviPoints.clear();
}

void CPhysXScene_Manager::Free()
{
	Safe_Delete(m_pControllerBehaviorCallBack);

	Safe_Delete(m_pSimulationFilterCallBack);
	Safe_Delete(m_pHitCallback);
	Safe_Delete(m_pSimulationEventCallBack);
	Safe_PxRelease(m_pControllerManager);
	Safe_PxRelease(m_pCooking);
	Safe_PxRelease(m_pDefaultMaterial);
	Safe_PxRelease(m_pScene);
	Safe_PxRelease(m_pDispatcher);
	Safe_PxRelease(m_pPhysics);
#ifdef _DEBUG
	if (m_pPVD)
		m_pPVD->disconnect();
	Safe_PxRelease(m_pPVD);
	Safe_PxRelease(m_pPvdTransport);
#endif _DEBUG
	Safe_PxRelease(m_pFoundation);
}
