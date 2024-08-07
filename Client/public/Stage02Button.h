#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CSphereCollider;
END

BEGIN(Client)
class CStage02Button final : public CGameObject
{
private:
	explicit CStage02Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStage02Button(const CStage02Button& rhs);
	virtual ~CStage02Button() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	virtual void OnTriggerEnter(CCollisionObject& collision) override;
	virtual void OnTriggerExit(CCollisionObject& collision) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pButtonTextureCom = nullptr;
	CTexture*			m_pButtonShowTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CSphereCollider*	m_pTriggerCom = nullptr;

	CVIBuffer_Rect*		m_pBackground_BufferCom = nullptr;
	CVIBuffer_Rect*		m_pKey_BufferCom[4] = { nullptr};
	CVIBuffer_Rect*		m_pNumPadButtons_BufferCom[12] = { nullptr };

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
private:
	_int Picking_NumberPad();
	_bool Checking_Password();
private:
	_int				m_iKeyCheck[4] = { -1,-1,-1,-1};			// 각자 키가 눌렸는지 
	//_float				m_fTimeAcc[4] = {};				// 각자 키가 눌린 시간
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

	RECT				m_NumberPad[12];

	_bool				m_isCorrect = false;
	_bool				m_isOpenPasswordUI = false;
private:
	_tchar*				m_szLayerTags[16] = {};
public:
	static CStage02Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END