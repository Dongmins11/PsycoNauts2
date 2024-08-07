#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
public:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Intialize_Frustum();
	void Update();
	void Update_Shadow();
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInverse);

public:
	_bool isIn_WorldSpace(_fvector vWorldPoint, _float fRange = 0.f);
	_bool isIn_LocalSpace(_fvector vLocalPoint, _float fRange = 0.f);

private:
	_float3			m_vOriginalPoints[8];
	_float3			m_vTransformPoints[8];

	_float4			m_WorldPlane[6];
	_float4			m_LocalPlane[6];

	/*For.Shadow*/
	_float3			m_vTransformPoints_Shadow[8];

	_float4			m_WorldPlane_Shadow[6];
	_float4			m_LocalPlane_Shadow[6];
private:
	void Make_Plane(_float4* pOuts, _float3* pPoints);


public:
	virtual void Free() override;
};

END