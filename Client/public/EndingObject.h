#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Quest.h"
#include "Camera_Fly.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CEndingObject :public CGameObject
{
protected:
	explicit CEndingObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEndingObject(const CEndingObject& rhs);
	virtual ~CEndingObject() = default;

protected:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
protected:
	HRESULT SetUp_Component(_uint iSceneIndex, const _tchar* pPrototypeTag);
	HRESULT SetUp_ConstantTable();

private:
	_tchar* m_pLayerTag = TEXT("");
public:
	static CEndingObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free();
};

END