#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
class CCollider;

END

BEGIN(Client)

class CPlatingPlate_Phase0 final :public CGameObject
{
protected:
	explicit CPlatingPlate_Phase0(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlatingPlate_Phase0(const CPlatingPlate_Phase0& rhs);
	virtual ~CPlatingPlate_Phase0() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	_bool	Checking_FruitCooking(FruitType eFruitType, CookType eCookType,_uint* iCookingUINumber);
	void	Add_PlatingCount() { m_iPlatingCount += 1; }
	_uint	Get_PlatingCount()const { return m_iPlatingCount; }
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CGameObject*			m_pFruitCollider[2] = { nullptr };
	//CBoxCollider*			m_pTriggerCom[2] = { nullptr };
	CNavMeshCollider*		m_pColliderCom = nullptr;
#ifdef _DEBUG
	//CCollider*				m_pTriggerRenderCom[2] = { nullptr };
#endif // _DEBUG

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
	_bool					m_isCreated = false;

	vector<_float3*>		m_vecNaviData;
	vector<FruitType>		m_vecPlateFruits;
	vector<_uint>			m_vecCookTypes;

	_uint					m_iPlatingCount = 0;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Loading_PlateNaviData(const _tchar * pNaviDataFilePath);
	HRESULT SetUp_Triggers();

#ifdef _DEBUG
	HRESULT SetUp_RenderComponentForDebug();
#endif // _DEBUG

public:
	static CPlatingPlate_Phase0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END