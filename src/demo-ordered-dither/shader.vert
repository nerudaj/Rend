varying vec4 noiseTexCoord;

void main()
{
    // transform the vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // transform the texture coordinates
    gl_TexCoord[0].w = (gl_Color.x * 256 + gl_Color.y);
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0 * gl_TexCoord[0].w;
	noiseTexCoord = gl_TexCoord[0];

    // forward the vertex color
    gl_FrontColor = vec4(gl_Color.z, gl_Color.z, gl_Color.z, 255);
}