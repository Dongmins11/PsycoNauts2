#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
//class CBoxCollider;
END

BEGIN(Client)

class CNpcDoor final:public CGameObject
{
protected:
	explicit CNpcDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNpcDoor(const CNpcDoor& rhs);
	virtual ~CNpcDoor() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void Setting_Throwing(_fvector vDir);

private:
	void Setting_MyCount();
	void MoveAnimation(_double TimeDelta);

public:
	void Set_Update_Modle(_bool bMove) { m_bMove = bMove; }
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	//CBoxCollider*			m_pCollider = nullptr;

private:
	_int					m_NavigationIndex = 0;
	_uint					m_iPX_TYPE = 0;
	_uint					m_iMyCount = 0;
	_bool					m_bMove = false;
	_bool					m_Finsh = false;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CNpcDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END