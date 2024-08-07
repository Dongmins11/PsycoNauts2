#pragma once

#include "PhysXCollider.h"

BEGIN(Engine)
class ENGINE_DLL CBoxCollider final : public CPhysXCollider
{
public:
	typedef struct tagDesc
	{
		CPhysXCollider::DESC tColliderDesc;
		_float3 vScale = { 1.f,1.f,1.f };
	}DESC;

private:
	explicit CBoxCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CBoxCollider(const CBoxCollider& _rhs);
	virtual ~CBoxCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	const _float3& Get_Scale() const;

private:
	_float3 m_vScale = { 1.f,1.f,1.f };

public:
	static CBoxCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END