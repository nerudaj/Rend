void main()
{
    // transform the vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // transform the texture coordinates
    gl_TexCoord[0].w = (gl_Color.x * 256 + gl_Color.y);
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0 * gl_TexCoord[0].w;

    // forward the vertex color
    gl_FrontColor = gl_Color;
}