#include "..\public\Picking.h"
#include "Transform.h"
#include "VIBuffer.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::NativeConstruct(ID3D11Device* pDievce, ID3D11DeviceContext* pDeviceContext, HWND hWnd)
{
	m_pDevice = pDievce;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	m_hWnd = hWnd;

	return S_OK;
}

void CPicking::Transform_ToWorldSpace()
{
	POINT			ptMouse;
	GetCursorPos(&ptMouse);
	/* 뷰포트 */
	ScreenToClient(m_hWnd, &ptMouse);
	_float3					vMousePos;

	D3D11_VIEWPORT ViewPort;
	_uint iNum = 1;
	m_pDeviceContext->RSGetViewports(&iNum, &ViewPort);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 투영스페이스 */
	vMousePos.x = (ptMouse.x - (ViewPort.Width / 2)) / (ViewPort.Width /2);
	vMousePos.y = (ptMouse.y - (ViewPort.Height /2)) / ((ViewPort.Height /2) * -1.f);
	vMousePos.z = 0.f;

	_matrix projMat = pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	projMat = XMMatrixInverse(nullptr, projMat);
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), projMat));

	m_vRayPos = _float3(0.f, 0.f, 0.f);
	XMStoreFloat3(&m_vRayDir,XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vRayPos));

	/* 월드스페이스 */
	_matrix		ViewMatrix;
	ViewMatrix = pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	XMStoreFloat3(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), ViewMatrix));
	XMStoreFloat3(&m_vRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), ViewMatrix));

	RELEASE_INSTANCE(CGameInstance);

}

_bool CPicking::Picking_InLocalSpace(CTransform * pTransform, CVIBuffer * pVIBuffer, _float3* pOut)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	POINT			ptMouse;
	GetCursorPos(&ptMouse);
	/* 뷰포트 */
	ScreenToClient(m_hWnd, &ptMouse);
	_float3					vMousePos;
	
	D3D11_VIEWPORT ViewPort;
	_uint iNum = 1;
	m_pDeviceContext->RSGetViewports(&iNum, &ViewPort);
	
	if (ptMouse.x > ViewPort.Width || ptMouse.y > ViewPort.Height
		|| ptMouse.x < 0 || ptMouse.y < 0)
		return false;

	_matrix		WorldMatrixInv = pTransform->Get_WorldMatrixInverse();

	_vector			vLocalRayPos, vLocalRayDir;
	/* 로컬스페이스 */
	vLocalRayPos = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos),WorldMatrixInv);
	vLocalRayDir = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv);
	//XMStoreFloat3(&, 
	//D3DXVec3TransformNormal(&vLocalRayDir, &m_vRayDir, &WorldMatrixInv);
	vLocalRayDir = XMVector3Normalize(vLocalRayDir);
	//D3DXVec3Normalize(&vLocalRayDir, &vLocalRayDir);

	_uint	iNumPrimitive = pVIBuffer->Get_NumPrimitive();
	_uint indicesSize = pVIBuffer->Get_PrimitiveIndicesSize();
	const _float3* pVerticesPos = pVIBuffer->Get_VerticesPos();

	_uint iIndex = 0;
	if (indicesSize / 3 > 2)
	{
		_ulong* pIndices = (_ulong*)pVIBuffer->Get_Indices();

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			_float		fU, fV, fDist;
			if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				&vLocalRayPos,
				&vLocalRayDir,
				&fU, &fV, &fDist))
			{
				_vector vec = vLocalRayPos + (vLocalRayDir * fDist);
				XMStoreFloat3(pOut, vec);
				return true;

			}

		}
	}
	else
	{
		_ushort* pIndices = (_ushort*)pVIBuffer->Get_Indices();

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			_float		fU, fV, fDist;
			if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				&vLocalRayPos,
				&vLocalRayDir,
				&fU, &fV, &fDist))
			{
				_vector vec = vLocalRayPos + (vLocalRayDir * fDist);
				 XMStoreFloat3(pOut,vec);
				 return true;

			}

		}
	}


	
	//RELEASE_INSTANCE(CGameInstance);

	return false;
}

