#include "..\Public\VIBuffer_Terrain.h"
#include "Frustum.h"
#include "Transform.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);

}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* pHeightunordered_mapPath)
{
	HANDLE		hFile = CreateFile(pHeightunordered_mapPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong		dwByte = 0;

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*			pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ZeroMemory(pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight);

	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);


	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXNORTEX[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = ((VTXNORTEX*)m_pVertices)[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.0f, i);
			((VTXNORTEX*)m_pVertices)[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			((VTXNORTEX*)m_pVertices)[iIndex].vTexUV = _float2(j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2; 
	m_iNumIndicesPerPrimitive = 3;
	m_iPrimitiveIndicesSize = sizeof(FACEINDICES32);	
	m_eFormat = DXGI_FORMAT_R32_UINT;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];

	_uint		iNumFace = 0;

	VTXNORTEX*	pVertices = (VTXNORTEX*)m_pVertices;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vNormal, vSour, vDest;
			

			((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
			((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[1];
			((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition) - XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vPosition);
			vDest = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vPosition) - XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition);
			vNormal = XMVector3Cross(vSour, vDest);
						
			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal, 
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal) + vNormal));

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal, 
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal) + vNormal));

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal, 
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal) + vNormal));

			++iNumFace;

			((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
			((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[2];
			((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition) - XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vPosition);
			vDest = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vPosition) - XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition);
			vNormal = XMVector3Cross(vSour, vDest);

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal) + vNormal));

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal) + vNormal));

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal) + vNormal));

			++iNumFace;
		}
	}

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), Elements, 3)))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	CloseHandle(hFile);

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX, m_iNumVerticesZ);
	if (nullptr == m_pQuadTree)
		return E_FAIL;
	
	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Culling(CTransform* pTransform)
{
	CFrustum*		pFrustum = GET_INSTANCE(CFrustum);

	pFrustum->Transform_ToLocalSpace(pTransform->Get_WorldMatrixInverse());

	FACEINDICES32*		pIndices = (FACEINDICES32*)m_pIndices;

	_uint	iNumFaces = 0;

	if (FAILED(m_pQuadTree->Culling(pFrustum, m_pVerticesPos, pIndices, &iNumFaces)))
		return E_FAIL;

	/*for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool isIn[] = {
				pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])), 
				pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
				pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
				pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]))
			};

			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				pIndices[iNumFaces]._1 = iIndices[0];
				pIndices[iNumFaces]._2 = iIndices[1];
				pIndices[iNumFaces]._3 = iIndices[2];
				++iNumFaces;
			}

			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				pIndices[iNumFaces]._1 = iIndices[0];
				pIndices[iNumFaces]._2 = iIndices[2];
				pIndices[iNumFaces]._3 = iIndices[3];
				++iNumFaces;
			}
		}
	}*/

	m_iNumPrimitive = iNumFaces;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	m_pDeviceContext->Unmap(m_pIB, 0);

	RELEASE_INSTANCE(CFrustum);

	return S_OK;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pHeightunordered_mapPath)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pHeightunordered_mapPath)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();



	Safe_Release(m_pQuadTree);

	int a = 10;


}
