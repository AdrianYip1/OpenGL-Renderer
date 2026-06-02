#ifndef STYLESANDMODELS_H
#define STYLESANDMODELS_H
#include <string>

struct ShaderParams {
    //Direction of the light
    float dirLight[3];

    // Specular Light: color, glossiness, specular map
    float specularLight[4];
    float glossiness;
    bool bSpecularMap = false;
    std::string specularMap;
    
    // Lighting colors
    float ambientTint[3];
    float shadowTint[3];
    float lightTint[3];

    // Rim lighting
    bool bRimColor = false;
    float rimColor[4];
    float rimThreshold;
    float rimAmount;

    // Outline
    bool bOutline = false;
    float outlineWidth;
    float outlineBurnIntensity;
    float outlineLightInfluence;

    // Tone mapping
    float gamma;

    // HDR
    bool bHDR = false;
    float exposure;

    // Bloom
    bool bBloom = false;
    int blurPasses;
};


enum ModelType {
    NONE_MODEL,
    GENSHIN,
    SAMUS,
    TANGROWTH
};

enum Style {
    NONE_STYLE,
    CARTOON,
    ANIME
};

#endif