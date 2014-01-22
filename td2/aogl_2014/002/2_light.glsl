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
uniform float Time;

uniform vec3  LightPosition;
uniform vec3  LightDiffuseColor;
uniform vec3  LightSpecColor;
uniform float LightIntensity;

uniform mat4 Projection;
uniform mat4 InverseViewProjection;

out vec4  Color;

vec3 computePointLight(vec3 cameraPos, vec3 fragPos, vec3 fragNormal, vec3 fragColor, float fragSpecFactor, vec3 lightPos, vec3 lightDiffuseColor, vec3 lightSpecColor, float lightIntensity);

void main(void)
{
	vec3 normal = texture(Normal, uv).xyz;
	vec4 material = texture (Material, uv);
		vec3 diffuse = material.xyz;
		float specular = material.w;
	float depth = texture(Depth, uv).x;

	vec2  xy = uv * 2.0 -1.0;
	vec4  wPosition =  vec4(xy, depth * 2.0 -1.0, 1.0) * InverseViewProjection;
	vec3  position = vec3(wPosition/wPosition.w);

	vec3 lightPos = vec3(sin(LightPosition.x + Time) *  10.0, LightPosition.y, cos(LightPosition.z + Time) * 10.0);

	vec3 pointLight = computePointLight(CameraPosition, position, normal, diffuse, specular, lightPos, LightDiffuseColor, LightSpecColor, LightIntensity);
	Color = vec4(pointLight, 1);
	// PointLight
	
}

vec3 computePointLight(vec3 cameraPos, vec3 fragPos, vec3 fragNormal, vec3 fragColor, float fragSpecFactor, vec3 lightPos, vec3 lightDiffuseColor, vec3 lightSpecColor, float lightIntensity)
{
	vec3 n = normalize(fragNormal);
	vec3 l =  lightPos - fragPos;
	vec3 v = fragPos - cameraPos;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float lightDistanceAttenuation = 16.f/pow(length(l), 2);
	return lightDistanceAttenuation * lightDiffuseColor * lightIntensity * (fragColor * n_dot_l + fragSpecFactor * lightSpecColor *  pow(n_dot_h, fragSpecFactor * 100));
}

#endif
