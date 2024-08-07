#include "..\Public\VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
	
{

}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_VBInstSubresourceData(rhs.m_VBInstSubresourceData)
	, m_iInstStride(rhs.m_iInstStride)
	, m_iNumInstance(rhs.m_iNumInstance)	
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
{
	Safe_AddRef(m_pVBInstance);

}

HRESULT CVIBuffer_Instance::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext ||
		iPassIndex >= m_Passes.size())
		return E_FAIL;

	/* 한개의 정점버퍼를 쓰는경우에 사용.*/
	/* 다수의 정점버퍼를 쓰는경우(ex.인스턴싱) 각 자식클래스로 오버라이딩하여 렌더링할 수 있도록 한다. */

	ID3D11Buffer*		pBuffers[] = { m_pVB, m_pVBInstance };
	_uint				iStrides[] = { m_iStride, m_iInstStride };
	_uint				iOffsets[] = { 0, 0 };

	m_pDeviceContext->IASetVertexBuffers(0, 2, pBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);

	// m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);
	m_pDeviceContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Create_InstanceBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubresourceData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Instance::Free()
{
	__super::Free();	

	Safe_Release(m_pVBInstance);
}
