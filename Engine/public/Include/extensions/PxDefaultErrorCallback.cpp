#include "stdafx.h"
#include "PxDefaultErrorCallback.h"

physx::PxDefaultErrorCallback::PxDefaultErrorCallback()
{
}

physx::PxDefaultErrorCallback::~PxDefaultErrorCallback()
{
}

void physx::PxDefaultErrorCallback::reportError(PxErrorCode::Enum code, const char * message, const char * file, int line)
{
}
