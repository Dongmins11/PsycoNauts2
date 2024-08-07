#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CScene abstract : public CBase
{
public:
	explicit CScene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void Setting_StageUpdate(_uint iSceneNum) {};
protected:
	virtual void* Get_SceneData() { return nullptr; };
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	_bool			m_isMoveScene = false;
	
public:
	virtual void Free() override;
};

END