#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_EffectRect final : public CVIBuffer
{
protected:
	explicit CVIBuffer_EffectRect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_EffectRect(const CVIBuffer_EffectRect& rhs);
	virtual ~CVIBuffer_EffectRect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	static CVIBuffer_EffectRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END