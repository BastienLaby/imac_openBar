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

	
	/*gl_Position = Projection * View * vec4(VertexPosition.x + gl_InstanceID*Espacement,
									   VertexPosition.y + RotateRangeX*cos(Time+gl_InstanceID),
									   VertexPosition.z - RotateRangeY*sin(Time+gl_InstanceID),
									   1.0);*/

	int square_size = int(floor(sqrt(InstanceCount-1))) + 1;
	float total_size = square_size * (1+SpaceBetweenCubes);

	gl_Position = Projection * View * vec4(	VertexPosition.x + (gl_InstanceID%square_size) * (1+SpaceBetweenCubes) - total_size/2,
											VertexPosition.y + Amplitude * cos(gl_InstanceID + Time),
											VertexPosition.z + (gl_InstanceID/square_size) * (1+SpaceBetweenCubes) - total_size/2,
											1.0);


	vertex.uv = VertexTexCoord;
	vertex.normal = VertexNormal;
	vertex.position = VertexPosition;
	vertex.instance = gl_InstanceID;
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
uniform float Time;

uniform vec3 LightPosition;
uniform vec4 LightDiffuseColor;
uniform vec4 LightSpecularColor;
uniform float LightDiffusePower;
uniform float LightSpecularHardness;

out vec4 Color;

void main(void)
{

	/*vec3 l = LightPosition - frag.position;
	vec3 v = CameraPosition - frag.position;
    vec3 h = normalize(l-v);
    Color = LightDiffusePower * LightDiffuseColor * dot(frag.normal, l) + LightSpecularColor * pow(dot(frag.normal, h), LightSpecularHardness);*/

	vec3 light_direction = LightPosition - frag.position; //3D position in space of the surface
	float distanceLightFragment = length(light_direction);
	normalize(light_direction);
	distanceLightFragment = distanceLightFragment * distanceLightFragment; //This line may be optimised using Inverse square root

	//Intensity of the diffuse light. Saturate to keep within the 0-1 range.
	float n_dot_l = dot(frag.normal, light_direction);
	float intensity = clamp(n_dot_l, 0, 1);

	// Calculate the diffuse light factoring in light color, power and the attenuation
	vec4 diffuse = intensity * LightDiffuseColor * LightDiffusePower / distanceLightFragment; 

    //Calculate the half vector between the light vector and the view vector.
    //This is faster than calculating the actual reflective vector.
    vec3 h = normalize(light_direction + CameraPosition);

    //Intensity of the specular light
	float n_dot_h = dot(frag.normal, h);
	intensity = pow(clamp(n_dot_h, 0, 1), LightSpecularHardness);

	//Sum up the specular light factoring
	vec4 specular = intensity * LightSpecularColor * LightSpecularHardness / distanceLightFragment;

	vec4 ambient_color = vec4(0.1, 0.1, 0.1, 1);

	Color = ambient_color + diffuse + specular;

}
#endif
