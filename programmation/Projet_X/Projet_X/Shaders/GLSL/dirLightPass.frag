//[FRAGMENT SHADER]
#version 330

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

layout(location = 0) out vec4 FragColor;

uniform sampler2D InPositionMap;
uniform sampler2D InColorMap;
uniform sampler2D InNormalMap;
uniform DirectionalLight InDirectionalLight;
uniform vec3 InEyeWorldPos;
uniform vec2 InScreenSize;
 
vec3 CalcLightInternal(BaseLight i_light,
					   vec3 i_lightDirection,
					   vec3 i_worldPos,
					   vec3 i_normal, 
					   float i_specularPower, 
					   float i_matSpecularIntensity)
{
    vec3 AmbientColor = i_light.Color * i_light.AmbientIntensity;
    float DiffuseFactor = dot(i_normal, -i_lightDirection);

    vec3 DiffuseColor  = vec3(0, 0, 0);
    vec3 SpecularColor = vec3(0, 0, 0);

    if (DiffuseFactor > 0) 
	{
        DiffuseColor = i_light.Color * i_light.DiffuseIntensity * DiffuseFactor;

        vec3 VertexToEye = normalize(InEyeWorldPos - i_worldPos);
        vec3 LightReflect = normalize(reflect(i_lightDirection, i_normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, i_specularPower);
        if (SpecularFactor > 0) 
		{
            SpecularColor = i_light.Color * i_matSpecularIntensity * SpecularFactor;
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec3 CalcDirectionalLight(vec3 i_worldPos, vec3 i_normal, float i_specularPower, float i_matSpecularIntensity)
{
    return CalcLightInternal(InDirectionalLight.Base,
							 InDirectionalLight.Direction,
							 i_worldPos,
							 i_normal, 
							 i_specularPower, i_matSpecularIntensity);
}

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / InScreenSize;
}

void main()
{
    vec2 TexCoord = CalcTexCoord();
	vec4 worldPos = texture(InPositionMap, TexCoord);
	vec3 Color = texture(InColorMap, TexCoord).xyz;
	vec3 normal = texture(InNormalMap, TexCoord).xyz;
	normal = normalize(normal);
	
	float specularPower = texture(InColorMap, TexCoord).a;
	float matSpecularIntensity = texture(InNormalMap, TexCoord).a;

	FragColor = vec4(Color, 1.0f);
	if((int(worldPos.a) & 0x00000001) != 1)
		FragColor.xyz = FragColor.xyz * CalcDirectionalLight(worldPos.xyz, normal, specularPower, matSpecularIntensity);
}