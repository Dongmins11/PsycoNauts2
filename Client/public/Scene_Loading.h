#pragma once

#include "Client_Defines.h"
#include "Scene.h"

/* ������������ ��������. */
/* ������������ ����� �����͵��� �غ�. */
/* ���� ������ �������������ؼ� ����. */
/* �ε��߿� ��������� ����� ����Ѵ�. */

BEGIN(Client)

class CScene_Loading final : public CScene
{
public:
	explicit CScene_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Loading() = default;
public:
	virtual HRESULT NativeConstruct(SCENE eNextScene);
	virtual _int Tick(_double fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Loading_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Loading_Player(const _tchar* pLayerTag);

private:
	SCENE			m_eNextScene = SCENE_END;
	class CLoader*	m_pLoader = nullptr;

	_double			m_TimeAcc = 0.0;
public:
	static CScene_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SCENE eNextScene);
	virtual void Free() override;
};

END