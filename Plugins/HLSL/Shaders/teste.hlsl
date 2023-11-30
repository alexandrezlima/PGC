vec2 hash( vec2 p )
{
    p = vec2(dot(p,vec2(127.1,311.7)),
             dot(p,vec2(269.5,183.3)));
    return frac(sin(p)*18.5453);
}

vec2 voronoi(vec2 x )
{
    vec2 n = floor( x );
    vec2 f = frac( x );

	vec3 m = vec3(8.0, 8.0, 8.0);
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2  g = vec2( float(i), float(j) );
        vec2  o = hash( n + g );
      //vec2  r = g - f + o;
	    vec2  r = g - f + (0.5+0.5*sin(iTime+6.2831*o));
		float d = dot( r, r );
        if( d<m.x )
            m = vec3( d, o );
    }

    return vec2( sqrt(m.x), m.y+m.z );
}

vec4 main(in vec2 fragCoord, in float iTime, in float2 iResolution)
{
    vec2 p = fragCoord.xy;
    
    // computer voronoi patterm
    vec2 c = voronoi( (14.0+6.0*sin(0.2*iTime))*p );

    // colorize
    vec3 col = 0.5 + 0.5*cos( c.y*6.2831 + vec3(0.0,1.0,2.0) );	
    col *= clamp(1.0 - 0.4*c.x*c.x,0.0,1.0);
    col -= (1.0-smoothstep( 0.08, 0.09, c.x));
	
    return vec4( col, 1.0 );
}