uniform sampler2D texture;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy / gl_TexCoord[0].w);


    // multiply it by the color
    gl_FragColor = vec4(gl_Color.z, gl_Color.z, gl_Color.z, 1) * pixel;
}
