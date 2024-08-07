#pragma once


#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
public:

protected:
	explicit CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	virtual HRESULT Render(_uint iPassIndex);

public:
	const _uint Get_NumPrimitive() const { return m_iNumPrimitive; }
	const _uint Get_PrimitiveIndicesSize() const { return m_iPrimitiveIndicesSize; }

	const void* Get_Indices() const { return m_pIndices; }
	const _float3* Get_VerticesPos() const { return m_pVerticesPos; }

public:
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT Set_TextureValue(const char* pConstantName, ID3D11ShaderResourceView* pSRV);

protected:
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

protected:
	ID3D11Buffer*			m_pVB = nullptr;
	D3D11_BUFFER_DESC		m_VBDesc;
	D3D11_SUBRESOURCE_DATA	m_VBSubresourceData;
	_uint					m_iStride = 0;
	_uint					m_iNumVertices = 0;
	void*					m_pVertices = nullptr;

protected:
	_float3*				m_pVerticesPos = nullptr;

protected:
	ID3D11Buffer*			m_pIB = nullptr;
	D3D11_BUFFER_DESC		m_IBDesc;
	D3D11_SUBRESOURCE_DATA	m_IBSubresourceData;
	_uint					m_iNumPrimitive = 0;
	_uint					m_iPrimitiveIndicesSize = 0;
	_uint					m_iNumIndicesPerPrimitive = 0;
	DXGI_FORMAT				m_eFormat;
	void*					m_pIndices = nullptr;

protected:
	vector<PASSDESC>		m_Passes;
	ID3DX11Effect*			m_pEffect = nullptr;

public:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();
	HRESULT Compile_Shader(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);

public:	
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
