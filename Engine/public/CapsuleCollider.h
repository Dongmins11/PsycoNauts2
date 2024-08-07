#pragma once

#include "PhysXCollider.h"

BEGIN(Engine)
class ENGINE_DLL CCapsuleCollider final : public CPhysXCollider
{
public:
	typedef struct tagDesc
	{
		CPhysXCollider::DESC tColliderDesc;
		_float3 vScale = { 1.f,1.f,1.f };
	}DESC;

private:
	explicit CCapsuleCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CCapsuleCollider(const CCapsuleCollider& _rhs);
	virtual ~CCapsuleCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	const _float3& Get_Scale() const;

private:
	_float3 m_vScale = { 1.f,1.f,1.f };

public:
	static CCapsuleCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END