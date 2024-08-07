#include "..\Public\Navigation.h"
#include "Cell.h"
#include "PhysXScene_Manager.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_NaviDesc(rhs.m_NaviDesc)
	, m_vecPoints(rhs.m_vecPoints)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNaviDataFilePath)
{
	if (FAILED(Create_Cells(pNaviDataFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_NaviDesc, pArg, sizeof(NAVIDESC));

	return S_OK;
}

_float3 * CNavigation::Get_Points(_uint idx)
{
	return m_Cells[idx]->Get_Points();
}

void CNavigation::Update(_fmatrix TransformMatrix)
{
	for (auto& pCell : m_Cells)
		pCell->Update(TransformMatrix);
}

_bool CNavigation::Move_OnNavigation(_fvector vPosition)
{
	if (m_NaviDesc.iCurrentIndex >= m_Cells.size())
		return false;

	CCell*			pNeighbor = nullptr;

	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &pNeighbor))
	{
		return true;
	}
	else
	{
		if (nullptr != pNeighbor)
		{
			while (true)
			{
				if (true == pNeighbor->isIn(vPosition, &pNeighbor))
				{
					m_NaviDesc.iCurrentIndex = pNeighbor->Get_Index();
					break;
				}
			}
			return true;
		}
		else
		{
			/* 슬라이딩.*/

			return false;
		}
	}
}

_float CNavigation::OnNavigationGetY(_fvector vPosition, _fvector vCurPosition)
{
	CCell* pNeighbor = nullptr;
	_uint iColIndex = 0;
	if (m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &pNeighbor, &iColIndex))
	{
		_vector planeVect = XMPlaneFromPoints(m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A),
			m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B),
			m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C));

		//y = (-Ax -Cz - D)/y
		_float PosY = (-(XMVectorGetX(planeVect)*XMVectorGetX(vPosition)) - (XMVectorGetZ(planeVect)*XMVectorGetZ(vPosition)) - XMVectorGetW(planeVect)) / XMVectorGetY(planeVect);
		return PosY;
	}
}

_bool CNavigation::CheckingCCW(_fvector vPointA, _fvector vPointB, _fvector vPointC)
{
	_vector v1 = vPointB - vPointA;
	_vector v2 = vPointC - vPointA;

	_vector crossVec = XMVector3Cross(v1, v2);

	if (XMVectorGetY(crossVec) < 0)
	{
		return false;
	}

	return true;

}

_bool CNavigation::CheckingCCW(_float3 vPointA, _float3 vPointB, _float3 vPointC)
{
	_vector v1 = XMLoadFloat3(&vPointB) - XMLoadFloat3(&vPointA);
	_vector v2 = XMLoadFloat3(&vPointC) - XMLoadFloat3(&vPointA);

	_vector crossVec = XMVector3Cross(v1, v2);

	if (XMVectorGetY(crossVec) < 0)
	{
		return false;
	}
	return true;
}

HRESULT CNavigation::Create_Cells(const _tchar * pNaviDataFilePath)
{
	HANDLE			hFile = 0;
	_ulong	dwByte = 0;
	hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;
	

	NAVIMESHDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

	_float3			vPoints[CCell::POINT_END];
	_uint			iOption = 0;
	while (true)
	{
		ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		memcpy(NaviDesc.vPoint, vPoints, sizeof(_float3)*CCell::POINT_END);
		NaviDesc.iByOption = iOption;

		CCell*			pCell = CCell::Create(m_pDevice, m_pDeviceContext, NaviDesc, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);
	//if (FAILED(SetUp_Neighbor()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Create_NaviData(const _tchar * pNaviDataFilePath)
{
	HANDLE			hFile = 0;
	_ulong	dwByte = 0;
	hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;


	NAVIMESHDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

	_float3			vPoints[CCell::POINT_END];
	_uint			iOption = 0;
	while (true)
	{
		ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		memcpy(NaviDesc.vPoint, vPoints, sizeof(_float3)*3);
		NaviDesc.iByOption = iOption;

		m_vecPoints.emplace_back(vPoints);
	}

	CloseHandle(hFile);

	return S_OK;
}

void CNavigation::Clear_Cells()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
	//m_Cells.swap(vector<CCell*>());
}

void CNavigation::Clear_NaviData()
{
	m_vecPoints.clear();
}

#ifdef _DEBUG
HRESULT CNavigation::Render(_uint iPassIndex)
{
	for (auto& pCell : m_Cells)
		pCell->Render(iPassIndex);

	return S_OK;
}
#endif // _DEBUG

HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSour : m_Cells)
	{
		for (auto& pDest : m_Cells)
		{
			if (pSour == pDest)
				continue;

			if (pDest->Compare_Points(pSour->Get_Point(CCell::POINT_A), pSour->Get_Point(CCell::POINT_B)))
				pSour->Set_Neighbor(CCell::LINE_AB, pDest);
			if (pDest->Compare_Points(pSour->Get_Point(CCell::POINT_B), pSour->Get_Point(CCell::POINT_C)))
				pSour->Set_Neighbor(CCell::LINE_BC, pDest);
			if (pDest->Compare_Points(pSour->Get_Point(CCell::POINT_C), pSour->Get_Point(CCell::POINT_A)))
				pSour->Set_Neighbor(CCell::LINE_CA, pDest);
		}
	}
	return S_OK;
}


CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNaviDataFilePath)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pNaviDataFilePath)))
	{
		MSGBOX("Failed to Created CNavigation");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNavigation");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

}
