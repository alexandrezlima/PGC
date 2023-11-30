// Alexandre Zeferino Lima

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MaterialExpressionIO.h"
#include "MaterialValueType.h"
#include "Internationalization/Regex.h"
#include "Materials/MaterialExpression.h"
#include "Materials/MaterialExpressionCustom.h"
#include "CustomHLSL.generated.h"

struct FPropertyChangedEvent;


/*
* Estrutura que guarda o nome e o tipo de uma dada vari�vel.
*/

USTRUCT()
struct FVariableInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
		FString varType;

	UPROPERTY(EditAnywhere, Category = "Input")
		FString varName;
};


UCLASS()
class HLSL_API UCustomHLSL : public UMaterialExpression
{
	GENERATED_BODY()

	/*
	* As vari�veis cuja propriedade � "EditAnywhere" podem ser editadas diretamente no painel
	* de detalhes do n�, dentro do editor de materiais.
	* 
	* Vari�veis que n�o possuem esta propriedade s�o utilizadas internamente no c�digo, sem que
	* o desenvolvedor tenha acesso direto.
	*/

	//T�tulo dado ao n�. Alterar o valor desta vari�vel altera o t�tulo do n� no editor de materiais.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		FString Title = "Custom HLSL";

	//Nome do shader que ser� carregado pelo n�. O arquivo deve estar na pasta Plugins/HLSL/Shaders.
	UPROPERTY(EditAnywhere, Category = "HLSL", meta = (MultiLine = true))
		FString HLSL = "default.hlsl";

	//Abre o shader que atualmente est� inserido no campo "HLSL".
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "HLSL")
		void OpenShader();
	
	//Abre a pasta Shaders do plugin.
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "HLSL")
		void OpenShadersFolder();

	//Armazena o �ltimo c�digo HLSL compilado. �til para buscar se houveram modifica��es no c�digo em rela��o ao �ltimo update.
	UPROPERTY()
		FString LastHLSLCode = "default.hlsl";

	//Boolean que for�a a recompila��o do n�. Alterar este valor chama a fun��o PostEditChangeProperty, a qual reconstr�i o n�.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		bool Recompile = false;

	//Define o tipo de sa�da da fun��o principal do c�digo HLSL. Em breve ser� setada automaticamente.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TEnumAsByte<enum ECustomMaterialOutputType> OutputType = ECustomMaterialOutputType::CMOT_Float3;

	//Guarda quais s�o os inputs presentes no c�digo HLSL.
	UPROPERTY()
		TArray<struct FCustomInput> Inputs;

	//Guarda outputs extras manualmente inseridos pelo desenvolvedor. Em breve se tornar� autom�tico como os inputs.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TArray<struct FCustomOutput> AdditionalOutputs;

	//Permite que defini��es sejam criadas. Por exemplo um define com nome PI e valor 3.14 far� com que "#define PI 3.14" seja inserido no c�digo.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TArray<struct FCustomDefine> AdditionalDefines;

	//Permite que mais arquivos sejam anexados no c�digo. �til para importar bibliotecas presentes em outros arquivos.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TArray<FString> IncludeFilePaths;

	//Ponteiro para um Custom node convencional. Importante para a correta compila��o de c�digo HLSL sem que seja necess�rio modificar o c�digo-fonte do motor.
	UPROPERTY()
		UMaterialExpressionCustom* CustomExpressionInstance;

	//Guarda as informa��es das vari�veis de input (tipo e nome).
	UPROPERTY()
		TArray<FVariableInfo> InputsInfo;

	//Guarda as informa��es das vari�veis de output (tipo e nome).
	UPROPERTY()
		TArray<FVariableInfo> OutputsInfo;

	//Guarda o c�digo formatado (estruturado) HLSL.
	UPROPERTY()
		FString CustomHLSLStruct = "";

	//Guarda os �ltimos inputs adicionados no c�digo.
	UPROPERTY()
		FString LastInputs = "";



	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; //Fun��o chamada quando alguma propriedade � alterada em runtime.
	void RebuildOutputs(); //Reconstroi os outputs toda vez que um novo AdditionalOutput � adicionado ou removido.
	

#endif // WITH_EDITOR
	//~ End UObject Interface.

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;	//Compilador. Utiliza o ponteiro do Custom Node convencional para repassar o resultado da compila��o.
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;	//Retorna o t�tulo do n�.
	virtual FText GetCreationName() const override	{	return FText::FromString(TEXT("Custom HLSL"));	}	//Retorna o texto a ser exibido na express�o do material (no editor de materiais).
	virtual const TArray<FExpressionInput*> GetInputs() override;	//Retorna todos os inputs do n� atual.
	virtual FExpressionInput* GetInput(int32 InputIndex) override;	//Dado um index, retorna o input correspondente.
	virtual FName GetInputName(int32 InputIndex) const override;	//Dado um index, retorna o nome do input correspondente.
	virtual uint32 GetInputType(int32 InputIndex) override { return MCT_Unknown; }	//Dado um index, retorna o tipo do input correspondente. Neste caso n�o implementado. N�o � utilizado.
	virtual uint32 GetOutputType(int32 OutputIndex) override;	//Dado um index, retorna o tipo do output.
	virtual bool IsResultMaterialAttributes(int32 OutputIndex) override;	//Dado um index, retorna a informa��o se o tipo de sa�da � "MaterialAttibutes".
	virtual bool GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const override;	//Fun��o padr�o. Gera o HLSL correspondente � express�o.

	virtual void PostLoad() override; //Fun��o chamada ap�s o carregamento do n�. Ser� utilizada futuramente.
	virtual void PostInitProperties() override; //Fun��o chamada ap�s a inicializa��o das propriedades. Utilizado para carregar as configura��es padr�es do n�.

#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface

	//~ Begin custom functions
#if WITH_EDITOR
	TArray<FVariableInfo> ExtractParameters(FString& Code, FString InOrOut);	//Dada uma string com c�digo HLSL, extrai os par�metros de input se InOrOut == "in"; ou extrai os par�metros de output InOrOut == "inout".
	FString GetVariableNamesAsString(const TArray<FVariableInfo>& VariableArray);	//Dado um array com informa��es de vari�veis, retorna uma string com o nome das vari�veis separados por v�rgula.
	FString GetVariableTypeAndNamesAsString(const TArray<FVariableInfo>& VariableArray); //Dado um array com informa��es de vari�veis, retorna uma string com o tipo das vari�veis e seus nomes separados por v�rgula.
	void GenerateHLSLStruct(); //Gera uma estrutura HLSL e a insere na propriedade CustomHLSLStruct.
	void RebuildInputs();	//Inspirado pela fun��o RebuildOuputs, reconstr�i os inputs.

#endif // WITH_EDITOR
	//~ End custom functions

};
