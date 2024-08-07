#pragma once

#include "Component.h"

BEGIN(Engine)
class CCell;

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_uint		iCurrentIndex;
	}NAVIDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pNaviDataFilePath);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void Set_CurrentIndex(_uint idx) { m_NaviDesc.iCurrentIndex = idx; }

public:
	_uint	Get_CurrentIndex() { return m_NaviDesc.iCurrentIndex; }
	const _uint	Get_NaviSize() const { return (_uint)m_Cells.size(); }
	_float3* Get_Points(_uint idx); 
public:

	void Update(_fmatrix TransformMatrix);
	_bool Move_OnNavigation(_fvector vPosition);
	_float OnNavigationGetY(_fvector vPosition, _fvector vCurPosition = XMVectorZero());
	_bool CheckingCCW(_fvector vPointA, _fvector vPointB, _fvector vPointC);
	_bool CheckingCCW(_float3 vPointA, _float3 vPointB, _float3 vPointC);

	HRESULT Create_Cells(const _tchar* pNaviDataFilePath);
	HRESULT Create_NaviData(const _tchar* pNaviDataFilePath);
	
	void Clear_Cells();
	void Clear_NaviData();
	
#ifdef _DEBUG
public:
	HRESULT Render(_uint iPassIndex);
#endif // _DEBUG


private:
	vector<class CCell*>				m_Cells;
	typedef vector<class CCell*>		CELLS;

	vector<_float3*>					m_vecPoints;
private:
	NAVIDESC		m_NaviDesc;

private:
	HRESULT SetUp_Neighbor();


public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNaviDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END