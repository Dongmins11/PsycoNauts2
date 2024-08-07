#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMSTATETYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	HRESULT Set_Transform(TRANSFORMSTATETYPE eState, _fmatrix StateMatrix) {
		XMStoreFloat4x4(&m_StateMatrix[eState], StateMatrix);
		return S_OK;
	}
	
	_matrix Get_Transform(TRANSFORMSTATETYPE eState) {
		return XMLoadFloat4x4(&m_StateMatrix[eState]);
	}

	_vector Get_CamPosition() {
		return XMLoadFloat4(&_float4(m_vCamPosition, 1.f));
	}

public:
	void Tick();


private:
	_float4x4			m_StateMatrix[D3DTS_END];
	
private:
	_float3				m_vCamPosition;

public:
	virtual void Free() override;
};

END