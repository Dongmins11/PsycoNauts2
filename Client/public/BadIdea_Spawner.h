#pragma once
#include "Enemy.h"

BEGIN(Engine)
class CTransform;
class CModel;
class CRenderer;
END

BEGIN(Client)

class CBadIdea_Spawner final :public CGameObject
{
protected:
	explicit CBadIdea_Spawner(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBadIdea_Spawner(const CBadIdea_Spawner& rhs);
	virtual ~CBadIdea_Spawner() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	_bool					m_IsSpawned = false;
	_double					m_dAccTime = 0.0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBadIdea_Spawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END