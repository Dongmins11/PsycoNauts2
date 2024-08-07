#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Transform.h"
BEGIN(Engine)
class CRenderer;
class CTexture;
class CTransform;
class CPipeLine;
class CVIBuffer_Rect;
END
BEGIN(Client)
class CFadeInOut final : public CGameObject
{
protected:
	explicit CFadeInOut(ID3D11Device* _pGraphic_Device, ID3D11DeviceContext* _pGraphic_DeviceContext);
	explicit CFadeInOut(const CFadeInOut& _rhs);
	virtual ~CFadeInOut() = default;

public:
	enum TYPE_COLOR{TYPE_COLOR_WHITE, TYPE_COLOR_BLACK, COLOR_END};
	enum TYPE_INOUT{INOUT_IN,INOUT_OUT,INOUT_END};
public:/*For.Get*/
	_bool Get_IsFinish() {
		return m_bFinish;
	}
public:/*For.Set*/
	void Set_Type(_double _Speed, TYPE_COLOR _eColor, TYPE_INOUT _eInOut, _bool _bKeeping = false);
	void Set_Reverse();
	void Active(_double Speed, _float4 vColor, TYPE_INOUT _eInOut);
private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	HRESULT SetUp_Component();

private:
	HRESULT SetUp_ConstantTable();

private:
	void Compute_Alpha();
private:
	class CRenderer*				m_pRenderer = nullptr;
	class CTransform*				m_pTransform = nullptr;
	class CPipeLine*				m_pPipeLine = nullptr;
	class CVIBuffer_Rect*			m_pVIBuffer_Rect = nullptr;
	class CTexture*					m_pTexture = nullptr;

	
	TYPE_COLOR						m_eType_Color = COLOR_END;
	TYPE_INOUT						m_eType_InOut = INOUT_END;
private:
	_float4x4		m_ProjMatrix;
private:/*For.Gem*/
	_float			m_fStartX = 0.f;
	_float			m_fStartY = 0.f;
	_float			m_fSizeX = 0.f;
	_float			m_fSizeY = 0.f;
private:
	_float			m_fAlpha = 0.f;
	_double			m_dTimeAcc = 0.0;
	_double			m_dSpeed = 0.0;
	_bool			m_bKeeping = false;
	_bool			m_bFinish = false;
	_bool			m_bActive = false;
	_float4			m_vColor;
private:
	
public:
	static CFadeInOut* Create(ID3D11Device* _pGraphic_Device, ID3D11DeviceContext* _pGraphic_DeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free()override;
};
END
