#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FObscuraCore : public IModuleInterface
{
public:
	static inline FObscuraCore& Get()
	{
		return FModuleManager::LoadModuleChecked<FObscuraCore>("ObscuraCore");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ObscuraCore");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
