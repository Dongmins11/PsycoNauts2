#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Triangle final : public CVIBuffer
{
protected:
	explicit CVIBuffer_Triangle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Triangle(const CVIBuffer_Triangle& rhs);
	virtual ~CVIBuffer_Triangle() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(_float3* pPoints);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Compute_WorldMatrix(_float3 vScale, _float3 vRads, _float3 vPos);
	void UpdateSub(_uint iIndex, _float3 vPos);

public:
	void	Set_Color(_float4 vColor) { m_vColor = vColor; }

public:
	static CVIBuffer_Triangle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* pPoints);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

private:
	_float3		m_NewPos[3];
	_float3		m_vRad;
	_float4		m_vColor;
	_float4x4	m_WorldMatrix;
	void*		m_pFaceIndices = nullptr;
};

END