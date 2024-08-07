#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CTexture;
END

BEGIN(Client)
class CSlidingMesh final :public CGameObject
{
public:
	enum EFFECT_TYPE { TYPE_LEFT, TYPE_RIGHT, TYPE_END };
private:
	explicit CSlidingMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSlidingMesh(const CSlidingMesh& rhs);
	virtual ~CSlidingMesh() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pAlphaTexture = nullptr;

	_float					m_fTimeAcc = 0.0f;
	EFFECT_TYPE				m_eType = TYPE_END;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CSlidingMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
END