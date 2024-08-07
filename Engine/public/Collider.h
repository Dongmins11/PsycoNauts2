#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	typedef struct tagColliderDesc
	{
		_float3		vCenter;
		_float3		vExtents;
		_float		fRadius;
	}COLLIDERDESC;

	typedef struct tagAABBDesc
	{
		_float3		vMin, vMax;
	}AABBDESC;

	typedef struct tagOBBDesc
	{
		_float3		vCenter;
		_float3		vExtentDir[3];
		_float3		vAlignDir[3];
	}OBBDESC;


public:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	_vector Get_CenterPos() { return XMLoadFloat3(&m_ColliderDesc.vCenter); }
	_float3 Get_Extents() { return m_ColliderDesc.vExtents; }
	_float	Get_Radius() const { return  m_ColliderDesc.fRadius; }
	_float3 Get_ColliderCenter(TYPE _Coltype);

public:
	void Set_CenterPos(_float3 vCenterPos) { m_ColliderDesc.vCenter = vCenterPos; }
	void Set_Extents(_float3 vExtents) { m_ColliderDesc.vExtents = vExtents; }
	void Set_Radius(_float _radius) { m_ColliderDesc.fRadius = _radius; }

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eColliderType);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Update(_fmatrix WorldMatrix);

	_bool RayCastCollider(_fvector vOriginPos, _fvector vRayDir, _float* fDist, CCollider* pTarget);
	_bool PickingCollider(_fvector vOriginPos, _fvector vRayDir, _float* fDist);
	_bool Collision_AABB(CCollider* pTarget, TYPE _Coltype = TYPE_AABB);
	_bool Collision_OBB(CCollider* pTarget, TYPE _Coltype = TYPE_OBB);
	_bool Collision_Sphere(CCollider* pTarget, TYPE _Coltype = TYPE_SPHERE);
	void Set_ColliderColor(_vector Color) { m_ColliderColor = Color; }
	void Set_ColliderNewColor(_bool ColorCheck) { m_ColliderColorCheck = ColorCheck; }
#ifdef _DEBUG
public:
	HRESULT Render(_fvector _vColor = XMVectorZero());
#endif // _DEBUG

private:
	BoundingBox*			m_pAABB = nullptr;
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingSphere*			m_pSphere = nullptr;

	TYPE					m_eType = TYPE_END;
	COLLIDERDESC			m_ColliderDesc;
	_vector					m_ColliderColor = { 0.f,0.f,0.f,0.f };
	_bool					m_isCollision = false;
	_bool					m_ColliderColorCheck = false;

private:
	BasicEffect*			m_pEffect = nullptr;
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;
	ID3D11InputLayout*		m_pInputLayout = nullptr;

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	OBBDESC Compute_OBBDesc();

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END