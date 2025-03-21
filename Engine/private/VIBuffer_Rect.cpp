#include "..\Public\VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Rect::NativeConstruct_Prototype( _float fSizeX, _float fSizeY)
{
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	m_pVerticesPos[0] = ((VTXTEX*)m_pVertices)[0].vPosition = _float3(-fSizeX/2.f, fSizeY/2.f, 0.f);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.f, 0.f);

	m_pVerticesPos[1] = ((VTXTEX*)m_pVertices)[1].vPosition = _float3(fSizeX / 2.f, fSizeY / 2.f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.f, 0.f);

	m_pVerticesPos[2] = ((VTXTEX*)m_pVertices)[2].vPosition = _float3(fSizeX / 2.f, -fSizeY / 2.f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.f, 1.f);

	m_pVerticesPos[3] = ((VTXTEX*)m_pVertices)[3].vPosition = _float3(-fSizeX / 2.f, -fSizeY / 2.f, 0.f);
	((VTXTEX*)m_pVertices)[3].vTexUV = _float2(0.f, 1.f);

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);



	m_iNumPrimitive = 2;
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

	((FACEINDICES16*)m_pIndices)[0]._1 = 0;
	((FACEINDICES16*)m_pIndices)[0]._2 = 1;
	((FACEINDICES16*)m_pIndices)[0]._3 = 2;

	((FACEINDICES16*)m_pIndices)[1]._1 = 0;
	((FACEINDICES16*)m_pIndices)[1]._2 = 2;
	((FACEINDICES16*)m_pIndices)[1]._3 = 3;

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	
/*
	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;	
	UINT InstanceDataStepRate;*/

	/* 현재 내가 정의한 정점 멤버들의 대해 기술. */
	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl"), Elements, 2)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CVIBuffer_Rect::NativeConstruct(void * pArg)
{
	return S_OK;
}

CVIBuffer_Rect * CVIBuffer_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float fSizeX, _float fSizeY)
{
	CVIBuffer_Rect*	pInstance = new CVIBuffer_Rect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(fSizeX,fSizeY)))
	{
		MSGBOX("Failed to Created CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect::Clone(void * pArg)
{
	CVIBuffer_Rect*	pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
