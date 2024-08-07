#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "Transform.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;

END

BEGIN(Client)

class COfficeWalls final :public CGameObject
{

public:
	enum RECTMOVE { RECTMOVE_NORMAL, RECTMOVE_SPEEDUP, RECTMOVE_END };
	enum WALLTYPE { WALLTYPE_LEFT, WALLTYPE_RIGHT, WALLTYPE_END };
private:
	COfficeWalls(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit COfficeWalls(const COfficeWalls& rhs);
	virtual ~COfficeWalls() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:/*For.Set*/
	void Set_RectMove(RECTMOVE eRectMove);
	void Set_Scaling(_float3 vScale) { m_vScale = vScale, m_pTransformCom->Scaling(XMLoadFloat3(&m_vScale)); }
	void Set_Pos(_float3 vPos) { m_vPos = vPos, m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPos)); }
	void Set_Rad(_float fRad) { m_fRad = fRad, m_pTransformCom->RotationByRadian(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRad); }
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
	CModel*					m_pModelCom = nullptr;


private:
	RECTMOVE				m_eRectMove = RECTMOVE_END;
	WALLTYPE				m_eWallType = WALLTYPE_END;
	_float					m_fSourSpeed[RECTMOVE_END];
	_float					m_fDestSpeed[RECTMOVE_END];
	_float					m_fMaxTime[RECTMOVE_END];

	_bool					m_bIsLerp = true;
	_double					m_dTimeAcc = 0.0;
	_float					m_fRatio = 0.0;
	_float3					m_vScale;
	_float3					m_vPos;
	_float					m_fRad = 0.f;
	_float3					m_vBackUpPos;

private:
	/*For.Shadow*/
	_float4x4				m_LightView;
	_bool					m_bIsRenderingShadow = true;
	/*For.MotionBlur*/
	_float4x4				m_OldWolrd;
	_float4x4				m_OldView;

	_float					m_ArrKey[3];
public:
	static COfficeWalls* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;

};

END