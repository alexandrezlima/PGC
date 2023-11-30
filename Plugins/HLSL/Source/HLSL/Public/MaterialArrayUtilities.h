#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MaterialArrayUtilities.generated.h"

/**
 * Biblioteca de fun��es para utiliza��o global em blueprints.
 */
UCLASS()
class HLSL_API UMaterialArrayUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Cria uma textura din�mica que guarda um array de vetores 4D.
	UFUNCTION(BlueprintCallable, Category = "Action")
		static UTexture2D* CreateTextureFromVector(TArray<FVector4> VectorArray, int32 TextureWidth);

	//Dado uma textura, realiza uma opera��o de atualiza��o, guardando os dados de um array de vetores 4D.
	UFUNCTION(BlueprintCallable, Category = "Action")
		static UTexture2D* UpdateTextureFromVector(TArray<FVector4> VectorArray, int32 TextureWidth, UTexture2D* Texture);	
};
