// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomHLSL.h"
#include "MaterialCompiler.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"


#if WITH_EDITOR
#include "MaterialGraph/MaterialGraphNode.h"
#endif

//Abre o shader que atualmente está inserido no campo "HLSL".
void UCustomHLSL::OpenShader()
{
	FString OpenShaderFilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectDir()) + TEXT("Plugins/HLSL/Shaders/" + HLSL);

	if (FPaths::FileExists(OpenShaderFilePath))
		FPlatformProcess::LaunchFileInDefaultExternalApplication(*OpenShaderFilePath);

}

//Abre a pasta Shaders do plugin.
void UCustomHLSL::OpenShadersFolder()
{
	FString OpenShaderFilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectDir()) + TEXT("Plugins/HLSL/Shaders/");
	FPlatformProcess::ExploreFolder(*OpenShaderFilePath);
}

//Este método é chamado quando alguma propriedade desta classe é alterada. Segue uma estrutura padrão para UMaterialExpression.
void UCustomHLSL::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	if (PropertyThatChanged && PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(FCustomInput, InputName))
	{
		for (FCustomInput& Input : Inputs)
		{
			FString InputName = Input.InputName.ToString();
			if (InputName.ReplaceInline(TEXT(" "), TEXT("")) > 0)
			{
				Input.InputName = *InputName;
			}
		}
	}

	RebuildOutputs();

	if (PropertyChangedEvent.MemberProperty && GraphNode)
	{
		const FName PropertyName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UCustomHLSL, Inputs) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UCustomHLSL, AdditionalOutputs))
		{
			GraphNode->ReconstructNode();
		}

		//Verifica a necessidade de recompilação.
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UCustomHLSL, Recompile) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UCustomHLSL, HLSL))
		{
			if (Recompile)
			{
				Recompile = false;
			}

			if (!HLSL.Equals(LastHLSLCode))
			{
				LastHLSLCode = HLSL;
				InputsInfo.Reset(0);
				RebuildOutputs();
			}

			GenerateHLSLStruct(); //Gera um novo código HLSL.
			
			UE_LOG(LogTemp, Warning, TEXT("Node reconstruction done!"));
		}

	}

	GraphNode->ReconstructNode();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

//Reconstroi os nós de output.
void UCustomHLSL::RebuildOutputs()
{
	Outputs.Reset(AdditionalOutputs.Num() + 1);
	if (AdditionalOutputs.Num() == 0)
	{
		bShowOutputNameOnPin = true;
		Outputs.Add(FExpressionOutput(TEXT("main return")));
	}
	else
	{
		bShowOutputNameOnPin = true;
		Outputs.Add(FExpressionOutput(TEXT("main return")));
		for (const FCustomOutput& CustomOutput : AdditionalOutputs)
		{
			if (!CustomOutput.OutputName.IsNone())
			{
				Outputs.Add(FExpressionOutput(CustomOutput.OutputName));
			}
		}
	}
}

//Reconstroi os nós de input.
void UCustomHLSL::RebuildInputs()
{
	Inputs.Reset(InputsInfo.Num());

	for (const FVariableInfo& Input : InputsInfo)
	{
		FExpressionInput NewInput;

		NewInput.InputName = FName(Input.varName.Replace(TEXT("_in"), TEXT("")));
		FCustomInput CustomInput;
		CustomInput.InputName = FName(Input.varName);
		CustomInput.Input = NewInput;

		Inputs.Add(FCustomInput(CustomInput));
	}
}

//Utiliza expressões regulares para encontrar as variáveis de input e output.
TArray<FVariableInfo> UCustomHLSL::ExtractParameters(FString& Code, FString InOrOut)
{
	TArray<FVariableInfo> InputVariables;

	//Define o padrão regex para a correspondência dos parâmetros de entrada da função main.
	FRegexPattern ParamPattern(InOrOut + TEXT("\\s+(\\w+)\\s+(\\w+)(,)?"));

	//Cria um objeto FRegexMatcher para a string de código e o padrão regex.
	FRegexMatcher Matcher(ParamPattern, Code);

	bool test = true;
	int32 b = -1;
	int32 e = -1;


	//Realiza a correspondência em todas as linhas da string do código.
	while (Matcher.FindNext())
	{
		FVariableInfo Info;
		Info.varType = Matcher.GetCaptureGroup(1);
		Info.varName = Matcher.GetCaptureGroup(2);

		if (InOrOut.Equals("in"))
			Info.varName += "_in";

		if (test)
		{
			b = Matcher.GetMatchBeginning();
			test = false;
		}

		e = Matcher.GetMatchEnding();
		

		InputVariables.Add(Info);
	}

	if (InOrOut.Equals("in"))
	{
		FString VariablesString = GetVariableTypeAndNamesAsString(InputVariables);

		FString replacement = Code.Mid(b, e - b);

		if (replacement.EndsWith(TEXT(",")))
			replacement = replacement.Mid(0, replacement.Len() - 1);

		UE_LOG(LogTemp, Warning, TEXT("REPLACEMENT: %i %i %s"), b, e, *replacement);
		Code.ReplaceInline(*replacement, *VariablesString);
	}	

	return InputVariables;
}

