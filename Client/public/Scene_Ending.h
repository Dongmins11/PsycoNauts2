#pragma once
#include "Client_Defines.h"
#include "Scene.h"

BEGIN(Engine)
class CNavigetion;
END

BEGIN(Client)

class CScene_Ending final :public CScene
{
public:
	explicit CScene_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CScene_Ending() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual _int Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light_Desc();
	HRESULT Ready_Animation_Object();
	void Setting_ObjectAnimation(const _tchar* LayerTag, _uint Objectindex, _uint AnimationIndex);
private:
	class CLoader*		m_pNextLoader = nullptr;

private:
	_float		m_fLoadTimeAcc = 0.0;
	_bool		m_isMove = false;

	_double	    m_FadeInTimer = 0.0;
	_bool		m_OnesCheck = true;

public:
	static CScene_Ending* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;

};

END