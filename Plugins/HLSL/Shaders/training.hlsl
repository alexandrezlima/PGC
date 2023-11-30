

float3 main(in float3 WorldNormal, in float3 LightPos, in float3 Color)
{
    float3 normal = normalize(WorldNormal);
    float NdotL = dot(LightPos, normal);

    float tst = NdotL > 0 ? 1 : 0;

    
    return Color * tst;
}