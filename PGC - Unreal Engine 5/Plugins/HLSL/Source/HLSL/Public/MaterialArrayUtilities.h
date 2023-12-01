#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MaterialArrayUtilities.generated.h"

/**
 * Biblioteca de funções para utilização global em blueprints.
 */
UCLASS()
class HLSL_API UMaterialArrayUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Cria uma textura dinâmica que guarda um array de vetores 4D.
	UFUNCTION(BlueprintCallable, Category = "Action")
		static UTexture2D* CreateTextureFromVector(TArray<FVector4> VectorArray, int32 TextureWidth);

	//Dado uma textura, realiza uma operação de atualização, guardando os dados de um array de vetores 4D.
	UFUNCTION(BlueprintCallable, Category = "Action")
		static UTexture2D* UpdateTextureFromVector(TArray<FVector4> VectorArray, int32 TextureWidth, UTexture2D* Texture);	
};
