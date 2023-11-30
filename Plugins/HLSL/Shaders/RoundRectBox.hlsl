struct RoundedBox
{
    //Round rect box. Teste de shader para interface gráfica.
    float4 RoundRectBoxShader(in float2 UV, in float2 BoxDimensions, in float4 CornerRadii, in float2 Center, in float Sharpness)
    {
        float CornerRadius = 0;
        if (UV.x < 0.5)
            CornerRadius = 1- BoxDimensions.x;
        else
            CornerRadius = 1 - BoxDimensions.y;

        float2 absolute = abs(UV - Center);
        float2 dimensions = (BoxDimensions + CornerRadius)*0.5;
        float2 sub = max(absolute - dimensions, 0);


        float4 RoundRect = 1 - clamp((length(sub)/CornerRadius), 0, 1);

        return saturate((Sharpness+1)*RoundRect);
    }
};

RoundedBox R;
return R.RoundRectBoxShader(UV, BoxDimensions, CornerRadius, Center, Sharpness);