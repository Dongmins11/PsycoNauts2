#pragma once

#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Client)

class CScene_Logo final : public CScene
{
public:
	explicit CScene_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Logo() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	/* 사본객체들을 생성한다. */
public:
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

private:
	class CNavigation* m_pNavigation = nullptr;


public:
	static CScene_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END