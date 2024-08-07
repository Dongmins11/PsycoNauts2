#include "..\Public\QuadTree.h" 
#include "Frustum.h"
#include "PipeLine.h"

CQuadTree::CQuadTree()
{

}

HRESULT CQuadTree::NativeConstruct(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_iCorner[CORNER_LT] = iNumVerticesX * iNumVerticesZ - iNumVerticesX;
	m_iCorner[CORNER_RT] = iNumVerticesX * iNumVerticesZ - 1;
	m_iCorner[CORNER_RB] = iNumVerticesX - 1;
	m_iCorner[CORNER_LB] = 0;

	m_iCenter = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_LB]) >> 1;

	_uint			iTC, iRC, iBC, iLC;

	iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
	iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
	iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;
	iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;

	m_pChild[CHILD_LT] = CQuadTree::Create(m_iCorner[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, m_iCorner[CORNER_RT], iRC, m_iCenter);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorner[CORNER_RB], iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorner[CORNER_LB]);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuadTree::NativeConstruct(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorner[CORNER_LT] = iLT;
	m_iCorner[CORNER_RT] = iRT;
	m_iCorner[CORNER_RB] = iRB;
	m_iCorner[CORNER_LB] = iLB;

	if (m_iCorner[CORNER_RT] - m_iCorner[CORNER_LT] == 1)
		return S_OK;

	m_iCenter = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_LB]) >> 1;

	_uint			iTC, iRC, iBC, iLC;

	iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
	iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
	iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;
	iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;

	m_pChild[CHILD_LT] = CQuadTree::Create(m_iCorner[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, m_iCorner[CORNER_RT], iRC, m_iCenter);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorner[CORNER_RB], iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorner[CORNER_LB]);

	return S_OK;
}

