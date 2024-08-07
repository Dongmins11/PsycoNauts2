#pragma once

#include "Base.h"

BEGIN(Engine)
class CSoundManager final : public CBase
{
	DECLARE_SINGLETON(CSoundManager)
private:
	CSoundManager();
	virtual ~CSoundManager() = default;

public:
	HRESULT Initialize();

public:
	_bool IsChannelPlaying(CHANNELID eID);
	HRESULT PlaySound(TCHAR* pSoundKey, CHANNELID eID);
	HRESULT PlayBGM(TCHAR* pSoundKey);
	HRESULT StopSound(CHANNELID eID);
	HRESULT StopAll();
	HRESULT	BGMVolumeUp(_float _vol);
	HRESULT	BGMVolumeDown(_float _vol);
	HRESULT SettingVolume(_float _vol,CHANNELID eID);
	HRESULT	BGMPause(CHANNELID eID);

private:
	HRESULT LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

	_float m_BGMvolume = SOUND_DEFAULT;
	_bool		m_bPause = false;

public:
	virtual void Free() override;
};
END
