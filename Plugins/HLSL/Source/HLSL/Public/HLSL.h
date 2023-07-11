// Copyright 2023, Alexandre Zeferino Lima. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FHLSLModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	const FString HLSL_PluginName = FString(TEXT("HLSL"));

};

