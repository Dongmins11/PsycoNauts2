#include "..\public\VIBuffer_Trail.h"


CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeivceContext)
	:CVIBuffer(pDevice, pDeivceContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		m_iNumRects = *(_uint*)pArg;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXTEX);
	//사각형 8개이므로(정점9줄x2)
	m_iNumVertices = (m_iNumRects + 1) * 2;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		m_pVerticesPos[i] = ((VTXTEX*)m_pVertices)[i].vPosition = _float3(0.f, 0.f, 0.f);
		((VTXTEX*)m_pVertices)[i].vTexUV = _float2(0.f, 0.f);
	}

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	//사각형 의 개수 x 2(삼각형의개수)
	m_iNumPrimitive = m_iNumRects * 2;
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


	_uint		iNumFace = 0;

	// 사각형의 개수만큼 돌면서 인덱스 추가
	for (_uint i = 0; i < m_iNumRects; ++i)
	{
		((FACEINDICES16*)m_pIndices)[iNumFace]._1 = i + 0;
		((FACEINDICES16*)m_pIndices)[iNumFace]._2 = i + 2;
		((FACEINDICES16*)m_pIndices)[iNumFace]._3 = i + 3;
		++iNumFace;

		((FACEINDICES16*)m_pIndices)[iNumFace]._1 = i + 0;
		((FACEINDICES16*)m_pIndices)[iNumFace]._2 = i + 3;
		((FACEINDICES16*)m_pIndices)[iNumFace]._3 = i + 1;
		++iNumFace;

	}
	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), Elements, 2)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Trail::Update_SwordTrail(_double TimeDelta, _fvector vUpper, _fvector vLower, _float fWeight)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	if (m_isFirst)
	{
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iVTXCut].vPosition, vUpper);
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iVTXCut + 1].vPosition, vLower);

		m_iVTXCut += 2;

		if (m_iVTXCut > m_iNumVertices)
			m_isFirst = false;

		for (_uint i = 0; i < m_iNumVertices; i += 2)
	
		{
			((VTXTEX*)SubResource.pData)[i].vTexUV = _float2((_float)(i * 2.f) / ((_float)m_iNumVertices - 1), 1.f);
			((VTXTEX*)SubResource.pData)[i + 1].vTexUV = _float2((_float)(i * 2.f) / ((_float)m_iNumVertices - 1), 0.f);
		}

		_uint iRemoveCount = m_iVTXCut / (m_iNumRects + 2);
		m_iVTXCut -= iRemoveCount;

		for (_uint i = 0; i < m_iNumVertices; i += 2)
		{
			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[i].vPosition, vUpper + vUpper * ( i / m_iNumVertices));
			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[i + 1].vPosition, vLower + vLower * (i / m_iNumVertices));
			//((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i].vPosition;
			//((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i + 1].vPosition;
		}
	}
	else
	{
		_uint iNumVertices = m_iNumVertices;
		for (_uint i = iNumVertices; i > 0; i -= 2)
		{
			if (2 <= i)
			{
				m_iCatmullRomIndex[2] = (i - 2);
				m_iCatmullRomIndex[3] = i;
			}
			else
			{
				m_iCatmullRomIndex[2] = i;
				m_iCatmullRomIndex[3] = 0;
			}

			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[1]].vPosition,
				XMVectorCatmullRom(XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[0]].vPosition)
					, XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[1]].vPosition)
					, XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[2]].vPosition)
					, XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[3]].vPosition)
					, fWeight));

			for (_uint j = 0; j < iNumVertices - 2; j += 2)
			{
				((VTXTEX*)SubResource.pData)[j + 3].vPosition = ((VTXTEX*)SubResource.pData)[j + 1].vPosition;
			}
			m_iCatmullRomIndex[0] = m_iCatmullRomIndex[1];
			m_iCatmullRomIndex[1] = m_iCatmullRomIndex[2];

			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[0].vPosition, vUpper);
			XMStoreFloat3(&((VTXTEX*)SubResource.pData)[1].vPosition, vLower);
		}
	}
	m_pDeviceContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Trail::Update_SetPosition(_double TimeDelta, _fvector vStartUpper, _fvector vStartLower, _fvector vEndUpper, _fvector vEndLower, _float fWeight)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_vector vUpper = vStartUpper - vEndUpper;
	_vector vLower = vStartLower - vEndLower;

	if (m_iVTXCut > m_iNumVertices)
		m_isFirst = false;

	for (_uint i = 0; i < m_iNumVertices; i += 2)
	{
		((VTXTEX*)SubResource.pData)[i].vTexUV = _float2((_float)(i * 2.f) / ((_float)m_iNumVertices - 1), 1.f);
		((VTXTEX*)SubResource.pData)[i + 1].vTexUV = _float2((_float)(i * 2.f) / ((_float)m_iNumVertices - 1), 0.f);
	}

	for (_uint i = 0; i < m_iNumVertices; i += 2)
	{
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[i].vPosition, vStartUpper - vUpper * (i / m_iNumVertices));
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[i + 1].vPosition, vStartLower - vLower * (i / m_iNumVertices));
	}

	m_pDeviceContext->Unmap(m_pVB, 0);
}

CVIBuffer_Trail * CVIBuffer_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pIndices);
}
