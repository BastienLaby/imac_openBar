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
	normal = vec3(Object * vec4(VertexNormal, 1.0));; 
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

uniform sampler2D Diffuse;
uniform sampler2D Spec;
uniform float Intensity;

out vec4 Color;

vec3 pointLight(in vec3 lcolor, in float intensity, in vec3 lpos, in vec3 n, in vec3 fpos, vec3 diffuse, float spec, vec3 cpos)
{
	vec3 l =  lpos - fpos;
	vec3 v = fpos - cpos;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float d = distance(l, position);
	float att = clamp(  1.0 /  (d*d) , 0.0, 1.0);
	vec3 color = lcolor * intensity * att * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));
	return color;
}

vec3 directionalLight(in vec3 lcolor, in float intensity, in vec3 ldir, in vec3 n, in vec3 fpos, vec3 diffuse, float spec, vec3 cpos)
{
	vec3 l =  ldir;
	vec3 v = fpos - cpos;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, -l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float d = distance(l, position);
	vec3 color = lcolor * intensity * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));
	return color;
}

vec3 spotLight(in vec3 lcolor, in float intensity, in vec3 ldir, in vec3 lpos, in vec3 n, in vec3 fpos, vec3 diffuse, float spec, vec3 cpos)
{
	vec3 l =  lpos - fpos;
	float cosTheta = dot( normalize(-l), normalize(ldir) ); 
	float cosTotal = cos(radians(30.0));      
	float cosFall = cos(radians(10.0));     
	float falloff; 
	if (cosTheta < cosTotal) 
	{ 
		falloff = 0.0;
	}
	else if (cosTheta > cosFall) 
	{
		falloff = 1.0;
	}
	else
	{
		falloff = (cosTheta - cosTotal) / (cosFall - cosTotal);
		falloff = falloff * falloff* falloff*falloff;
	}

	vec3 v = fpos - cpos;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);
	float d = distance(l, position);
	vec3 color = vec3(0.0, 0.0, 0.0);

	return falloff * lcolor * intensity * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));
}

void main(void)
{
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
	vec3 n = normalize(normal);

	vec3 cpointlight1 = pointLight(vec3(1.0, 1.0, 1.0), Intensity, vec3(sin(Time) *  10.0, 1.0, cos(Time) * 10.0), n, position, diffuse, spec, CameraPosition );
	vec3 cpointlight2 = pointLight(vec3(1.0, 0.0, 0.0), Intensity, vec3(sin(Time * 1.5) *  10.0, 1.0, cos(Time) * 10.0), n, position, diffuse, spec, CameraPosition );
	vec3 cdirlight1 = directionalLight(vec3(1.0, 1.0, 1.0), Intensity/2.0, vec3(0.0, -1.0, 0.0), n, position, diffuse, spec, CameraPosition );
	vec3 cspotlight1 = spotLight(vec3(1.0, 1.0, 1.0), Intensity/1.8, vec3(0.0, -0.5, -0.5), vec3(sin(Time * 1.5) *  10.0, 10.0, cos(Time) * 10.0), n, position, diffuse, spec, CameraPosition );

	Color = vec4(cpointlight1 + cpointlight2 + cdirlight1 + cspotlight1, 1.0);
	//Color = vec4(cpointlight1, 1.0);
	//Color = vec4(cpointlight1 + cpointlight2, 1.0);

	Color = vec4(cspotlight1, 1.0);
	//Color = vec4(cdirlight1, 1.0);
}

#endif
