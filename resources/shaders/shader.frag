uniform sampler2D texture;
uniform sampler2D noise;
uniform vec4 shadeColor;
uniform vec2 resolution;
uniform bool ditherShadows;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy / gl_TexCoord[0].w);

    vec2 noisePos = vec2(
        gl_FragCoord.x / resolution.x,
        gl_FragCoord.y / resolution.y);
    vec4 noisePixel = texture2D(noise, noisePos);

    bool dim = gl_Color.x < noisePixel.x && pixel.w > 0.;
    vec4 ditheredPixel = float(dim) * shadeColor + float(!dim) * pixel;
    vec4 shadedPixel = gl_Color * pixel;
    gl_FragColor = float(ditherShadows) * ditheredPixel + float(!ditherShadows) * shadedPixel;
}
