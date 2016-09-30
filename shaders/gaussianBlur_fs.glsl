uniform sampler2D source;
uniform vec2 direction;

const float radius = 4.0;

/* gaussian, sigma = 2.0 */
const float weights[] = {0.202, 0.180, 0.124, 0.067, 0.028};


void main()
{
	vec2 step = direction / textureSize(source, 0) * radius / 4;
	vec2 tc = gl_TexCoord[0].xy;

	vec4 sum = 
		texture2D(source, tc - 4.0 * step) * weights[4] +
		texture2D(source, tc - 3.0 * step) * weights[3] +
		texture2D(source, tc - 2.0 * step) * weights[2] +
		texture2D(source, tc - 1.0 * step) * weights[1] +
		texture2D(source, tc)              * weights[0] +
		texture2D(source, tc + 1.0 * step) * weights[1] +
		texture2D(source, tc + 2.0 * step) * weights[2] +
		texture2D(source, tc + 3.0 * step) * weights[3] +
		texture2D(source, tc + 4.0 * step) * weights[4];
	
	gl_FragColor =  sum;
}
