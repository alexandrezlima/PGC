#define PI 3.14159265358979

/*
* Função Fresnel utilizando a aproximação de Schlick, discutida no capítulo 3, seção 3.2.3.1.
* É calculada como:
*   F0 + (1 - F0)*(1-V.H)^5
* V pode ser substituído neste caso por L, uma vez que o ângulo entre H e V é o mesmo entre
* H e L (pois H é o vetor intermediário entre eles).
*/
float3 FresnelSchlick(float3 V, float3 H, float3 F0)
{
    float NoV = dot(V, H);
    return F0 + (float3(1, 1, 1) - F0)*pow((1-NoV), 5);
}

/*
* Função de Distribuição GGX, discutida no capítulo 3, seção 3.2.3.2.
* É calculada como:
*   alpha^2/(pi*((N.H)^2 * (alpha^2 - 1) + 1)^2)
* Onde alpha = roughness^2.
*/
float DistributionGGX(float3 N, float3 H, float alpha)
{
    float alpha2 = alpha*alpha; //alpha^2.
    float NoH = saturate(dot(N, H)); //Produto escalar N.H.

    //Construção do denominador por etapas, para maior eficiência computacional.
    float denominador = NoH*NoH*(alpha2 - 1) + 1; //(N.H)^2 * (Alpha^2-1)+1

    //Em vez de checar se NoH*NoH*(alpha2 - 1) = -1, verificou-se se o denominador vale 0.
    //Como isso só pode ocorrer se alpha = 0, então se
    return alpha2/(PI * denominador * denominador + step(denominador, 0)*step(0, denominador));
}

/*
* Método de Smith para função geométrica: multiplica duas funções geométricas iguais, uma
* para L e outra para V (sombreamento e obstrução geométrica, respectivamente).
* Dada uma função genérica G1, o método de Smith é calculado como:
*   G1(n,l)*G1(n,v)
*/
float GeometricSmith(float3 N, float3 L, float3 V, float alpha)
{
    return GeometricGGX(N, L, alpha) * GeometricGGX(N, V, alpha);
}

/*
* Função Geométrica GGX, discutida no capítulo 3.2.3.3.
* É calculada como:
*   2*(N.G)/((N.G + sqrt(alpha^2 + (1-alpha^2)*(N.G)^2)))
*/
float GeometricGGX(float3 N, float3 G, float alpha)
{
    float NoG = saturate(dot(N, G));
    float alpha2 = alpha*alpha;
    float denominador = NoG + sqrt(alpha2 + (1 - alpha2) * NoG * NoG);

    return (2*NoG)/denominador;
}

/*
* Termo difuso da BRDF. Utiliza o modelo Lambertiano discutido na seção 3.2.3.
* É calculado como:
*   (BaseColor/PI) * (n.l) * L(P, l)
*/
float3 Lambertian(float3 C)
{
    return C/PI;
}

/*
* Parâmetros:
*   ViewDir: Vetor de visualização.
*   LightDir: Vetor de iluminação. 
*   Normal: Vetor normal à superfície.
*   BaseColor: Cor do ponto P analisado.
*   Metallic: Define o quão metálico é o objeto no ponto P. Varia no intervalo real [0,1].
*   Roughness: Define a rugosidade da superfície no ponto P. Varia no intervalo real [0,1].
*/
float3 main(in float3 ViewDir, in float3 LightDir, in float3 Normal, in float3 BaseColor, in float Metallic, in float Roughness, inout float3 Fresnel, inout float Distribution, inout float3 Geometric, inout float3 Lambert, inout float3 DiffuseTerm, inout float3 SpecularTerm)
{
    float3 V = normalize(ViewDir);  //Vetor de visualização v.
    float3 L = normalize(LightDir); //Vetor de iluminação l.
    float3 N = normalize(Normal);   //Normal à superfície n.

    float3 H = normalize(V+L);  //Vetor intermediário h.

    float NoL = saturate(dot(N, L));
    float NoV = saturate(dot(N, V));

    Roughness = clamp(Roughness, 0.01, 1); //Limita para que a reflexão seja visível.
    float alpha = Roughness*Roughness; //Parâmetro alpha para as funções D e G. Utiliza o mapeamento apresentado por Burley.

    //Ajusta o valor de F0 para metais e dielétricos com uma interpolação linear usando o parâmetro Metallic=[0,1].
    float3 F0 = lerp(float3(0.04,0.04,0.04), BaseColor, Metallic);

    //Cálculo da função Fresnel F.
    float3 F = FresnelSchlick(V, H, F0);    
    Fresnel = F;    //Salva o valor fresnel para o output "Fresnel".

    //Cálculo da função de Distribuição D.
    float D = DistributionGGX(N, H, alpha);
    Distribution = D;   //Salva o valor de distribuição para o output "Distribution".

    //Cálculo da função Geométrica G.
    float G = GeometricSmith(N, L, V, alpha);
    Geometric = G;  //Salva o valor de geometria para o output "Geometric".

    //Cálculo do termo especular da BRDF Cook-Torrance.
    float3 Specular = (F*D*G)*NoL/(4*NoL*NoV+0.0001); //0.001 é adicionado para evitar a divisão por zero.
    SpecularTerm = Specular;    //Salva o valor do termo especular para o output "SpecularTerm".

    //Cálculo da função Lambertiana.
    float3 f_d = Lambertian(BaseColor);
    Lambert = f_d; //Salva o valor da função lambertiana para o output "Lambert".

    //Cálculo do termo difuso da BRDF Cook-Torrance.
    float kd = 1 - F; //Ks = 1 - Ks. Como F retorna a proporção de luz refletida (logo, é o Ks), então 1-F retorna a proporção difratada.
    float3 Diffuse = kd*f_d*NoL;
    DiffuseTerm = Diffuse;  //Salva o valor do termo difuso para o output "DiffuseTerm".

    return Diffuse + Specular;
}