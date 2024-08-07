#include "..\Public\VIBuffer.h"



CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_IBDesc(rhs.m_IBDesc)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_eTopology(rhs.m_eTopology)
	, m_VBSubresourceData(rhs.m_VBSubresourceData)
	, m_pVerticesPos(rhs.m_pVerticesPos)
	, m_IBSubresourceData(rhs.m_IBSubresourceData)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iPrimitiveIndicesSize(rhs.m_iPrimitiveIndicesSize)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_eFormat(rhs.m_eFormat)
	, m_pIndices(rhs.m_pIndices)
	, m_Passes(rhs.m_Passes)
	, m_pEffect(rhs.m_pEffect)	
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);

	for (auto& pPassDesc : m_Passes)
	{
		Safe_AddRef(pPassDesc.pInputLayout);
		Safe_AddRef(pPassDesc.pPass);
	}

	Safe_AddRef(m_pEffect);
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render(_uint iPassIndex)
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
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);	

	m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);
	
	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Set_RawValue(const char * pConstantName, void * pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);	
}

HRESULT CVIBuffer::Set_TextureValue(const char * pConstantName, ID3D11ShaderResourceView * pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetResource(pSRV);
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;	

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubresourceData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubresourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Compile_Shader(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint			iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
	
#endif // _DEBUG

	ID3DBlob*		pCodes = nullptr;	

	/* 셰이더파일을 컴파일하여 메모리에 올리고. */
	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pCodes, nullptr)))
		return E_FAIL;

	/* 올라온 메모리로부터 이펙트 객체를 생성한다. */
	if (FAILED(D3DX11CreateEffectFromMemory(pCodes->GetBufferPointer(), pCodes->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		PASSDESC			PassDesc;
		ZeroMemory(&PassDesc, sizeof(PASSDESC));

		PassDesc.pPass = pTechnique->GetPassByIndex(i);	

		D3DX11_PASS_DESC		Pass;
		ZeroMemory(&Pass, sizeof(D3DX11_PASS_DESC));

		PassDesc.pPass->GetDesc(&Pass);	

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, Pass.pIAInputSignature, Pass.IAInputSignatureSize, &PassDesc.pInputLayout)))
			return E_FAIL;

		m_Passes.push_back(PassDesc);
	}

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	for (auto& pPassDesc : m_Passes)
	{
		Safe_Release(pPassDesc.pInputLayout);
		Safe_Release(pPassDesc.pPass);
	}
	m_Passes.clear();

	Safe_Release(m_pEffect);
}
