#pragma once
#include "Quest.h"

BEGIN(Client)
class CNormalQuest :
	public CQuest
{
public:
	explicit CNormalQuest(QUESTDESC& tDesc);
	virtual ~CNormalQuest();

	virtual void Free();
};
END
