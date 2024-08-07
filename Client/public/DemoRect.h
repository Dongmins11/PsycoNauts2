#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "Transform.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CBoxCollider;
END

BEGIN(Client)

class CDemoRect final :public CGameObject
{
public:
	enum RECTMOVE{RECTMOVE_NORMAL,RECTMOVE_SPEEDUP,RECTMOVE_END};
private:
	CDemoRect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDemoRect(const CDemoRect& rhs);
	virtual ~CDemoRect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual void OnTriggerEnter(CCollisionObject& collision)override;
public:/*For.Set*/
	void Set_RectMove(RECTMOVE eRectMove);
	void Set_Scaling(_float3 vScale) { m_vScale = vScale, m_pTransformCom->Scaling(XMLoadFloat3(&m_vScale));}
	void Set_Pos(_float3 vPos);
	void Set_AllSpeed(_float* ArrSourSpeed, _float* ArrDestSpeed) {
		memcpy(m_fSourSpeed, ArrSourSpeed, sizeof(_float) * 2),
		memcpy(m_fDestSpeed, ArrDestSpeed, sizeof(_float) * 2);
	}
	void Set_MaxTime(_float* ArrMaxTime) { memcpy(m_fMaxTime, ArrMaxTime, sizeof(_float) * 2); }
	
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
	void Update(_double TimeDelta);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferRectCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CBoxCollider*			m_pColliderCom = nullptr;
private:
	RECTMOVE				m_eRectMove = RECTMOVE_END;
	_float					m_fSourSpeed[RECTMOVE_END];
	_float					m_fDestSpeed[RECTMOVE_END];

	_bool					m_bIsLerp = true;
	_double					m_dTimeAcc = 0.0;
	_float					m_fRatio = 0.0;
	_float3					m_vScale;
	_float3					m_vPos;
	_float3					m_vBackUpPos;
	_float					m_fMaxTime[RECTMOVE_END];

private:
	_int					m_iNavigationIndex = 0;
	_float3					m_vColPos = { 0,0,0 };
	_int					m_iCurlingIndex = 0;
	_int                    m_iDeleteCurlingIndex = 0;
	_bool					m_bFall = false; /* 쓰러지는거 확인하기 위해서 */
	_bool					m_isMove = false;
	_bool					m_isColController = false;
	_uint					m_iMYObjectCount = 0;

private:
	_uint					m_iLightCount = 18;
public:
	static CDemoRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;

};

END