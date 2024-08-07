#include "..\Public\VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Cube::NativeConstruct_Prototype()
{
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXCUBETEX[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	m_pVerticesPos[0] = ((VTXCUBETEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[0].vTexUV = ((VTXCUBETEX*)m_pVertices)[0].vPosition;

	m_pVerticesPos[1] = ((VTXCUBETEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[1].vTexUV = ((VTXCUBETEX*)m_pVertices)[1].vPosition;

	m_pVerticesPos[2] = ((VTXCUBETEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[2].vTexUV = ((VTXCUBETEX*)m_pVertices)[2].vPosition;

	m_pVerticesPos[3] = ((VTXCUBETEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[3].vTexUV = ((VTXCUBETEX*)m_pVertices)[3].vPosition;

	m_pVerticesPos[4] = ((VTXCUBETEX*)m_pVertices)[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[4].vTexUV = ((VTXCUBETEX*)m_pVertices)[4].vPosition;

	m_pVerticesPos[5] = ((VTXCUBETEX*)m_pVertices)[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[5].vTexUV = ((VTXCUBETEX*)m_pVertices)[5].vPosition;

	m_pVerticesPos[6] = ((VTXCUBETEX*)m_pVertices)[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[6].vTexUV = ((VTXCUBETEX*)m_pVertices)[6].vPosition;

	m_pVerticesPos[7] = ((VTXCUBETEX*)m_pVertices)[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[7].vTexUV = ((VTXCUBETEX*)m_pVertices)[7].vPosition;

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);



	m_iNumPrimitive = 12;
	m_iNumIndicesPerPrimitive = 3;
	m_iPrimitiveIndicesSize = sizeof(FACEINDICES16);	
	m_eFormat = DXGI_FORMAT_R16_UINT;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pIndices = new FACEINDICES16[m_iNumPrimitive];
	FACEINDICES16*	pIndices = (FACEINDICES16*)m_pIndices;
	
	/* +x */
	pIndices[0]._1 = 1; pIndices[0]._2 = 5; pIndices[0]._3 = 6;
	pIndices[1]._1 = 1; pIndices[1]._2 = 6; pIndices[1]._3 = 2;

	/* -x */
	pIndices[2]._1 = 4; pIndices[2]._2 = 0; pIndices[2]._3 = 3;
	pIndices[3]._1 = 4; pIndices[3]._2 = 3; pIndices[3]._3 = 7;

	/* +y */
	pIndices[4]._1 = 4; pIndices[4]._2 = 5; pIndices[4]._3 = 1;
	pIndices[5]._1 = 4; pIndices[5]._2 = 1; pIndices[5]._3 = 0;

	/* -y */
	pIndices[6]._1 = 3; pIndices[6]._2 = 2; pIndices[6]._3 = 6;
	pIndices[7]._1 = 3; pIndices[7]._2 = 6; pIndices[7]._3 = 7;

	/* +z */
	pIndices[8]._1 = 5; pIndices[8]._2 = 4; pIndices[8]._3 = 7;
	pIndices[9]._1 = 5; pIndices[9]._2 = 7; pIndices[9]._3 = 6;

	/* -z */
	pIndices[10]._1 = 0; pIndices[10]._2 = 1; pIndices[10]._3 = 2;
	pIndices[11]._1 = 0; pIndices[11]._2 = 2; pIndices[11]._3 = 3;

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;	

	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl"), Elements, 2)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CVIBuffer_Cube::NativeConstruct(void * pArg)
{
	return S_OK;
}

CVIBuffer_Cube * CVIBuffer_Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Cube*	pInstance = new CVIBuffer_Cube(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Cube::Clone(void * pArg)
{
	CVIBuffer_Cube*	pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
