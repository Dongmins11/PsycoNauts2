#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_EffectRect;
class CTexture;
class CEffect_Option;
END

BEGIN(Client)
class CRectEffect final :public CGameObject
{
	enum EFFECT_OPTION { OPTION_LINEAR, OPTION_EASE_IN_QUAD, OPTION_EASE_OUT_QUAD, OPTION_EASE_IN_OUT, OPTION_END };
	enum EFFECT_CHACK { CHACK_FI, CHACK_FO, CHACK_UA, CHACK_END };

protected:
	explicit CRectEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRectEffect(const CRectEffect& rhs);
	virtual ~CRectEffect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Delete_ColliderBox() {
		m_isDestroy = true;
	}

public:  /* 툴에서만 진행될 함수 */
	void Effect_Time(_double TimeDetla);
	void Effect_TimeAcc(_bool* pPlay, _float fTotalTime, _double* pTargetTimeAcc, _double TimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_EffectRect*   m_pVIBuffer = nullptr;
	CTexture*				m_pAlphaMapCom = nullptr;
	CTexture*				m_pDiffuseMapCom = nullptr;
	CEffect_Option*			m_pOptionCom = nullptr;

public:
	/* 활성화 */
	_bool	m_CDT_isPlay = true;
	_bool	m_CDT_Start = false;

	/* 시간 누적 */
	_double m_CDT_TimeAcc = 0.0;
	_double m_PP_TimeAcc = 0.0;
	_double m_PS_TimeAcc = 0.0;
	_double m_PR_TimeAcc = 0.0;
	_double m_FI_TimeAcc = 0.0;
	_double m_FO_TimeAcc = 0.0;
	_double m_US_TimeAcc = 0.0;
	_double m_UA_TimeAcc = 0.0;

	/* UV Sprite */
	_double m_US_SpriteAcc = 0.0;
	_uint m_US_iNowSprite = 0;
	_uint m_US_fCurIndexX = 0;
	_uint m_US_fCurIndexY = 0;

	/* 애니메이션 누적 */
	_float	m_UA_UVAccX = 0.f;
	_float	m_UA_UVAccY = 0.f;

	/* 상속 */
	_bool	  m_isOnce = false;
	_float4x4 m_ParentMat;

public: /* 저장해야하는 값 */

	EFFECTRECTINFO m_tRectInfo;

public:
	HRESULT Pattern_Option(_double TimeDelta);
	_float	Option_Type(_bool* isOptionType, _double TimeAcc, _float fStartTime, _float fStartValue, _float fEndValue, _float fEndTime, _double TimeDelta);
	_float	Linear(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_In_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_Out_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_In_Out(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_Out_Bounce(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CRectEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};
END
