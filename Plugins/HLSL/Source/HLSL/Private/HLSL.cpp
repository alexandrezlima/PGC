#include "HLSL.h"
#include "Interfaces/IPluginManager.h"
#include <Runtime/Engine/Classes/Materials/MaterialExpressionCustom.h>

#define LOCTEXT_NAMESPACE "FHLSLModule"

void FHLSLModule::StartupModule()
{
	//Este código será executado após este módulo ser carregado na memória. O tempo exato é especificado no arquivo .uplugin por módulo.
	FString PluginBaseDirectory = IPluginManager::Get().FindPlugin(HLSL_PluginName)->GetBaseDir();

	//Remonta o caminho original da pasta shaders.
	FString ShaderDirectory = FPaths::Combine(PluginBaseDirectory, TEXT("Shaders"));

	//Mapeia o diretório de shaders para um diretório virtual.
	AddShaderSourceDirectoryMapping("/Plugin/HLSL", ShaderDirectory);
}

void FHLSLModule::ShutdownModule()
{
	// Esta função é chamada durante o desligamento e a limpeza do módulo.

	//Reseta os diretórios virtuais criados. Útil para economia de recursos computacionais.
	ResetAllShaderSourceDirectoryMappings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHLSLModule, HLSL)