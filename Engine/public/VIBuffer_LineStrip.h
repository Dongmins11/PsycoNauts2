#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_LineStrip final : public CVIBuffer
{
protected:
	explicit CVIBuffer_LineStrip(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_LineStrip(const CVIBuffer_LineStrip& rhs);
	virtual ~CVIBuffer_LineStrip() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_float3* pPoints, _uint iNumVertices);
	virtual HRESULT NativeConstruct(void* pArg);
public:
	virtual HRESULT Render(_uint iPassIndex);
	
public:
	static CVIBuffer_LineStrip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* pPoints, _uint iNumVertices);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END