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
* Estrutura que guarda o nome e o tipo de uma dada variável.
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
	* As variáveis cuja propriedade é "EditAnywhere" podem ser editadas diretamente no painel
	* de detalhes do nó, dentro do editor de materiais.
	* 
	* Variáveis que não possuem esta propriedade são utilizadas internamente no código, sem que
	* o desenvolvedor tenha acesso direto.
	*/

	//Título dado ao nó. Alterar o valor desta variável altera o título do nó no editor de materiais.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		FString Title = "Custom HLSL";

	//Nome do shader que será carregado pelo nó. O arquivo deve estar na pasta Plugins/HLSL/Shaders.
	UPROPERTY(EditAnywhere, Category = "HLSL", meta = (MultiLine = true))
		FString HLSL = "default.hlsl";

	//Abre o shader que atualmente está inserido no campo "HLSL".
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "HLSL")
		void OpenShader();
	
	//Abre a pasta Shaders do plugin.
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "HLSL")
		void OpenShadersFolder();

	//Armazena o último código HLSL compilado. Útil para buscar se houveram modificações no código em relação ao último update.
	UPROPERTY()
		FString LastHLSLCode = "default.hlsl";

	//Boolean que força a recompilação do nó. Alterar este valor chama a função PostEditChangeProperty, a qual reconstrói o nó.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		bool Recompile = false;

	//Define o tipo de saída da função principal do código HLSL. Em breve será setada automaticamente.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TEnumAsByte<enum ECustomMaterialOutputType> OutputType = ECustomMaterialOutputType::CMOT_Float3;

	//Guarda quais são os inputs presentes no código HLSL.
	UPROPERTY()
		TArray<struct FCustomInput> Inputs;

	//Guarda outputs extras manualmente inseridos pelo desenvolvedor. Em breve se tornará automático como os inputs.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TArray<struct FCustomOutput> AdditionalOutputs;

	//Permite que definições sejam criadas. Por exemplo um define com nome PI e valor 3.14 fará com que "#define PI 3.14" seja inserido no código.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TArray<struct FCustomDefine> AdditionalDefines;

	//Permite que mais arquivos sejam anexados no código. Útil para importar bibliotecas presentes em outros arquivos.
	UPROPERTY(EditAnywhere, Category = "HLSL")
		TArray<FString> IncludeFilePaths;

	//Ponteiro para um Custom node convencional. Importante para a correta compilação de código HLSL sem que seja necessário modificar o código-fonte do motor.
	UPROPERTY()
		UMaterialExpressionCustom* CustomExpressionInstance;

	//Guarda as informações das variáveis de input (tipo e nome).
	UPROPERTY()
		TArray<FVariableInfo> InputsInfo;

	//Guarda as informações das variáveis de output (tipo e nome).
	UPROPERTY()
		TArray<FVariableInfo> OutputsInfo;

	//Guarda o código formatado (estruturado) HLSL.
	UPROPERTY()
		FString CustomHLSLStruct = "";

	//Guarda os últimos inputs adicionados no código.
	UPROPERTY()
		FString LastInputs = "";



	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; //Função chamada quando alguma propriedade é alterada em runtime.
	void RebuildOutputs(); //Reconstroi os outputs toda vez que um novo AdditionalOutput é adicionado ou removido.
	

#endif // WITH_EDITOR
	//~ End UObject Interface.

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;	//Compilador. Utiliza o ponteiro do Custom Node convencional para repassar o resultado da compilação.
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;	//Retorna o título do nó.
	virtual FText GetCreationName() const override	{	return FText::FromString(TEXT("Custom HLSL"));	}	//Retorna o texto a ser exibido na expressão do material (no editor de materiais).
	virtual const TArray<FExpressionInput*> GetInputs() override;	//Retorna todos os inputs do nó atual.
	virtual FExpressionInput* GetInput(int32 InputIndex) override;	//Dado um index, retorna o input correspondente.
	virtual FName GetInputName(int32 InputIndex) const override;	//Dado um index, retorna o nome do input correspondente.
	virtual uint32 GetInputType(int32 InputIndex) override { return MCT_Unknown; }	//Dado um index, retorna o tipo do input correspondente. Neste caso não implementado. Não é utilizado.
	virtual uint32 GetOutputType(int32 OutputIndex) override;	//Dado um index, retorna o tipo do output.
	virtual bool IsResultMaterialAttributes(int32 OutputIndex) override;	//Dado um index, retorna a informação se o tipo de saída é "MaterialAttibutes".
	virtual bool GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const override;	//Função padrão. Gera o HLSL correspondente à expressão.

	virtual void PostLoad() override; //Função chamada após o carregamento do nó. Será utilizada futuramente.
	virtual void PostInitProperties() override; //Função chamada após a inicialização das propriedades. Utilizado para carregar as configurações padrões do nó.

#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface

	//~ Begin custom functions
#if WITH_EDITOR
	TArray<FVariableInfo> ExtractParameters(FString& Code, FString InOrOut);	//Dada uma string com código HLSL, extrai os parâmetros de input se InOrOut == "in"; ou extrai os parâmetros de output InOrOut == "inout".
	FString GetVariableNamesAsString(const TArray<FVariableInfo>& VariableArray);	//Dado um array com informações de variáveis, retorna uma string com o nome das variáveis separados por vírgula.
	FString GetVariableTypeAndNamesAsString(const TArray<FVariableInfo>& VariableArray); //Dado um array com informações de variáveis, retorna uma string com o tipo das variáveis e seus nomes separados por vírgula.
	void GenerateHLSLStruct(); //Gera uma estrutura HLSL e a insere na propriedade CustomHLSLStruct.
	void RebuildInputs();	//Inspirado pela função RebuildOuputs, reconstrói os inputs.

#endif // WITH_EDITOR
	//~ End custom functions

};
