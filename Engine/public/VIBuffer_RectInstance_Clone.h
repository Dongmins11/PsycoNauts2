#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectInstance_Clone final : public CVIBuffer_Instance
{
public:
	typedef struct tagViewZ
	{
		_float       fViewZ;
		_float4		 vPosition;
	}VIEWDESC;
	struct stlListSort
	{
		bool operator()(VIEWDESC vOne, VIEWDESC vTwo) const
		{
			return vOne.fViewZ > vTwo.fViewZ;
		}
	};
private:
	CVIBuffer_RectInstance_Clone(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_RectInstance_Clone(const CVIBuffer_RectInstance_Clone& rhs);
	virtual ~CVIBuffer_RectInstance_Clone() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
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
	void Update_VentSmoke(EFFECTPARTICLEINFO* pParticleInfo, _fvector vTargetPos, _fvector vCamPos, _double TimeDelta);

private:
	_float3		m_pResetPos[10];
	_float3		m_pResetDist[10];
	_bool		m_pReset[10];

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
	static CVIBuffer_RectInstance_Clone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END