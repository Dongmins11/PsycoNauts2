#include "..\Public\AnimMeshContainer.h"
#include "HierarchyNode.h"

CAnimMeshContainer::CAnimMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CAnimMeshContainer::CAnimMeshContainer(const CAnimMeshContainer & rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_BoneNodes(rhs.m_BoneNodes)	
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pBoneNode : m_BoneNodes)
		Safe_AddRef(pBoneNode);
}

HRESULT CAnimMeshContainer::SetUp_MeshBone(CModel * pModel,CHierarchyNode * pNode)
{
	if (nullptr == pNode)
	{
		CHierarchyNode*	pNode = pModel->Find_HierarchyNodes(m_szName);
		if (nullptr == pNode)
			return E_FAIL;

		m_BoneNodes.push_back(pNode);

		m_iNumBones = 1;

		Safe_AddRef(pNode);

		return S_OK;
	}
	else
	{
		m_BoneNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}
	return S_OK;
}

void CAnimMeshContainer::Get_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{ 
	_uint		iIndex = 0;

	for (auto& pHierarchyNode : m_BoneNodes)
	{
		_matrix OffsetMatrix = pHierarchyNode->Get_OffsetMatrix();
		_matrix CombinedTranformationMatrix = pHierarchyNode->Get_CombinedTransformationMatrix();

		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(OffsetMatrix * CombinedTranformationMatrix* PivotMatrix));
	}
}

HRESULT CAnimMeshContainer::NativeConstruct_Prototype(VTXANIMMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint _iNumVertices, _uint _iNumFaceIndices, _uint iMaterialIndex, _uint iNumBone, char* _szName)
{
	m_iMaterialIndex = iMaterialIndex;
	strcpy_s(m_szName, _szName);

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXANIMMODEL);
	m_iNumVertices = _iNumVertices;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = pVTXModelVertices;

	m_iNumBones = iNumBone;

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (__super::Create_VertexBuffer())
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);


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

HRESULT CAnimMeshContainer::NativeConstruct(void* pArg)
{
	return S_OK;
}

HRESULT CAnimMeshContainer::Render(_uint iPassIndex)
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

CAnimMeshContainer * CAnimMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, VTXANIMMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint _iNumVertices, _uint _iNumFaceIndices, _uint iMaterialIndex, _uint iNumBone, char* _szName)
{
	CAnimMeshContainer*	pInstance = new CAnimMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pVTXModelVertices, _pIndices, _iNumVertices, _iNumFaceIndices, iMaterialIndex, iNumBone, _szName)))
	{
		MSGBOX("Failed to Created CAnimMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CAnimMeshContainer::Clone(void * pArg)
{
	CAnimMeshContainer*	pInstance = new CAnimMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CAnimMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_BoneNodes)
		Safe_Release(pHierarchyNode);

	m_BoneNodes.clear();	
}
