#include "ObscuraCore.h"
#include "Modules/ModuleManager.h"

#include "Log.h"

void FObscuraCore::StartupModule()
{
	UE_LOG(LogObscuraCore, Log, TEXT("ObscuraCore module starting up"));
}

void FObscuraCore::ShutdownModule()
{
	UE_LOG(LogObscuraCore, Log, TEXT("ObscuraCore module shutting down"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FObscuraCore, ObscuraCore, "ObscuraCore");