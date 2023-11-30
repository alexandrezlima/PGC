//Versão simplificada em construção.
float4 main(in float3 Normal, in float3 CameraVector, inout float3 N, inout float3 V, inout float3 L, inout float3 H)
{

    N = normalize(Normal); //Normal à superfície no ponto P analisado.
    V = normalize(CameraVector); //Vetor de visualização, parte do ponto P analisado ao observador.
    L = normalize(View.DirectionalLightDirection); //Vetor de luz incidente.

    H = normalize(V + L); //Halfway vector, ou vetor intermediário.

    return float4(dot(N, L), dot(N, L), dot(N, L), 1.0);
}