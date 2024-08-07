#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectInstance final : public CVIBuffer_Instance
{
public:
	typedef struct tagViewZ
	{
		_float       fViewZ;
		_float4		 vPosition;
	}VIEWDESC;
	typedef struct tagViewAlphaZ
	{
		_float       fViewZ;
		_float4		 vPosition;
		_float3		 vResetDist;
		_float		 fAlpha;
	}VIEWALPHADESC;
	struct stlListSort
	{
		bool operator()(VIEWDESC vOne, VIEWDESC vTwo) const
		{
			return vOne.fViewZ > vTwo.fViewZ;
		}
	};
	struct stlListAlphaSort
	{
		bool operator()(VIEWALPHADESC vOne, VIEWALPHADESC vTwo) const
		{
			return vOne.fAlpha > vTwo.fAlpha;
		}
	};
private:
	CVIBuffer_RectInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_RectInstance(const CVIBuffer_RectInstance& rhs);
	virtual ~CVIBuffer_RectInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Update_Smoke_Landing(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, _double TimeAcc, _double TimeDelta);
	void Update_Billboard(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Option(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Smoke_Die(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Spark(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Smoke_Working(EFFECTPARTICLEINFO* pParticleInfo, _fvector vTargetPos, _fvector vCamPos, _bool isUpdate, _double TimeDelta);
	void Update_Smoke_Object(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Paper(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Fire(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Particle(EFFECTPARTICLEINFO* pParticleInfo, _fvector vTargetPos, _fvector vCamPos, _double TimeDelta);
	void Update_BulletFire(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _fvector vCamPos, _double TimeDelta);
	void Update_VentSmoke(EFFECTPARTICLEINFO* pParticleInfo, _fvector vPos, _fvector vCamPos, _double TimeDelta);
	void Update_PickingObject(EFFECTPARTICLEINFO* pParticleInfo,  _fvector vTargetPos, _fvector vCamPos, _uint iLayerSize, _double TimeDelta);
	void Update_BurnerFire(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, _double TimeAcc, _double TimeDelta);
	void Update_Sparkling(EFFECTPARTICLEINFO* pParticleInfo, _fvector vPos, _fvector vCamPos, _double TimeDelta);
	void Update_Sparkling_Fruit(EFFECTPARTICLEINFO* pParticleInfo, _fvector vPos, _fvector vCamPos, _uint iLayerSize, _double TimeDelta);
	void Update_Boss_Effect(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Boss_Landing(EFFECTPARTICLEINFO* pParticleInfo, _fvector vCamPos, _double TimeAcc, _double TimeDelta);

private:
	_float3*	m_pResetPos = nullptr;
	_float3*	m_pResetDist = nullptr;
	_bool*		m_pReset = nullptr;

	_float3		m_vAxis;
	_bool		m_isFirst = false;
	_double		m_UVTimeAcc = 0.0;

private: /* Smoke_Working */
	_bool		m_isUpdate = false;
	_bool		m_isTick = false;
	_float		m_fResetTime = 0.f;

private:
	_float	Set_Option(_uint iOption, _float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Linear(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_In_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_Out_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_In_Out(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	
public:
	static CVIBuffer_RectInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END