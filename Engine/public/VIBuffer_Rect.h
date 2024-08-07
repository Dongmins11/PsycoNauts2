#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
protected:
	explicit CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_float fSizeX = 1.f, _float fSizeY = 1.f);
	virtual HRESULT NativeConstruct(void* pArg);
	
public:
	static CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,_float fSizeX=1.f, _float fSizeY=1.f);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END