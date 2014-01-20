#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0));
	position = vec3(Object * vec4(VertexPosition, 1.0));
	gl_Position = Projection * View * Object * vec4(VertexPosition, 1.0);
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;
uniform float Time;

in vec2 uv;
in vec3 position;
in vec3 normal;

uniform vec3 LightPosition;
uniform float LightIntensity;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform float SpecularFactor;


uniform sampler2D Diffuse;
uniform sampler2D Spec;

out vec4 Color;

void main(void)
{
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
	vec3 n = normalize(normal);

	vec3  lightColor = vec3(1.0, 1.0, 1.0);
	//vec3  lightPosition = vec3(sin(Time) *  10.0, 1.0, cos(Time) * 10.0);

	vec3 l =  LightPosition - position;
	vec3 v = position - CameraPosition;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);

	float lightDistanceAttenuation = 1.f/pow(length(l), 2);

	vec3 color = lightDistanceAttenuation * DiffuseColor * LightIntensity * (diffuse * n_dot_l + spec * SpecularColor *  pow(n_dot_h, spec * SpecularFactor));

	Color = vec4(color, 1.0);
}
#endif
