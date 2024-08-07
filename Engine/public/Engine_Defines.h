#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <wincodec.h>
#include <d3dcompiler.h>
#include <d3dcompiler.inl>
using namespace DirectX;


#ifdef _DEBUG

#pragma comment(lib, "PxFoundationDEBUG_x64.lib")
#pragma comment(lib, "LowLevelAABBDEBUG.lib")
#pragma comment(lib, "LowLevelClothDEBUG.lib")
#pragma comment(lib, "LowLevelDEBUG.lib")
#pragma comment(lib, "LowLevelDynamicsDEBUG.lib")
#pragma comment(lib, "LowLevelParticlesDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX3VehicleDEBUG.lib")
#pragma comment(lib, "PsFastXmlDEBUG_x64.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x64.lib")
#pragma comment(lib, "PxTaskDEBUG_x64.lib")
#pragma comment(lib, "SceneQueryDEBUG.lib")
#pragma comment(lib, "SimulationControllerDEBUG.lib")

#else
#pragma comment(lib, "LowLevel.lib")
#pragma comment(lib, "LowLevelAABB.lib")
#pragma comment(lib, "LowLevelCloth.lib")
#pragma comment(lib, "LowLevelDynamics.lib")
#pragma comment(lib, "LowLevelParticles.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3Vehicle.lib")
#pragma comment(lib, "PxFoundation_x64.lib")
#pragma comment(lib, "PxPvdSDK_x64.lib")
#pragma comment(lib, "PxTask_x64.lib")
#pragma comment(lib, "SceneQuery.lib")
#pragma comment(lib, "SimulationController.lib")

#endif //_DEBUG

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <process.h>
#include <typeinfo.h>
#include <iostream>
#include <time.h>

using namespace std;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 

#endif

#endif // _DEBUG


#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"
#include "Engine_Struct.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

enum CHANNELID { BGM, PLAYER, EFFECT, NATURE_SOUND, SCRIPT_RAZ, SCRIPT_MILLA, SCRIPT_LILI, SCRIPT_SASHA, SCRIPT_COACH,SCRIPT_LOBOTO ,UI,
	MONSTER, MONSTERATT, BOSS, BOSS_ATT, STAGE3ONES, JELLYHIT, EATCUTSCENE, COOKEND,
	PLAYER_CHOP_A, PLAYER_CHOP_B, PLAYER_CHOP_C,
	PLAYER_DOWNATT, PLAYER_GRAB, PLAYER_SKILLE, PLAYER_SKILLE_ONE, PLAYER_MOVESKILLE_ONE, PLAYER_MOVESKILLE_TWO,
	PLAYER_TRAMPOLINE, PLAYER_TRAMPOLINE_TWO,
	PLAYER_CUTSCENE, PLAYER_JUMP_ONE, PLAYER_JUMP_TWO,
	PLAYER_LANDING, PLAYER_RUNNING, PLAYER_DEAD,
	PLAYER_HIT_ONE, PLAYER_HIT_TWO, PLAYER_HIT_THREE, PLAYER_HIT_FOR,
	PLAYER_KNOCKBACK, PLAYER_DASHSOUND, PLAYER_DASHATTACKSOUND,
	PLAYER_FLOOTING, PLAYER_FLOOTING_END,
	PLAYER_LEDGE_SOUND, PLAYER_LEDGE_JUMP_SOUND,
	PLAYER_BOUNCER, PLAYER_DODGE, PLAYER_STOP,
	PLAYER_CLIME, PLAYER_CLIME_JUMP,
	PLAYER_LEFT_HAND, PLAYER_RIGHT_HAND, PLAYER_RIGHT_HAND_GRAB, PLAYER_RIGHT_HAND_THROW,
	PLAYER_DISH_THROW, PLAYER_LEFT_FOOD_THROW, PLAYER_RIGHT_FOOD_THROW,
	PLAYER_HEALPACK, PLAYER_PULLSWITCING, PLAYER_GROUND_SLIDESOUND,
	STATICK_MESH, PLAYER_BUTTON, PLAYER_CHOP_PIG, PLAYER_GAS_BURRER, PLAYER_GAS_BURRER_FIRE,
	MOUSE_CUTSCENE,
	MAXCHANNEL };

#include <io.h>
#include <tchar.h>
#include <fmod.h>

using namespace Engine;