_bool CPicking::Picking_ShortInLocalSpace(CTransform * pTransform, CVIBuffer * pVIBuffer, _float3 * pOut)
{
	_matrix  worldMatrix = pTransform->Get_WorldMatrixInverse();

	_vector  vLocalRayPos, vLocalRayDir;

	vLocalRayPos = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), worldMatrix);
	vLocalRayDir = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), worldMatrix);
	vLocalRayDir = XMVector3Normalize(vLocalRayDir);

	const _uint			iNumPrimitive = pVIBuffer->Get_NumPrimitive();
	const _ulong*	pIndices = (_ulong*)pVIBuffer->Get_Indices();
	const _float3*pVerticesPos = pVIBuffer->Get_VerticesPos();

	list<_float> ListDist;

	_uint iIndex = 0;
	for (_uint i = 0; i < iNumPrimitive; ++i)
	{
		_float		fU, fV, fDist;
		if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
			XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
			XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
			&vLocalRayPos,
			&vLocalRayDir,
			&fU, &fV, &fDist))
		{
			ListDist.push_back(fDist);
		}

	}
	if (0 == ListDist.size())
		return false;

	ListDist.sort();

	XMStoreFloat3(pOut, vLocalRayPos + vLocalRayDir* *(ListDist.begin()));
	return true;
}

_bool CPicking::Picking_InLocalSpace(_fmatrix * _modelWorldMat, CVIBuffer * pVIBuffer, _float3 * pOut)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr,*_modelWorldMat);

	_vector			vLocalRayPos, vLocalRayDir;
	/* 로컬스페이스 */
	vLocalRayPos = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv);
	vLocalRayDir = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv);
	vLocalRayDir = XMVector3Normalize(vLocalRayDir);

	_uint	iNumPrimitive = pVIBuffer->Get_NumPrimitive();
	_uint indicesSize = pVIBuffer->Get_PrimitiveIndicesSize();
	const _float3* pVerticesPos = pVIBuffer->Get_VerticesPos();

	_uint iIndex = 0;
	if (indicesSize / 3 > 2)
	{
		_ulong* pIndices = (_ulong*)pVIBuffer->Get_Indices();

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			_float		fU, fV, fDist;
			if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				&vLocalRayPos,
				&vLocalRayDir,
				&fU, &fV, &fDist))
			{
				_vector vec = vLocalRayPos + (vLocalRayDir * fDist);
				XMStoreFloat3(pOut, vec);
				return true;

			}

		}
	}
	else
	{
		_ushort* pIndices = (_ushort*)pVIBuffer->Get_Indices();

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			_float		fU, fV, fDist;
			if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				&vLocalRayPos,
				&vLocalRayDir,
				&fU, &fV, &fDist))
			{
				_vector vec = vLocalRayPos + (vLocalRayDir * fDist);
				XMStoreFloat3(pOut, vec);
				return true;

			}

		}
	}

	return false;
}

_bool CPicking::Picking_InWorldSpace(CTransform * _pTransform, CCollider * pCollider, _float3 * pOut, _float* dist)
{
	_float fDist;
	_bool isPicking;
	_vector RayDir = XMVector3Normalize(XMLoadFloat3(&m_vRayDir));
	if (XMVector3IsNaN(RayDir))
		return false;
	isPicking = pCollider->PickingCollider(XMLoadFloat3(&m_vRayPos), XMLoadFloat3(&m_vRayDir), &fDist);
	_vector vec = XMLoadFloat3(&m_vRayPos) + (RayDir * fDist);
	XMStoreFloat3(pOut, vec);
	*dist = fDist;

	return isPicking;
}

