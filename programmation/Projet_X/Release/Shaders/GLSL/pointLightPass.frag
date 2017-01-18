//[FRAGMENT SHADER]
#version 330
 
 
 struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};
 
struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct PointLight
{
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};
 
layout(location = 0) out vec4 FragColor;
 
uniform sampler2D InPositionMap;
uniform sampler2D InColorMap;
uniform sampler2D InNormalMap;
uniform PointLight InPointLight;
uniform vec3 InEyeWorldPos;
uniform float InMatSpecularIntensity;
uniform float InSpecularPower;
uniform vec2 InScreenSize;
 
vec3 CalcLightInternal(BaseLight Light,
					   vec3 LightDirection,
					   vec3 WorldPos,
					   vec3 Normal)
{
    vec3 AmbientColor = Light.Color * Light.AmbientIntensity;
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec3 DiffuseColor  = vec3(0, 0, 0);
    vec3 SpecularColor = vec3(0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = Light.Color * Light.DiffuseIntensity * DiffuseFactor;

        vec3 VertexToEye = normalize(InEyeWorldPos - WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, InSpecularPower);
        if (SpecularFactor > 0) {
            SpecularColor = Light.Color * InMatSpecularIntensity * SpecularFactor;
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}
 
vec3 CalcPointLight(vec3 WorldPos, vec3 Normal)
{
    vec3 LightDirection = WorldPos - InPointLight.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec3 Color = CalcLightInternal(InPointLight.Base, LightDirection, WorldPos, Normal);

    float Attenuation =  InPointLight.Atten.Constant +
                         InPointLight.Atten.Linear * Distance +
                         InPointLight.Atten.Exp * Distance * Distance;

    Attenuation = max(1.0, Attenuation);

    return Color / Attenuation;
}

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / InScreenSize;
}
 
void main()
{
	vec2 TexCoord = CalcTexCoord();
	vec3 WorldPos = texture(InPositionMap, TexCoord).xyz;
	vec3 Color = texture(InColorMap, TexCoord).xyz;
	vec3 Normal = texture(InNormalMap, TexCoord).xyz;
	Normal = normalize(Normal);

    FragColor = vec4(Color * CalcPointLight(WorldPos, Normal), 1.0f);
}