#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END

BEGIN(Client)

class CBag final :public CGameObject
{
protected:
	explicit CBag(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBag(const CBag& rhs);
	virtual ~CBag() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void Setting_Throwing(_fvector vDir);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CBoxCollider*			m_pCollider = nullptr;

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
private:
	_bool					m_bPickingCheck = false;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END