//Reune todas as variáveis passadas como parâmetro em uma única string (apenas seus nomes).
FString UCustomHLSL::GetVariableNamesAsString(const TArray<FVariableInfo>& VariableArray)
{
	FString VariableNamesString = "";

	for (const FVariableInfo& Info : VariableArray)
	{
		if (!VariableNamesString.IsEmpty())
		{
			VariableNamesString.Append(TEXT(", "));
		}

		VariableNamesString.Append(Info.varName);
	}

	return VariableNamesString;
}

//Reúne em uma única string todas as variáveis passadas como parâmetro e seus respectivos tipos.
FString UCustomHLSL::GetVariableTypeAndNamesAsString(const TArray<FVariableInfo>& VariableArray)
{
	FString VariablesString = "";

	for (const FVariableInfo& Info : VariableArray)
	{
		if (!VariablesString.IsEmpty())
		{
			VariablesString.Append(TEXT(", "));
		}
		VariablesString.Append("in " + Info.varType + " " + Info.varName);
	}

	return VariablesString;
}

//Gera uma estrutura hlsl a partir do código escrito em um arquivo externo.
void UCustomHLSL::GenerateHLSLStruct()
{
	//Pega o caminho da pasta shaders, que fica dentro da pasta deste plugin.
	FString FullPath = FPaths::ProjectDir() + TEXT("Plugins/HLSL/Shaders/" + HLSL);

	//Caso o arquivo exista...
	if (FPaths::FileExists(FullPath))
	{
		//Lê o arquivo.
		FString FileContent;
		FFileHelper::LoadFileToString(FileContent, *FullPath);

		//Para cada tipo "vec", substituímos pelo float correspondente. Uma tabela será construída para ajustar as equivalências com HLSL antigo.
		FileContent = FileContent.Replace(TEXT("vec2"), TEXT("float2"));
		FileContent = FileContent.Replace(TEXT("vec3"), TEXT("float3"));
		FileContent = FileContent.Replace(TEXT("vec4"), TEXT("float4"));

		//Dado o código HLSL, extrai os inputs.
		TArray<FVariableInfo> ExtractedInputs = ExtractParameters(FileContent, TEXT("in"));

		/*
		* Passo para transformar as variáveis de input em variáveis globais.
		* Para isso, declara uma variável global homônima e a seta no inicio da função main com valor correspondente ao input respectivo.
		* 
		* Ideia:
		* 
		*			float2 global; <- adiciona a declaração de variável global no início do documento.
		*			
		*			float3 main(in float2 global_in) <- adiciona um "_in" ao parâmetro de entrada.
		*			{
		*				global = global_in; <- usando o parâmetro de entrada, seta a variável global correspondente.
		*			} 
		*/
		FString GlobalVariables = "\n";
		int32 Index = 0;
		for (const FVariableInfo& Info : ExtractedInputs)
		{
			FString VarName = Info.varName;
			VarName.ReplaceInline(TEXT("_in"), TEXT(""));


			FString VariableDeclaration = Info.varType + " " + VarName + ";\n";
			FileContent = VariableDeclaration.Append(FileContent);

			GlobalVariables += "\t" + VarName + " = " + Info.varName + ";\n";

			Index++;
		}



		/*
		* Utiliza expressões regulares para encontrar os inputs presentes após a abertura do parêntesis da função main.
		* Identifica, portanto, onde começa a função main para que as variáveis globais sejam setadas.
		*/
		FRegexPattern MainFunctionPattern(TEXT("main\\s*\\((.*?)\\)\\s*\\{"));
		FRegexMatcher Matcher(MainFunctionPattern, FileContent);
		if (Matcher.FindNext())
		{
			FString MainFunctionPart = Matcher.GetCaptureGroup(0);
			FString replacement = MainFunctionPart + GlobalVariables;

			FileContent = FileContent.Replace(*MainFunctionPart, *replacement);
		}


		InputsInfo = ExtractedInputs;
		FString InputVariables = GetVariableNamesAsString(InputsInfo);
		FString OutputVariables = GetVariableNamesAsString(ExtractParameters(FileContent, TEXT("inout")));

		/*
		* Monta a estrutura básica do nó, dado por algo como:
		* 
		*		struct Code {
		*			//código...
		*			
		*			... main(parameters) {
		*					(...)
		*			}
		*		};
		* 
		*		Code c;
		*		return c.main(parameters);
		*/
		CustomHLSLStruct = "struct Code {\n" + FileContent + "\n};\nCode c;\nreturn c.main(" + InputVariables + (OutputVariables.IsEmpty() ? "" : ", " + OutputVariables) + ");" + "\nreturn 0;";

		//Imprime no console a estrutura criada.
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Text From File: %s"), *CustomHLSLStruct);

		//Caso algum input seja distindo da compilação anterior, reconstroi os inputs.
		if (!LastInputs.Equals(InputVariables))
		{
			LastInputs = InputVariables;
			RebuildInputs();
		}
		
	}
}

