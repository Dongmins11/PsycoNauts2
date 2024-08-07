#pragma once

namespace Client
{
	enum SCENE { SCENE_STATIC, SCENE_LOADING, SCENE_LOGO, SCENE_DEMO, SCENE_STAGE01, SCENE_STAGE02, SCENE_STAGE03, SCENE_STAGE04, SCENE_STAGE05,SCENE_ENDING, SCENE_THREADING, SCENE_END };

	inline const char* ToString(SCENE eType)
	{
		switch (eType)
		{
		case SCENE_STATIC:   return "STATIC";
		case SCENE_LOADING:   return "LOADING";
		case SCENE_LOGO: return "LOGO";
		case SCENE_DEMO: return "DEMO";
		case SCENE_STAGE01: return "STAGE01";
		case SCENE_STAGE02: return "STAGE02";
		case SCENE_STAGE03: return "STAGE03";
		case SCENE_STAGE04: return "STAGE04";
		case SCENE_STAGE05: return "STAGE05";
		case SCENE_ENDING: return "Ending";
		default:      return "END";
		}
	}

	const unsigned int g_iWinCX = 1280;
	const unsigned int g_iWinCY = 720;
}

extern HINSTANCE g_hInst;
extern HWND		 g_hWnd;

using namespace Client;