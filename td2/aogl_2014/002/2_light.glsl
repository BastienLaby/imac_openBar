#if defined(VERTEX)

in vec2 VertexPosition;

out vec2 uv;

void main(void)
{	
	uv = VertexPosition * 0.5 + 0.5;
	gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)

in vec2 uv;

uniform sampler2D Material;
uniform sampler2D Normal;
uniform sampler2D Depth;
uniform vec3 CameraPosition;
uniform vec3  LightPosition;
uniform vec3  LightColor;
uniform float LightIntensity;
uniform mat4 InverseViewProjection;

out vec4  Color;

void main(void)
{
	Color = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif
