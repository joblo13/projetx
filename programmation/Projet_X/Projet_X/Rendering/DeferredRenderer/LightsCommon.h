#ifndef LIGHTS_COMMON_H
#define	LIGHTS_COMMON_H

#include "../../Math/Vector.h"

struct BaseLight
{
    Vector Color;
    float AmbientIntensity;
    float DiffuseIntensity;

    BaseLight()
    {
        Color = Vector(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
    }
};

struct DirectionalLight : public BaseLight
{        
    Vector Direction;

    DirectionalLight()
    {
        Direction = Vector(0.0f, 0.0f, 0.0f);
    }
};

struct PointLight : public BaseLight
{
    Vector position;

    struct
    {
        float Constant;
        float Linear;
        float Exp;
    } Attenuation;

    PointLight()
    {
        position = Vector(0.0f, 0.0f, 0.0f);
        Attenuation.Constant = 0.0f;
        Attenuation.Linear = 0.0f;
        Attenuation.Exp = 0.0f;
    }
};

struct SpotLight : public PointLight
{
    Vector Direction;
    float Cutoff;

    SpotLight()
    {
        Direction = Vector(0.0f, 0.0f, 0.0f);
        Cutoff = 0.0f;
    }
};

#define COLOR_WHITE Vector(1.0f, 1.0f, 1.0f)
#define COLOR_RED Vector(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Vector(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN Vector(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE Vector(0.0f, 0.0f, 1.0f)

#endif
