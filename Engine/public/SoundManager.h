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
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;

	_float m_BGMvolume = SOUND_DEFAULT;
	_bool		m_bPause = false;

public:
	virtual void Free() override;
};
END
