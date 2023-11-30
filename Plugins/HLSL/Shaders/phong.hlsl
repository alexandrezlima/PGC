
/*
* ka: constante de reflexão ambiente.
* la: intensidadade de luz para cada componente RGB.
*/
float3 calculateAmbient(float3 ka, float3 la)
{
    return ka*la;
}

/*
* NoL: produto escalar entre a normal n e o vetor de iluminação l.
* kd: constante de reflexão difusa
* ld: intensidade de luz para cada componente RGB.
*/
float3 calculateDiffuse(float NoL, float3 kd, float3 ld)
{
    return kd*ld*NoL;
}

/*
* NoL: produto escalar entre a normal n e o vetor de iluminação l.
* N: normal à superfície
* L: vetor de iluminação l.
* V: vetor de visualização v.
* ks: constante de reflexão especular.
* ls: intensidade de luz para cada componente RGB.
* alpha: constante para o controle do brilho da reflexão especular.
*/
float3 calculateSpecular(float NoL, float3 N, float3 L, float3 V, float3 ks, float3 ls, float alpha)
{
    float3 r = reflect(-L, N);
    return ks*ls*pow(max(dot(V, r), 0), alpha);//*step(0, NoL);
}

float3 main(in float3 BaseColor, in float3 Normal, in float3 CameraVector, in float3 ka, in float3 kd, in float3 ks, in float alpha, inout float3 ambient, inout float3 diffuse, inout float3 specular, inout float3 r)
{
    float3 N = normalize(Normal); //Normal à superfície no ponto P.
    float3 L = normalize(View.DirectionalLightDirection); //Vetor de iluminação l.
    float3 V = normalize(CameraVector); //Vetor de visualização v.
    
    float NoL = max(dot(N, L), 0.0); //Produto escalar entre n e l.

    ambient = calculateAmbient(1, ka); //Calcula a contribuição da reflexão ambiente.
    diffuse = calculateDiffuse(NoL, kd, 1); //Calcula a contribuição da reflexão difusa.
    specular = calculateSpecular(NoL, N, L, V, ks, 1, alpha); //Calcula a contribuição da reflexão especular.

    return (ambient + diffuse + specular)*BaseColor;
}
//Exemplo estudado e aplicado a partir do site da disciplina de Computação Gráfica, disponível em: https://hbatagelo.github.io/cg/viewer3.html