HRESULT CQuadTree::Culling(CFrustum * pFrustum, _float3 * pVerticesPos, FACEINDICES32 * pIndices, _uint * pNumFaces)
{
	if (nullptr == m_pChild[CHILD_LT] ||
		true == SceneofDetail(pVerticesPos))
	{
		_bool		isIn[CORNER_END] = {
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]])),
		};

		if (nullptr == m_pChild[CHILD_LT])
		{
			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RT] ||
				true == isIn[CORNER_RB])
			{
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RT];
				pIndices[*pNumFaces]._3 = m_iCorner[CORNER_RB];
				++(*pNumFaces);
			}


			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RB] ||
				true == isIn[CORNER_LB])
			{
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
				pIndices[*pNumFaces]._3 = m_iCorner[CORNER_LB];
				++(*pNumFaces);
			}

			return S_OK;
		}

		_bool		isDraw[NEIGHBOR_END] =
		{
			nullptr == m_pNeighbor[NEIGHBOR_LEFT] ? true : m_pNeighbor[NEIGHBOR_LEFT]->SceneofDetail(pVerticesPos),
			nullptr == m_pNeighbor[NEIGHBOR_TOP] ? true : m_pNeighbor[NEIGHBOR_TOP]->SceneofDetail(pVerticesPos),
			nullptr == m_pNeighbor[NEIGHBOR_RIGHT] ? true : m_pNeighbor[NEIGHBOR_RIGHT]->SceneofDetail(pVerticesPos),
			nullptr == m_pNeighbor[NEIGHBOR_BOTTOM] ? true : m_pNeighbor[NEIGHBOR_BOTTOM]->SceneofDetail(pVerticesPos),
		};

		if ((true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM]))
		{

			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RT] ||
				true == isIn[CORNER_RB])
			{
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RT];
				pIndices[*pNumFaces]._3 = m_iCorner[CORNER_RB];
				++(*pNumFaces);
			}


			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RB] ||
				true == isIn[CORNER_LB])
			{
				pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
				pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
				pIndices[*pNumFaces]._3 = m_iCorner[CORNER_LB];
				++(*pNumFaces);
			}
			return S_OK;
		}

		else
		{
			_uint		iTC, iRC, iBC, iLC;

			iTC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_RT]) >> 1;
			iRC = (m_iCorner[CORNER_RT] + m_iCorner[CORNER_RB]) >> 1;
			iBC = (m_iCorner[CORNER_LB] + m_iCorner[CORNER_RB]) >> 1;
			iLC = (m_iCorner[CORNER_LT] + m_iCorner[CORNER_LB]) >> 1;

			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RB] ||
				true == isIn[CORNER_LB])
			{
				if (false == isDraw[NEIGHBOR_LEFT])
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
					pIndices[*pNumFaces]._2 = m_iCenter;
					pIndices[*pNumFaces]._3 = iLC;
					++(*pNumFaces);

					pIndices[*pNumFaces]._1 = iLC;
					pIndices[*pNumFaces]._2 = m_iCenter;
					pIndices[*pNumFaces]._3 = m_iCorner[CORNER_LB];
					++(*pNumFaces);
					
				}
				else
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
					pIndices[*pNumFaces]._2 = m_iCenter;
					pIndices[*pNumFaces]._3 = m_iCorner[CORNER_LB];
					++(*pNumFaces);
				}

				if (false == isDraw[NEIGHBOR_BOTTOM])
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LB];
					pIndices[*pNumFaces]._2 = m_iCenter;
					pIndices[*pNumFaces]._3 = iBC;
					++(*pNumFaces);

					pIndices[*pNumFaces]._1 = iBC;
					pIndices[*pNumFaces]._2 = m_iCenter;
					pIndices[*pNumFaces]._3 = m_iCorner[CORNER_RB];
					++(*pNumFaces);

				}
				else
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LB];
					pIndices[*pNumFaces]._2 = m_iCenter;
					pIndices[*pNumFaces]._3 = m_iCorner[CORNER_RB];
					++(*pNumFaces);
				}
			}

			if (true == isIn[CORNER_LT] ||
				true == isIn[CORNER_RT] ||
				true == isIn[CORNER_RB])
			{

				if (false == isDraw[NEIGHBOR_TOP])
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
					pIndices[*pNumFaces]._2 = iTC;
					pIndices[*pNumFaces]._3 = m_iCenter;
					++(*pNumFaces);

					pIndices[*pNumFaces]._1 = m_iCenter;
					pIndices[*pNumFaces]._2 = iTC;
					pIndices[*pNumFaces]._3 = m_iCorner[CORNER_RT];
					++(*pNumFaces);
				}
				else
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_LT];
					pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RT];
					pIndices[*pNumFaces]._3 = m_iCenter;
					++(*pNumFaces);
				}

				if (false == isDraw[NEIGHBOR_RIGHT])
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RT];
					pIndices[*pNumFaces]._2 = iRC;
					pIndices[*pNumFaces]._3 = m_iCenter;
					++(*pNumFaces);

					pIndices[*pNumFaces]._1 = m_iCenter;
					pIndices[*pNumFaces]._2 = iRC;
					pIndices[*pNumFaces]._3 = m_iCorner[CORNER_RB];
					++(*pNumFaces);
				}
				else
				{
					pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RT];
					pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
					pIndices[*pNumFaces]._3 = m_iCenter;
					++(*pNumFaces);
				}
			}
		}


		return S_OK;
	}

	_float		fRange = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]]) -
		XMLoadFloat3(&pVerticesPos[m_iCenter])));

	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRange))
	{
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumFaces);
		}		
	}

	return S_OK;
}

HRESULT CQuadTree::SetUp_Neighbor()
{
	if (nullptr == m_pChild[CHILD_LT])
		return S_OK;

	

	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	for (auto& pChild : m_pChild)
	{
		pChild->SetUp_Neighbor();
	}

	return S_OK;
}

_bool CQuadTree::SceneofDetail(_float3 * pVerticesPos)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);
	
	_vector	vCamPosition = pPipeLine->Get_CamPosition();

	_float fCamDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat3(&pVerticesPos[m_iCenter])));
	_float fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])));

	if (fWidth < fCamDistance * 0.1f)
	{
		RELEASE_INSTANCE(CPipeLine);
		return true;
	}

	RELEASE_INSTANCE(CPipeLine);

	return false;
}

CQuadTree * CQuadTree::Create(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	CQuadTree*	pInstance = new CQuadTree();

	if (FAILED(pInstance->NativeConstruct(iNumVerticesX, iNumVerticesZ)))
	{
		MSGBOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*	pInstance = new CQuadTree();

	if (FAILED(pInstance->NativeConstruct(iLT, iRT, iRB, iLB)))
	{
		MSGBOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}
	

void CQuadTree::Free()
{


	for (auto& pChild : m_pChild)
		Safe_Release(pChild);		 	
}
