#include "..\Public\VIBuffer_LineStrip.h"

CVIBuffer_LineStrip::CVIBuffer_LineStrip(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_LineStrip::CVIBuffer_LineStrip(const CVIBuffer_LineStrip & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_LineStrip::NativeConstruct_Prototype(_float3* pPoints, _uint iNumVertices)
{
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_iStride = sizeof(VTXCOL);
	m_iNumVertices = iNumVertices;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXCOL[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	memcpy(m_pVerticesPos, pPoints, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		((VTXCOL*)m_pVertices)[i].vPosition = pPoints[i];
		((VTXCOL*)m_pVertices)[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);
	
	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_Line.hlsl"), Elements, 2)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CVIBuffer_LineStrip::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_LineStrip::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext ||
		iPassIndex >= m_Passes.size())
		return E_FAIL;

	/* 한개의 정점버퍼를 쓰는경우에 사용.*/
	/* 다수의 정점버퍼를 쓰는경우(ex.인스턴싱) 각 자식클래스로 오버라이딩하여 렌더링할 수 있도록 한다. */

	ID3D11Buffer*		pBuffers[] = { m_pVB };
	_uint				iStrides[] = { m_iStride };
	_uint				iOffsets[] = { 0 };

	m_pDeviceContext->IASetVertexBuffers(0, 1, pBuffers, iStrides, iOffsets);	
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);

	m_pDeviceContext->Draw(m_iNumVertices, 0);

	return S_OK;
}

CVIBuffer_LineStrip * CVIBuffer_LineStrip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float3* pPoints, _uint iNumVertices)
{
	CVIBuffer_LineStrip*	pInstance = new CVIBuffer_LineStrip(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pPoints, iNumVertices)))
	{
		MSGBOX("Failed to Created CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_LineStrip::Clone(void * pArg)
{
	CVIBuffer_LineStrip*	pInstance = new CVIBuffer_LineStrip(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_LineStrip::Free()
{
	__super::Free();
}
