#version 120

// Adapted manually from shader code at - https://github.com/Swordfish90/cool-retro-term

#ifdef GL_ES
    precision mediump float;
#endif

#define M_PI 3.1415926535897932384626433832795

// This is the average value of the abs(sin) function. Needed to avoid aliasing.
#define absSinAvg 0.63661828335466886

uniform sampler2D texture;
uniform vec2 textureSize;

uniform float devicePixelRatio;
uniform vec2 virtualResolution;

uniform bool enableScanlines;
uniform bool enablePixelRasterization;

uniform bool enableScreenCurvature;
uniform float screenCurvature;

vec2 rasterizationSmooth = vec2(
    clamp(2.0 * virtualResolution.x / (textureSize.x * devicePixelRatio), 0.0, 1.0),
    clamp(2.0 * virtualResolution.y / (textureSize.y * devicePixelRatio), 0.0, 1.0));

float getScanlineIntensity(vec2 coords) {
    float result = 1.0;

    if(enableScanlines) {
        float val = abs(sin(coords.y * virtualResolution.y * M_PI));
        result *= mix(val, absSinAvg, rasterizationSmooth.y);

        if(enablePixelRasterization) {
            val = abs(sin(coords.x * virtualResolution.x * M_PI));
            result *= mix(val, absSinAvg, rasterizationSmooth.x);
        }
    }

    return result;
}

float rgb2grey(vec3 v) {
    return dot(v, vec3(0.21, 0.72, 0.04));
}

void main() {
    vec2 cc = vec2(0.5) - gl_TexCoord[0].xy;
    float distance = length(cc);

    vec2 staticCoords;

    if(enableScreenCurvature) {
        float distortion = dot(cc, cc) * screenCurvature;
        staticCoords = (gl_TexCoord[0].xy - cc * (1.0 + distortion) * distortion);
    } else {
        staticCoords = gl_TexCoord[0].xy;
    }

    vec2 coords = staticCoords;

    vec2 txt_coords = coords;

    vec3 finalColor = texture2D(texture, txt_coords).rgb;

    finalColor *= getScanlineIntensity(coords);

    finalColor *= smoothstep(-textureSize.x, 0.0, staticCoords.x) - smoothstep(1.0, 1.0 + textureSize.x, staticCoords.x);
    finalColor *= smoothstep(-textureSize.y, 0.0, staticCoords.y) - smoothstep(1.0, 1.0 + textureSize.y, staticCoords.y);

    gl_FragColor = vec4(finalColor, 1.0);
}
