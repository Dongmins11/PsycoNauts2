#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vOriginalPoints[ePoint]);
	}

	_float3* Get_Points()
	{
		return (_float3*)&m_vOriginalPoints;
	}

	_uint Get_Index() {
		return m_iIndex;
	}

	_uint* Get_Option_index() { return& m_iOption; }

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_pNeighbor[eLine] = pNeighbor;
	}

	void Set_Pos(POINT ePoint, _float3 vNewPos) { XMStoreFloat3(&m_vOriginalPoints[ePoint], XMLoadFloat3(&vNewPos)); }
	
	void Set_Option(_uint iOption) { m_iOption = iOption; }

public:
	HRESULT NativeConstruct(NAVIMESHDESC& NaviDesc, _uint iIndex);
	HRESULT Change_Pos(POINT ePoint, _float3 vNewPos);

public:
	_bool Compare_Points(_fvector	vSourPoint, _fvector vDestPoint);
	void Update(_fmatrix TransformMatrix);
	_bool isIn(_fvector vPosition, CCell** ppNeighbor, _uint* iLineIndex = nullptr);

#ifdef _DEBUG
public:
	HRESULT Render(_uint iPassIndex);
#endif 

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	_float3			m_vOriginalPoints[POINT_END];
	_float3			m_vPoints[POINT_END];

	_float3			m_vLines[LINE_END];
	_uint			m_iIndex = 0;
	_uint			m_iOption = 0;
	CCell*			m_pNeighbor[LINE_END];
	NAVIMESHDESC	m_NaviDesc;

#ifdef _DEBUG
private:
	//class CVIBuffer_LineStrip*			m_pVIBuffer = nullptr;
	class CVIBuffer_Triangle*				m_pVIBuffer = nullptr;
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Ready_Debug_Buffer();
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, NAVIMESHDESC& NaviDesc, _uint iIndex);
	virtual void Free() override;
};

END