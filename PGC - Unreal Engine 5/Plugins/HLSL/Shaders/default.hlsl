struct test {
    float x;
    float y;
    float z;
};

float3 auxFunction()
{
    test k;
    k.x = 0.15;
    k.y = 0.25;
    k.z = t;

    return float3(k.x, k.y, k.z);
}

float3 main(in float t, in float t2, inout float3 testout)
{
    testout = vec3(0.5,0.5,0);

    return auxFunction();
}