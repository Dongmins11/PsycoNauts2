#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CTimerUI final : public CGameObject
{
private:
	explicit CTimerUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTimerUI(const CTimerUI& rhs);
	virtual ~CTimerUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	void Set_IsFinished(_bool isFinished) { m_isFinished = isFinished; }
	_bool Get_IsFinished() const { return m_isFinished; }

private:
	void Compute_TimerNumbers();
	void Compute_DisitNumbers(_int Number,_uint* Tenth, _uint* Ones);
private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

	CVIBuffer_Rect*		m_pVIBufferCom[8] = { nullptr };

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	_bool				m_isFinished = false;
	
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float				m_fAddX = 0.f;
	_float4x4			m_ProjMatrix;

	_uint				m_iCurrentImgIndex = 0;
	_double				m_lfTimeAcc = 0.0;
	_uint				m_iTimeNumbers[6] = { 0 };
private:
	_tchar*				m_szVIBufferTags[8] = { TEXT("Com_VIBuffer0"),TEXT("Com_VIBuffer1"),TEXT("Com_VIBuffer2"),TEXT("Com_VIBuffer3"),
												TEXT("Com_VIBuffer4"),TEXT("Com_VIBuffer5"),TEXT("Com_VIBuffer6"),TEXT("Com_VIBuffer7") };
public:
	static CTimerUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END