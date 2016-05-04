varying float diffuse_intensity;

void main(void) {
	diffuse_intensity = 1.0;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
