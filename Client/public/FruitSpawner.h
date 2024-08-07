#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Fruit.h"

BEGIN(Client)

class CFruitSpawner :public CGameObject
{
private:
	CFruitSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFruitSpawner(const CFruitSpawner& rhs);
	virtual ~CFruitSpawner() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_FruitIndex(FruitType iFruit) { m_iFruitIndex = iFruit; }
public:
	void Create_Fruit(FruitType iFruit);
	CFruit* Get_FruitObject() { return m_pFruitObject; }
	void Create_SpwnerIndex(_uint Count) {m_SpwnerIndex = Count;}

	void Reset_FruitObject() { m_pFruitObject = nullptr; }
	void Stting_Create_Cook_And_Mesh_Setting();

	void Cooking_EndCut_SceneAnimation();

private:
	CFruit*  m_pFruitObject = nullptr;
	_double  m_dCreateTimer = 0.0;
	
private:
	_matrix		 m_SpawnerMatrix;
	FruitType    m_iFruitIndex = FruitType::FRUIT_END;/*0: ´Þ°¿, 1: ¼ö¹Ú ,2: ¹úÁý, 3: »§ , 4: ¾çÆÄ, 5: µÅÁö, 6: µþ±â  */
	_uint        m_SpwnerIndex = 0;

public:
	static CFruitSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene) override;
	virtual void Free() override;
};

END