// Copyright 2023, Alexandre Zeferino Lima. All Rights Reserved.

#include "HLSL.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FHLSLModule"

void FHLSLModule::StartupModule()
{
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginBaseDirectory = IPluginManager::Get().FindPlugin(HLSL_PluginName)->GetBaseDir();

	FString ShaderDirectory = FPaths::Combine(PluginBaseDirectory, TEXT("Shaders"));

	FString VirtualShaderDirectory = FString::Printf(TEXT("/Plugins/%s"), *HLSL_PluginName);

	AddShaderSourceDirectoryMapping(VirtualShaderDirectory, ShaderDirectory);

}

void FHLSLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ResetAllShaderSourceDirectoryMappings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHLSLModule, HLSL)