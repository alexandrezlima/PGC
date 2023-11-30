#include "HLSL.h"
#include "Interfaces/IPluginManager.h"
#include <Runtime/Engine/Classes/Materials/MaterialExpressionCustom.h>

#define LOCTEXT_NAMESPACE "FHLSLModule"

void FHLSLModule::StartupModule()
{
	//Este c�digo ser� executado ap�s este m�dulo ser carregado na mem�ria. O tempo exato � especificado no arquivo .uplugin por m�dulo.
	FString PluginBaseDirectory = IPluginManager::Get().FindPlugin(HLSL_PluginName)->GetBaseDir();

	//Remonta o caminho original da pasta shaders.
	FString ShaderDirectory = FPaths::Combine(PluginBaseDirectory, TEXT("Shaders"));

	//Mapeia o diret�rio de shaders para um diret�rio virtual.
	AddShaderSourceDirectoryMapping("/Plugin/HLSL", ShaderDirectory);
}

void FHLSLModule::ShutdownModule()
{
	// Esta fun��o � chamada durante o desligamento e a limpeza do m�dulo.

	//Reseta os diret�rios virtuais criados. �til para economia de recursos computacionais.
	ResetAllShaderSourceDirectoryMappings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHLSLModule, HLSL)