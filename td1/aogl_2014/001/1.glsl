#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

uniform float Time;
uniform float Speed;
uniform int InstanceCount;
uniform float Amplitude;
uniform float SpaceBetweenCubes;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;
}vertex;

void main(void)
{	

	int square_size = int(floor(sqrt(InstanceCount-1))) + 1;
	float total_size = square_size * (1+SpaceBetweenCubes);

	vec4 position = vec4(	VertexPosition.x + (gl_InstanceID%square_size) *(1+SpaceBetweenCubes) - total_size/2,
										VertexPosition.y + Amplitude * cos(gl_InstanceID + Time),
										VertexPosition.z + (gl_InstanceID/square_size) * (1+SpaceBetweenCubes) - total_size/2,
										1.0);

	vertex.uv = VertexTexCoord;
	vertex.normal = vec3(Object * vec4(VertexNormal, 1.0));
	vertex.position = vec3(Object * position);
	vertex.instance = gl_InstanceID;

	gl_Position = Projection * View * position;
}

#endif

#if defined(GEOMETRY)

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;

}vertices[];

out fData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;
}frag;    

void main()
{
	int i;
	for (i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		frag.normal = vertices[i].normal;
		frag.position = vertices[i].position;
		frag.uv = vertices[i].uv;
		frag.instance = vertices[i].instance;
		EmitVertex();
	}
	EndPrimitive();
}

#endif

#if defined(FRAGMENT)

in fData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
    flat int instance;
}frag;

uniform vec3 CameraPosition;
uniform mat4 View;

uniform vec3 LightPosition;
uniform vec4 LightSpecularColor;
uniform float LightDiffusePower;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;

out vec4 Color;

void main(void)
{
/*
	vec3 l = LightPosition - frag.position;
	vec3 v = mat3(View) * CameraPosition - frag.position;
    vec3 h = normalize(l-v);
    Color = LightDiffusePower * texture(DiffuseTexture, frag.uv) * dot(frag.normal, l) +  LightSpecularHardness * LightSpecularColor * pow(dot(frag.normal, h), texture(SpecularTexture, frag.uv).r);
  */  


	vec3 diffuse = texture(DiffuseTexture, frag.uv).rgb;
	float spec = texture(SpecularTexture, frag.uv).r;

	vec3 n = normalize(frag.normal);
	vec3 l = vec3(LightPosition) - frag.position;
	//vec3 l =  vec3(sin(Time) *  10.0, 1.0, cos(Time) * 10.0) - position;

	vec3 v = frag.position - CameraPosition;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);

	vec3 color = LightDiffusePower * diffuse * n_dot_l + spec * vec3(LightSpecularColor) *  pow(n_dot_h, spec * 10.0);

	Color = vec4(color, 1.0);

}
#endif
