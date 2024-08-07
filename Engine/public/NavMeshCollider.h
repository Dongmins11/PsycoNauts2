#pragma once

#include "PhysXCollider.h"

BEGIN(Engine)

class ENGINE_DLL CNavMeshCollider final : public CPhysXCollider
{
public:
	typedef struct tagDesc
	{
		CPhysXCollider::DESC tColliderDesc;
		vector<_float3*> vecPoints;
		_bool			m_isPlane = false;
	}DESC;
private:
	explicit CNavMeshCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CNavMeshCollider(const CNavMeshCollider& _rhs);
	virtual ~CNavMeshCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	vector<_float3*>& Get_NaviPoints();

private:
	vector<_float3*> m_vecPoints;

public:
	static CNavMeshCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END