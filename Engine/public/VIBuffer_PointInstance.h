#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstance final : public CVIBuffer_Instance
{
public:
	enum POS_OPTION { OPTION_NONE, OPTION_LINEAR, OPTION_EIQ, OPTION_EOQ, OPTION_EIO, OPTION_END };
private:
	CVIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_PointInstance(const CVIBuffer_PointInstance& rhs);
	virtual ~CVIBuffer_PointInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Update_Default(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_OnePoint(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Spiral(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_RandomPos(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Environment(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Skate(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta);
	void Update_Test(_double TimeDelta);

private:
	_float3*	m_pResetPos = nullptr;
	_float3*	m_pResetDist = nullptr;
	_bool*		m_pReset = nullptr;

	_double		m_UVTimeAcc = 0.f;
private:
	_float	Set_Option(_uint iOption, _float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Linear(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_In_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_Out_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);
	_float	Ease_In_Out(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime);

public:
	static CVIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END