#pragma once
#include "Client_Defines.h"
#include "Fruit.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBoxCollider;
END

BEGIN(Client)

class CBread final :public CFruit
{
protected:
	explicit CBread(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBread(const CBread& rhs);
	virtual ~CBread() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	void Setting_ModelNames();
private:
	//CRenderer*				m_pRendererCom = nullptr;
	//CTransform*				m_pTransformCom = nullptr;
	//CModel*					m_pModelCom = nullptr;
	//CBoxCollider*		m_pColliderCom = nullptr;
	//CBoxCollider*			m_pColliderCom = nullptr;
	CGameObject*		m_pEffect = nullptr;

	_bool				m_isChangeEffect = false;

	_vector				vPos;
private:
	_int					m_NavigationIndex = 0;
	LayerType				m_iPX_TYPE = LayerType::LAYERTYPE_END;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Create_Effect();
	HRESULT	Change_Effect();
public:
	static CBread* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END