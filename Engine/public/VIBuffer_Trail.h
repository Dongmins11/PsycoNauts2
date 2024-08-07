#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Trail final:	public CVIBuffer
{
protected:
	explicit CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeivceContext);
	explicit CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void Update_SwordTrail(_double TimeDelta, _fvector vUpper, _fvector vLower, _float fWeight = 0.5f);
	void Update_SetPosition(_double TimeDelta, _fvector vStartUpper, _fvector vStartLower, _fvector vEndUpper, _fvector vEndLower, _float fWeight = 0.5f);
	
private:
	_bool		m_isFirst = true;

	_uint		m_iNumRects = 40;
	_uint		m_iVTXCut = 0;
	_uint		m_iCatmullRomIndex[4] = { 0,0,0,0 };
	
public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
