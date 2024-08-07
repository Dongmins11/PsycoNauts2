#include "..\Public\MeshContainer.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)

{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex) 
{
	strcpy_s(m_szName, rhs.m_szName);
	m_pVertices = rhs.m_pVertices;
	m_isCloned = true;
}

HRESULT CMeshContainer::NativeConstruct_Prototype(_uint iMaterialIndex, VTXMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint _iNumVertices, _uint _iNumFaceIndices, _float3* _pVerticesPos, char* szMeshContainerName)
{
	m_iMaterialIndex = iMaterialIndex;
	m_iNumVertices = _iNumVertices;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXMODEL);
	strcpy_s(m_szName, szMeshContainerName);
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = pVTXModelVertices;
	m_pVerticesPos = _pVerticesPos;

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = _iNumFaceIndices;
	m_iNumIndicesPerPrimitive = 3;
	m_iPrimitiveIndicesSize = sizeof(FACEINDICES32);
	m_eFormat = DXGI_FORMAT_R32_UINT;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pIndices = _pIndices;

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void* pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 한개의 정점버퍼를 쓰는경우에 사용.*/
	/* 다수의 정점버퍼를 쓰는경우(ex.인스턴싱) 각 자식클래스로 오버라이딩하여 렌더링할 수 있도록 한다. */

	ID3D11Buffer*		pBuffers[] = { m_pVB };
	_uint				iStrides[] = { m_iStride };
	_uint				iOffsets[] = { 0 };

	m_pDeviceContext->IASetVertexBuffers(0, 1, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);	
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VTXMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint iMaterialIndex, _uint _iNumVertices, _uint _iNumFaceIndices, _float3* _pVerticesPos,  char* szMeshContainerName)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iMaterialIndex,pVTXModelVertices,_pIndices,_iNumVertices, _iNumFaceIndices, _pVerticesPos, szMeshContainerName)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	if (!m_isCloned)
	{
		Safe_Delete_Array(m_pVertices);
	}
}
