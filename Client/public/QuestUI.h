#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CQuestUI final : public CGameObject
{
public:
	enum UISTATE { UISTATE_START, UISTATE_IDLE, UISTATE_SELECTED, UISTATE_TAB, UISTATE_END };
private:
	explicit CQuestUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CQuestUI(const CQuestUI& rhs);
	virtual ~CQuestUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	void Set_IsFinished(_bool isFinished) { m_isFinished = isFinished; }
	_bool Get_IsFinished() const { return m_isFinished; }

	void WaitingForMoveX();

	void Set_MoveX(_bool isMove) { m_isMoveX = isMove; }
	_bool Get_MoveX() const { return m_isMoveX; }

	void Set_UIState(UISTATE eState) { m_eCurState = eState; }
	const UISTATE Get_UIState() const { return m_eCurState; }

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;

	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	void	Update_UIState();
	void	Update_UIPosY(_double TimeDelta);
private:
	_bool				m_isFinished = false;

	//_bool				m_isWaiting = false;
	_bool				m_isMoveX = false;
	// idle까지 가는 변수
	_bool				m_isIdle = false;
	// 선택되어 있는지 판단하는 변수
	_bool				m_isSelected = false;
	// 탭중인지 판단하는 변수
	_bool				m_isTapping = false;

	_bool				m_isAnimatedUp = false;
	_bool				m_isAnimatedDown = false;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4			m_ProjMatrix;

	_uint				m_iCurrentImgIndex = 0;
	UISTATE				m_ePreState = UISTATE_END;
	UISTATE				m_eCurState = UISTATE_END;
	_float				m_fYPosArr[UISTATE_END] = {721.f, 665.f, 635.f, 439.f};
	_float				m_fXFinalPos = 0.0f;
	_uint				m_iLoadingTextureIndex = 1;
private:
public:
	static CQuestUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END