#pragma once

#include "PhysXCollider.h"
#include "Model.h"
BEGIN(Engine)
class ENGINE_DLL CMeshCollider final : public CPhysXCollider
{
public:
	typedef struct tagDesc
	{
		CPhysXCollider::DESC tColliderDesc;
		CModel::MODELMESHDATA tModelDataDesc;
	}DESC;

private:
	explicit CMeshCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	explicit CMeshCollider(const CMeshCollider& _rhs);
	virtual ~CMeshCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	static CMeshCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END