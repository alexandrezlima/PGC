float3 main(in float3 WorldPos, in float3 Normal, in float3 CameraVector, in float3 ToonColor, in float3 AmbientColor, in float3 SpecularColor, in float Shininess)
{
    float3 N = normalize(Normal);
    float3 L = normalize(View.DirectionalLightDirection);
    float3 V = normalize(CameraVector);

    float NdotL = dot(N, L);

    float3 R = normalize((2.0 * NdotL * N) - L); //reflection vector

    float LightIntensity = smoothstep(0, 0.01, NdotL);

    float3 H = normalize(L + V);
    float NdotH = dot(N, H);

    float SpecularIntensity = pow(NdotH * LightIntensity, Shininess * Shininess);


    return ToonColor * (LightIntensity + AmbientColor + SpecularIntensity);
}

//Versão de toon shading adaptada de https://roystan.net/articles/toon-shader/