//Utiliza a compilação do próprio Custom node convencional como retorno. Apenas informamos os inputs, outputs e o código formatado.
int32 UCustomHLSL::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	CustomExpressionInstance = NewObject<UMaterialExpressionCustom>(this);
	CustomExpressionInstance->Code = CustomHLSLStruct;
	CustomExpressionInstance->OutputType = OutputType;
	CustomExpressionInstance->Description = Title;
	CustomExpressionInstance->Desc = Desc;
	CustomExpressionInstance->Inputs = Inputs;
	CustomExpressionInstance->AdditionalOutputs = AdditionalOutputs;
	CustomExpressionInstance->AdditionalDefines = AdditionalDefines;
	CustomExpressionInstance->IncludeFilePaths = IncludeFilePaths;

	return CustomExpressionInstance->Compile(Compiler, OutputIndex);
}

//Retorna o título do nó.
void UCustomHLSL::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(Title);
}

//Retorna a lista de inputs presentes no código HLSL.
const TArray<FExpressionInput*> UCustomHLSL::GetInputs()
{
	TArray<FExpressionInput*> Result;
	for (int32 i = 0; i < Inputs.Num(); i++)
	{
		Result.Add(&Inputs[i].Input);
	}
	return Result;
}

//Dado um index, devolve o input correspondente.
FExpressionInput* UCustomHLSL::GetInput(int32 InputIndex)
{
	if (InputIndex < Inputs.Num())
		return &Inputs[InputIndex].Input;

	return nullptr;
}


//Dado um index, devolve o nome do input correspondente.
FName UCustomHLSL::GetInputName(int32 InputIndex) const
{
	if (InputIndex < Inputs.Num())
		return Inputs[InputIndex].InputName;

	return NAME_None;
}

//Dado um index, devolve o tipo de saída correspondente.
uint32 UCustomHLSL::GetOutputType(int32 OutputIndex)
{
	ECustomMaterialOutputType Type = CMOT_MAX;
	if (OutputIndex == 0)
	{
		Type = OutputType;
	}
	else if (OutputIndex >= 1 && OutputIndex - 1 < AdditionalOutputs.Num())
	{
		Type = AdditionalOutputs[OutputIndex - 1].OutputType;
	}

	switch (Type)
	{
	case CMOT_Float1:
		return MCT_Float;
	case CMOT_Float2:
		return MCT_Float2;
	case CMOT_Float3:
		return MCT_Float3;
	case CMOT_Float4:
		return MCT_Float4;
	case CMOT_MaterialAttributes:
		return MCT_MaterialAttributes;
	default:
		return MCT_Unknown;
	}
}

//Retorna a informação que diz se o index passado pertence ao tipo "MCT_MaterialAttributes".
bool UCustomHLSL::IsResultMaterialAttributes(int32 OutputIndex)
{
	return GetOutputType(OutputIndex) == MCT_MaterialAttributes;
}

//Função interna. Dado a estrutura hlsl gerada, passa para o compilador hlsl interno do motor.
bool UCustomHLSL::GenerateHLSLExpression(FMaterialHLSLGenerator& Generator, UE::HLSLTree::FScope& Scope, int32 OutputIndex, UE::HLSLTree::FExpression const*& OutExpression) const
{
	return CustomExpressionInstance->GenerateHLSLExpression(Generator, Scope, OutputIndex, OutExpression);
}

//Função chamada ao carregar o nó.
void UCustomHLSL::PostLoad()
{
	Super::PostLoad();
}

//Função chamada após inicializar as propriedades do nó.
void UCustomHLSL::PostInitProperties()
{
	Super::PostInitProperties();
	bCollapsed = false;
	RebuildOutputs();
	GenerateHLSLStruct();
}