uniform sampler2D texture;
uniform sampler2D noise;
uniform vec4 shadeColor;

varying vec4 noiseTexCoord;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy / gl_TexCoord[0].w);

    vec4 noise = texture2D(noise, noiseTexCoord.xy / gl_TexCoord[0].w);

    // gl_Color is unused
	if (gl_Color.x < noise.x)
		gl_FragColor = shadeColor;
	else
		gl_FragColor = pixel;
}