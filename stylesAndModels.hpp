#ifndef STYLESANDMODELS_H
#define STYLESANDMODELS_H
#include <string>

struct ShaderParams {
    //Direction of the light
    float dirLight[3];

    // Specular Light: color, glossiness, specular map
    float specularColor[4];
    float glossiness;
    bool bSpecularMap = false;
    std::string specularMap;
    
    // Lighting colors
    float ambientTint[3];
    float shadowTint[3];
    float lightTint[3];

    float saturation;

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
    unsigned int HDRtype; // 0: exposure, 1: GT, 2: ACE

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

// Stylized non-photorealistic shading modes. CEL and HATCHING are inspired by
// anime/manga rendering respectively (Guilty Gear Xrd's cel shading).
enum Style {
    NONE_STYLE,
    CARTOON,
    CEL,       // anime-style two-tone cel shading
    HATCHING,  // manga-style pen-and-ink cross-hatching
    ISOPHOTES
};

#endif