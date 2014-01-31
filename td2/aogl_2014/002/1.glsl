#define M_PI 3.1415926535897932384626433832795

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

uniform vec3 LightPosition2;
uniform float LightIntensity2;
uniform vec3 DiffuseColor2;
uniform vec3 SpecularColor2;
uniform float SpecularFactor2;

uniform float SpotLightExternalAngle;
uniform float SpotLightInternalAngle;

uniform sampler2D Diffuse;
uniform sampler2D Spec;

out vec4 Color;

vec3 computePointLight(vec3 lightpos, vec3 diffuseColor, float lightIntensity, vec3 specColor, float specFactor);
vec3 computeDirectionnalLight(vec3 lightDirection, vec3 diffuseColor, float lightIntensity, vec3 specColor, float specFactor);
vec3 computeSpotLight(vec3 lightpos, vec3 lightDirection, float externeAngle, float interneAngle, vec3 diffuseColor, float lightIntensity, vec3 specColor, float specFactor);

void main(void)
{
	vec3 pointLight1 = computePointLight(LightPosition, DiffuseColor, LightIntensity, SpecularColor, SpecularFactor);
	vec3 pointLight2 = computePointLight(LightPosition2, DiffuseColor2, LightIntensity2, SpecularColor2, SpecularFactor2);
	vec3 directionnalLight = computeDirectionnalLight(vec3(-1, -1, -1), vec3(1, 1, 1), 0.6f, vec3(1.0f, 1.0f, 0), SpecularFactor);
	vec3 spotLight = computeSpotLight(vec3(5, 5, 5), vec3(-1, -1, -1), SpotLightExternalAngle, SpotLightInternalAngle, vec3(1, 1, 1), 1, vec3(1, 1, 1), 100);
	Color = vec4(pointLight1 + pointLight2 + spotLight + directionnalLight, 1.0);
}

vec3 computePointLight(vec3 lightpos, vec3 diffuseColor, float lightIntensity, vec3 specColor, float specFactor)
{
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
	vec3 n = normalize(normal);
	lightpos = lightpos + vec3(sin(Time) *  10.0, 1.0, cos(Time) * 10.0);
	vec3 l =  lightpos - position;
	vec3 v = position - CameraPosition;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float lightDistanceAttenuation = 1.f/pow(length(l), 2);
	return lightDistanceAttenuation * diffuseColor * lightIntensity * (diffuse * n_dot_l + spec * specColor *  pow(n_dot_h, spec * specFactor));
}

vec3 computeDirectionnalLight(vec3 lightDirection, vec3 diffuseColor, float lightIntensity, vec3 specColor, float specFactor)
{
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
	vec3 n = normalize(normal);
	vec3 l =  -normalize(lightDirection);
	vec3 v = position - CameraPosition;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	return diffuseColor * lightIntensity * (diffuse * n_dot_l + spec * specColor *  pow(n_dot_h, spec * specFactor));
}


vec3 computeSpotLight(vec3 lightpos, vec3 lightDirection, float externeAngle, float interneAngle, vec3 diffuseColor, float lightIntensity, vec3 specColor, float specFactor)
{
	vec3 lumiere_surface = normalize(lightpos - position);
	float cosAngle = dot(lumiere_surface, -normalize(lightDirection)) / length(lumiere_surface);

	float attenuation;

	if(cosAngle < cos(externeAngle))
	{
		attenuation = 0.f;
	}
	else if(cosAngle > cos(externeAngle) && cosAngle < cos(interneAngle)) {
		attenuation = pow((cosAngle - cos(externeAngle)) / (cos(interneAngle) - cos(externeAngle)), 4);
	}
	else
	{
		attenuation = 1.f;
	}
	

	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
	vec3 n = normalize(normal);
	//vec3  lightPosition = vec3(sin(Time) *  10.0, 1.0, cos(Time) * 10.0);
	vec3 l =  -normalize(lightDirection);
	vec3 v = position - CameraPosition;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	return attenuation * diffuseColor * lightIntensity * (diffuse * n_dot_l + spec * specColor *  pow(n_dot_h, spec * specFactor));
}

#endif
