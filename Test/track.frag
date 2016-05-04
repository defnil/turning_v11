uniform sampler2D dirt_map;
uniform sampler2D grass_map;
uniform sampler2D track_mask;
uniform float dirt_scale;
uniform float grass_scale;
uniform float mask_scale;


varying float diffuse_intensity;

void main(void) {
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_TexCoord[0];
	vec4 dirt = texture2D(dirt_map, dirt_scale*gl_TexCoord[0].xy);
	vec4 grass = texture2D(grass_map, grass_scale*gl_TexCoord[0].xy);
	vec4 mask = texture2D(track_mask, mask_scale*gl_TexCoord[0].xy);
	gl_FragColor = diffuse_intensity * mix(grass, dirt, mask.r);
}