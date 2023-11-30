
float3 calculateAmbient(float ka, float la)
{
    float ia = ka*la;
    return float3(ia, ia, ia);
}

float3 calculateDiffuse(float3 N, float3 L, float kd, float ld, float lambertian)
{
    float id = kd*ld*lambertian;

    return float3(id, id, id);
}

float calculateSpecular(float3 N, float3 L, float3 V, float ks, float ls, float alpha)
{
    float3 r = normalize(2*(dot(N, L))*N - L);

    float is = ks*ls*pow(dot(r, V), alpha);

    return is;

}

float3 main(in float3 Normal, in float3 CameraVector, in float3 WorldPos, in Texture2D texture, inout float3 ambient, inout float3 diffuse, inout float3 specular, inout float3 r, inout float4 mm)
{
    float3 N = normalize(Normal); //Parâmetro já no espaço da câmera.
    float3 V = normalize(CameraVector); //Parâmetro já no espaço da câmera.


    ambient = calculateAmbient(1, 0.1); //Calcula a contribuição da reflexão ambiente. Neste caso, utilizamos a constante de reflexão do ambiente = 1 e a intensidade de luz ambiente = 0.1.

    float3 sum = float3(0,0,0);

    //Capturando informações da textura que contém os vetores de iluminação.
    uint k = texture[uint2(0,0)].r;

    specular = float3(0.0, 0.0, 0.0);
    diffuse = float3(0.0, 0.0, 0.0);

    mm = texture[uint2(1,0)].rgba;

    for (int i = 1; i <= k; i++)
    {
        //Calcula o vetor de iluminação. Precisa de adaptações para os diferentes tipos de luz. A ideia seria utilizar o último valor (w) do vetor 4D para guardar o tipo de luz.
        float3 L = normalize(texture[uint2(i,0)].rgb - WorldPos);
        float lambertian = max(dot(N, L), 0.0);
        float is = 0.0;

        if (lambertian > 0.0)
        {
            r = normalize(2*dot(N,L)*N - L);
            
            float angle = max(dot(r, V), 0.0);
            is = pow(angle, 100);
        }

        sum += calculateDiffuse(N, L, 0.7, 1, lambertian);

        sum += float3(is, is, is);
    }


    return (ambient + sum);
}

//Exemplo estudado e aplicado a partir do site da disciplina de Computação Gráfica, disponível em: https://hbatagelo.github.io/cg/viewer3.html


