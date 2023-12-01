// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialArrayUtilities.h"

UTexture2D* UMaterialArrayUtilities::CreateTextureFromVector(TArray<FVector4> VectorArray, int32 TextureWidth)
{
    //Cria uma referência, a princípio, vazia.
    UTexture2D* Texture;

    /**
    * Gera uma textura dinâmica com largura definida pelo parâmetro da função(TextureWidth) e altura 1.
    * A textura tem formato PF_A32B32G32R32F, de forma que é possível guardar em cada texel um vetor 4D,
    * onde cada componente rgba pode guardar um ponto flutuante de até 32bits.
    **/
    Texture = UTexture2D::CreateTransient(TextureWidth, 1, PF_A32B32G32R32F);

    //Caso a textura não possa ser criada no momento, retorna um valor nulo.
    if (!Texture)
        return nullptr;

    //Ajusta a informação de mipmaps caso esteja no editor.
#if WITH_EDITORONLY_DATA
    Texture->MipGenSettings = TMGS_NoMipmaps;
#endif

    //Ajusta algumas configurações da textura para evitar perda de dados presentes nos texels.
    Texture->NeverStream = true;
    Texture->SRGB = 0;
    Texture->LODGroup = TEXTUREGROUP_Pixels2D;
    FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];

    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

    //Copia os dados do array, um vetor 4D para cada texel da textura.
    FLinearColor* ColorData = (FLinearColor*)Data;
    for (int32 i = 0; i < TextureWidth; i++)
    {
        if (i < VectorArray.Num())
            ColorData[i] = FLinearColor(VectorArray[i].X, VectorArray[i].Y, VectorArray[i].Z, VectorArray[i].W);
        else // Caso improvável, uma vez que a utilização pressupõe que a largura corresponde ao tamanho do array que será utilizado.
            ColorData[i] = FLinearColor(0, 0, 0, 0); //Mas, caso ocorra, isto é, se o array for menor do que a largura da textura, preenche-se com zeros.
    }

    //Atualiza a textura.
    Mip.BulkData.Unlock();
    Texture->UpdateResource();

    //Retorna a textura criada.
    return Texture;
}

/**
* Dado uma textura, atualiza a informação nela presente a partir de um array de vetores 4D.
**/
UTexture2D* UMaterialArrayUtilities::UpdateTextureFromVector(TArray<FVector4> VectorArray, int32 TextureWidth, UTexture2D* Texture)
{
    //Se a textura não existe, cria uma nova a partir do array de FVector4.
    if (!Texture)
        return CreateTextureFromVector(VectorArray, TextureWidth);

    //Verifica se as dimensões são compatíveis com a nova inserção. Em caso negativo, gera uma nova textura compatível.
    if (Texture->GetSizeX() != TextureWidth || Texture->GetSizeY() != 1)
        return CreateTextureFromVector(VectorArray, TextureWidth);

    //Caso a textura exista e suas dimensões sejam compatíveis, atualiza-se a informação nela contida.
    FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];
    void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

    //Copia os dados do array, um vetor 4D para cada texel da textura.
    FLinearColor* ColorData = (FLinearColor*)Data;
    for (int32 i = 0; i < TextureWidth; i++)
    {
        if (i < VectorArray.Num())
            ColorData[i] = FLinearColor(VectorArray[i].X, VectorArray[i].Y, VectorArray[i].Z, VectorArray[i].W);
        else // Caso improvável, uma vez que a utilização pressupõe que a largura corresponde ao tamanho do array que será utilizado.
            ColorData[i] = FLinearColor(0, 0, 0, 0); //Mas, caso ocorra, isto é, se o array for menor do que a largura da textura, preenche-se com zeros.
    }

    //Atualiza a textura.
    Mip.BulkData.Unlock();
    Texture->UpdateResource();

    //Retorna a textura atualizada.
    return Texture;
}


