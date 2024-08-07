#pragma once

#include "PhysXCollider.h"

BEGIN(Engine)
class ENGINE_DLL CSphereCollider final : public CPhysXCollider
{
public:
	typedef struct tagDesc
	{
		CPhysXCollider::DESC tColliderDesc;
		_float3 vScale = { 1.f,1.f,1.f };
	}DESC;

private:
	explicit CSphereCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CSphereCollider(const CSphereCollider& _rhs);
	virtual ~CSphereCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	const _float3& Get_Scale() const;

private:
	_float3 m_vScale = { 1.f,1.f,1.f };

public:
	static CSphereCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END