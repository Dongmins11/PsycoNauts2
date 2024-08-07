#include "..\public\VIBuffer_Triangle.h"


CVIBuffer_Triangle::CVIBuffer_Triangle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Triangle::CVIBuffer_Triangle(const CVIBuffer_Triangle & rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Triangle::NativeConstruct_Prototype(_float3 * pPoints)
{
	for (_uint i = 0; i < 3; ++i)
		m_NewPos[i] = pPoints[i];

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVertices = 3;
	m_iStride = sizeof(VTXTEX);

	m_VBDesc.ByteWidth = m_iStride*m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	m_pVertices = new VTXTEX[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	m_pVerticesPos[0] = ((VTXTEX*)m_pVertices)[0].vPosition = pPoints[0];
	m_pVerticesPos[1] = ((VTXTEX*)m_pVertices)[1].vPosition = pPoints[1];
	m_pVerticesPos[2] = ((VTXTEX*)m_pVertices)[2].vPosition = pPoints[2];

	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.f, 1.f);

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	//Safe_Delete_Array(m_pVertices);

	/*For.IndexBuffer*/
	_ulong dwIndex = 0;

	m_iNumIndicesPerPrimitive = 3;
	m_iPrimitiveIndicesSize = sizeof(FACEINDICES16);
	m_iNumPrimitive = 1;
	m_eFormat = DXGI_FORMAT_R16_UINT;

	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize* m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pIndices = new FACEINDICES16[m_iNumPrimitive];
	m_pFaceIndices = new _ushort[m_iNumPrimitive * 3];

	((_ushort*)m_pFaceIndices)[dwIndex++] = ((FACEINDICES16*)m_pIndices)[0]._1 = 0;
	((_ushort*)m_pFaceIndices)[dwIndex++] = ((FACEINDICES16*)m_pIndices)[0]._2 = 1;
	((_ushort*)m_pFaceIndices)[dwIndex++] = ((FACEINDICES16*)m_pIndices)[0]._3 = 2;

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC Element[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	if (FAILED(Compile_Shader(TEXT("../bin/ShaderFiles/Shader_Triangle.hlsl"), Element, 2)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Triangle::NativeConstruct(void * pArg)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	return S_OK;
}

HRESULT CVIBuffer_Triangle::Compute_WorldMatrix(_float3 vScale, _float3 vRads, _float3 vPos)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(vScale.x, vScale.y, vScale.z)
		* XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), vRads.x)*XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRads.y)
		*XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), vRads.z)
		*XMMatrixTranslation(vPos.x, vPos.y, vPos.z));

	return S_OK;
}

void CVIBuffer_Triangle::UpdateSub(_uint iIndex, _float3 vPos)
{
	if (iIndex >= m_iNumVertices)
		return;

	((VTXTEX*)m_pVertices)[iIndex].vPosition = m_NewPos[iIndex] = vPos;
	//m_pVerticesPos[iIndex] = m_NewPos[iIndex] = vPos;
	m_pDeviceContext->UpdateSubresource(m_pVB, 0, nullptr, m_pVertices, 0, 0);
}

CVIBuffer_Triangle * CVIBuffer_Triangle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float3 * pPoints)
{
	CVIBuffer_Triangle* pInstance = new CVIBuffer_Triangle(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pPoints)))
	{
		MSGBOX("Failed To Create CVIBuffer_Triangle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Triangle::Clone(void * pArg)
{
	CVIBuffer_Triangle* pInstance = new CVIBuffer_Triangle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed To Create CVIBuffer_Triangle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Triangle::Free()
{
	__super::Free();
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pFaceIndices);
	}
}
