#pragma once

#include "Base.h"
#include "DebugDraw.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
	//DECLARE_SINGLETON(CPicking);

private:
	CPicking();
	virtual ~CPicking() = default;
public:
	HRESULT NativeConstruct(ID3D11Device* pDievce, ID3D11DeviceContext* pDeviceContext, HWND hWnd);
	void Transform_ToWorldSpace();
	_bool Picking_InLocalSpace(class CTransform* pTransform, class CVIBuffer* pVIBuffer, _float3* pOut);
	_bool Picking_ShortInLocalSpace(class CTransform* pTransform, class CVIBuffer* pVIBuffer, _float3* pOut);
	_bool Picking_InLocalSpace(_fmatrix* _modelWorldMat, class CVIBuffer* pVIBuffer, _float3* pOut);
	_bool Picking_InWorldSpace(CTransform * _pTransform, class CCollider* pCollider, _float3* pOut, _float* dist);
	_bool Picking_InLocalSpaceWithDist(_fmatrix* _modelWorldMat, class CVIBuffer* pVIBuffer, _float3* pOut, _float* pDist);
	_vector Get_RayDir() { return XMLoadFloat3(&m_vRayDir); }
	_vector Get_RayPos() { return XMLoadFloat3(&m_vRayPos); }

private:
	/*CONST D3DXVECTOR3 *p0,
		CONST D3DXVECTOR3 *p1,
		CONST D3DXVECTOR3 *p2,
		CONST D3DXVECTOR3 *pRayPos,
		CONST D3DXVECTOR3 *pRayDir,
		FLOAT *pU,
		FLOAT *pV,
		FLOAT *pDist);*/
	_bool IntersectTri(const _fvector& p0,const _fvector& p1,const _fvector& p2,const _gvector* pRayPos, const _hvector* pRayDir
	, _float* pU, _float* pV, _float* pDist );

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HWND					m_hWnd;
	
private:
	_float3					m_vRayDir;
	_float3					m_vRayPos;
	


public:
	virtual void Free() override;
	
};

END