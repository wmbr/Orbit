uniform sampler2D source;
uniform vec4 color;

void main()
{
	gl_FragColor = floor(texture2D(source, gl_TexCoord[0].xy)*255 * 0.98 + color * 0.02 *255)/255;
}
