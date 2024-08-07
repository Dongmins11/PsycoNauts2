#pragma once

#include "Base.h"
#include "PxPhysicsAPI.h"

#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425
#define PVD_DEFAULT_TIMEOUT 10

#define ToPxVec3(v)					PxVec3((v).x, (v).y, (v).z)
#define FromPxVec3(v)				_float3((v).x, (v).y, (v).z)
#define ToPxExtendedVec3(v)			PxExtendedVec3((v).x, (v).y, (v).z)
#define FromPxextendedVec3(v)		_float3((_float)(v).x, (_float)(v).y, (_float)(v).z)
#define ToPxQuat(v)					PxQuat((v).x, (v).y, (v).z, (v).w)
#define FromPxQuat(v)				_float4((v).x, (v).y, (v).z, (v).w)
using namespace physx;

BEGIN(Engine)

class CPhysXCollider;
class CBoxCollider;
class CSphereCollider;
class CCapsuleCollider;
class CNavMeshCollider;
class CMeshCollider;
class CCharacterController;
class CGameObject;
class CControllerBehaviorCallback;
class CControllerHitReport;
class CQueryFilterCallback;
class CSimulationEventCallBack;
class CSimulationFilterCallback;

class CPhysXScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CPhysXScene_Manager)

	
public:
	enum TYPE{TYPE_STATIC, TYPE_DYNAMIC, TYPE_END};

	typedef struct RayCastDescIn
	{
		_float3 vOrigin = _float3(0.f, 0.f, 0.f);
		_float3 vUnitDir = _float3(0.f, 0.f, 0.f);
		_float fDistance;
		PxHitFlags pxHitFlags;
		PxQueryFilterData pxFilterData;
		PxU32		pxLayerMask;
	}RAYCASTDESCIN;

	typedef struct RayCastDescOut
	{
		_float3 vHitPos;
		CGameObject** ppHitObject = nullptr;
		_uint	iHitCounts = 0;
		vector<CGameObject*> vecHitObjects;
		vector<_float3> vecHitObjectsPos;
	}RAYCASTDESCOUT;

	typedef struct tagOverlapDesc
	{
		// in
		PxGeometryHolder geometry;
		_float3 vOrigin = { 0.f, 0.f, 0.f };
		_float4 vQuat = { 0.f, 0.f, 0.f, 1.f };
		PxQueryFilterData filterData;
		PxU32 layerMask = 0;
		// out
		CGameObject** ppOutHitObject = nullptr;
		_uint iHitNum = 0;
		vector<CGameObject*> vecHitObjects;
	}OVERLAPDESC;


private:
	CPhysXScene_Manager();
	virtual ~CPhysXScene_Manager() = default;

public:
	HRESULT Initialize_PhysX();
	HRESULT Create_Scene();

public:
	void Set_IsReadyPhysX(_bool isReady) { m_isReadyToPhysX = true; }
public:
	void Simulate(_double dTimeDelta);
	void Add_Actor(PxActor* _pActor);
	void Remove_Actor(PxActor* _pActor);

	// raycast¿ë LayerMask
	_uint	Add_LayerMask(const _uint iLayer1,const _uint iLayer2);
	void	Set_ShapeLayer(PxShape* pShape, const _uint iLayer);

private:
	PxMaterial* Create_Material(const PxReal _staticFriction, const PxReal _dynamicFriction, const PxReal _restitution);
	PxRigidActor* Create_RigidActor(CPhysXCollider* _pCollider);
	PxRigidActor* Create_RigidActor(const TYPE _eType, const _bool _isGravity, const _bool _isKinematic, const _bool _isVisualization = true
		, const _float _fMass = 1.f, const PxVec3& _pxvPosition = { 0.f, 0.f, 0.f });

public:
	HRESULT Create_Box(CBoxCollider* _pCollider);
	HRESULT Create_Sphere(CSphereCollider* _pCollider);
	HRESULT Create_Capsule(CCapsuleCollider* _pCollider);
	HRESULT Create_NaviMesh(CNavMeshCollider* pCollider, _bool isPlane);
	HRESULT Create_MeshCollider(CMeshCollider* pCollider, const _float3* pVerticesPos, _uint VerticesSize, const FACEINDICES32* pIndices, _uint IndiciesSize);
	HRESULT Create_CharacterController(CCharacterController* pCharacterController);


public:
	_bool	Raycast(RAYCASTDESCIN tDesc, RAYCASTDESCOUT& tOutDesc);
	const _bool Overlap(OVERLAPDESC& _desc);

	void Add_Points(const _float3& _vPoint);
	void Clear_PointsList();

	void Add_PointsToVector(_float3* vPoint);
	vector<_float3> Get_PointVector() { return m_NaviPoints; };
	void Clear_NaviPoints();
private:
	PxDefaultAllocator				m_Allocator;
	PxDefaultErrorCallback			m_ErrorCallback;
	PxFoundation*					m_pFoundation = nullptr;
	PxPhysics*						m_pPhysics = nullptr;
	PxDefaultCpuDispatcher*			m_pDispatcher = nullptr;
	PxPvd*							m_pPVD = nullptr;
	PxPvdTransport*					m_pPvdTransport = nullptr;
	PxScene*						m_pScene = nullptr;
	PxCooking*						m_pCooking = nullptr;
	PxRigidActor*					m_pNaviActor = nullptr;
	PxControllerManager*			m_pControllerManager = nullptr;
	CControllerHitReport*			m_pHitCallback = nullptr;
	CSimulationEventCallBack*		m_pSimulationEventCallBack = nullptr;
	CSimulationFilterCallback*		m_pSimulationFilterCallBack = nullptr;
	CControllerBehaviorCallback*	m_pControllerBehaviorCallBack = nullptr;
	PxMaterial*						m_pDefaultMaterial = nullptr;

private:
	vector<_float3>					m_vecPoints;
	vector<_float3>					m_NaviPoints;
	_uint							m_iLayerMaskSize = 0;
	list<_uint>						m_listLayerMask;

	_bool							m_isReadyToPhysX = false;
public:
	virtual void Free() override;
};

END