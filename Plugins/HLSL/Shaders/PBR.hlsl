float DistributionGGX(float3 N, float3 H, float roughness, float PI)
{
    float a = roughness*roughness; //alpha do GGX
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a*a;
    float denom = (NdotH2 * (a*a - 1.0) + 1.0);
    denom = denom*denom;
    denom = PI * denom;

    return num/denom;
}

float3 FresnelSchlick(float3 F0, float3 V, float3 H)
{
    return F0 + (1.0 - F0) * pow(1.0 - dot(V, H), 5.0);
}


float3 CookTorrance(float3 N, float3 V, float3 L, float3 F0, float Roughness, float PI)
{
    //Cálculo da função de distribuição normal D: quantas facetas alinhadas à N em uma área infinitesimal A próximas a um ponto P?
    float3 H = normalize(L + V); //halfway vector
    float NdotH = dot(N, H);
    float D = DistributionGGX(N, H, Roughness, PI);

    //Cálculo da função de geometria G de Cook Torrance.
    float NdotV = max(0.0, dot(N, V));
    float NdotL = max(0.0, dot(N, L));
    float VdotH = dot(V, H);
    float LdotH = dot(L, H);
    float G = GeometricSmith(N, V, L, Roughness);

    //Cálculo da função fresnel F.
    float F = FresnelSchlick(F0, V, H);

    //Cook-Torrance, cálculo da componente especular.
    float specular = (D * G * F)/(4 * NdotL * NdotV);

    return float3(specular, specular, specular);
}

float3 lambertiandiffuse(float PI, float3 N, float3 L, float3 Color, float3 F0)
{
    float NdotL = max(0.0, dot(N, L));

    return Color * (NdotL/PI);
}

float3 main(in float3 Normal, in float3 CameraVector, in float3 Color, in float Roughness, in float Metallic)
{

    float PI = 3.14159265358979;

    float3 N = normalize(Normal);
    float3 V = normalize(CameraVector);
    float3 L = normalize(View.DirectionalLightDirection);

    float3 F0 = float3(0.04, 0.04, 0.04); //Coeficiente Fresnel padrão para dielétricos em um ângulo de incidência

    //Ainda não há a aplicação de metallic!

    float3 specular = CookTorrance(N, V, L, F0, Roughness, PI);
    float3 diffuse = lambertiandiffuse(PI, N, L, Color, F0);

    //Falta a adição do termo ambient
    return specular + diffuse;

}










float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r*r)/8.0;

    float num = NdotV;
    float denom = NdotV * (1.0-k) + k;

    return num/denom;
}

float GeometricSmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1*ggx2;
}