_bool CPicking::Picking_InLocalSpaceWithDist(_fmatrix * _modelWorldMat, CVIBuffer * pVIBuffer, _float3 * pOut, _float * pDist)
{
	_matrix		WorldMatrixInv = XMMatrixInverse(nullptr, *_modelWorldMat);

	_vector			vLocalRayPos, vLocalRayDir;
	/* 로컬스페이스 */
	vLocalRayPos = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv);
	vLocalRayDir = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv);
	//XMStoreFloat3(&, 
	//D3DXVec3TransformNormal(&vLocalRayDir, &m_vRayDir, &WorldMatrixInv);
	vLocalRayDir = XMVector3Normalize(vLocalRayDir);
	//D3DXVec3Normalize(&vLocalRayDir, &vLocalRayDir);

	_uint	iNumPrimitive = pVIBuffer->Get_NumPrimitive();
	_uint indicesSize = pVIBuffer->Get_PrimitiveIndicesSize();
	const _float3* pVerticesPos = pVIBuffer->Get_VerticesPos();
	list<_float> listDist;
	_uint iIndex = 0;
	if (indicesSize / 3 > 2)
	{
		_ulong* pIndices = (_ulong*)pVIBuffer->Get_Indices();

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			_float		fU, fV, fDist;
			if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				&vLocalRayPos,
				&vLocalRayDir,
				&fU, &fV, &fDist))
			{
				listDist.push_back(fDist);

				//_vector vec = vLocalRayPos + (vLocalRayDir * fDist);
				//XMStoreFloat3(pOut, vec);
				//*pDist = fDist;
				//return true;

			}

		}
		if (listDist.size() <= 0)
			return false;

		listDist.sort();
		_vector vec = vLocalRayPos + (vLocalRayDir * listDist.front());
		XMStoreFloat3(pOut, vec);
		*pDist = listDist.front();
		return true;
	}
	else
	{
		_ushort* pIndices = (_ushort*)pVIBuffer->Get_Indices();

		for (_uint i = 0; i < iNumPrimitive; ++i)
		{
			_float		fU, fV, fDist;
			if (true == IntersectTri(XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				XMLoadFloat3(&pVerticesPos[pIndices[iIndex++]]),
				&vLocalRayPos,
				&vLocalRayDir,
				&fU, &fV, &fDist))
			{
				listDist.push_back(fDist);

				//_vector vec = vLocalRayPos + (vLocalRayDir * fDist);
				//XMStoreFloat3(pOut, vec);
				//*pDist = fDist;
				//return true;
			}

		}
		if (listDist.size() <= 0)
			return false;

		listDist.sort();
		_vector vec = vLocalRayPos + (vLocalRayDir * listDist.front());
		XMStoreFloat3(pOut, vec);
		*pDist = listDist.front();
		return true;
	}



	//RELEASE_INSTANCE(CGameInstance);

	return false;
}

_bool CPicking::IntersectTri(const _fvector& p0, const _fvector& p1, const _fvector& p2, const _gvector * pRayPos, const _hvector * pRayDir, _float * pU, _float * pV, _float * pDist)
{
	_vector e1 = p1 - p0;
	_vector e2 = p2 - p0;

	_vector pVec;

	pVec = XMVector3Cross(*pRayDir,e2);
	_float fDet = XMVectorGetX(XMVector3Dot(e1,pVec));

	_vector tVec;

	if (fDet > 0)
	{
		tVec = *pRayPos - p0;
	}
	else if (fDet < 0)
	{
		tVec = p0 - *pRayPos;
		fDet = -fDet;
	}

	if (fDet < 0.0001f)
		return false;

	*pU = XMVectorGetX(XMVector3Dot(tVec, pVec));

	if (*pU <0.0f || *pU > fDet)
		return false;

	_vector qVec;
	qVec = XMVector3Cross(tVec, e1);

	*pV = XMVectorGetY(XMVector3Dot(*pRayDir,qVec));

	if (*pV <0.0f || *pU + *pV > fDet)
		return false;

	*pDist = XMVectorGetX(XMVector3Dot(e2,qVec));

	if (*pDist < 0.f)
		return false;

	_float fInvDet = 1.f / fDet;

	*pDist *= fInvDet;
	*pU *= fInvDet;
	*pV *= fInvDet;

	return true;
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}

