#version 330 core

out vec4 FragColor;
in vec4 fillColor;
in vec2 texCoord;
uniform vec2 textureSize;

uniform sampler2D texture1;
uniform bool FXAA;

///////////////////////////////////////////////////////

#ifndef FXAA_REDUCE_MIN
    #define FXAA_REDUCE_MIN   (1.0/ 64.0)
#endif
#ifndef FXAA_REDUCE_MUL
    #define FXAA_REDUCE_MUL   (1.8)
#endif
#ifndef FXAA_SPAN_MAX
    #define FXAA_SPAN_MAX     6.0
#endif


void fxaa() {
    // Calculate texel size
    vec2 texelSize = vec2(1.0 / textureSize.x, 1.0 / textureSize.y);

    // Sample neighboring texels
    vec4 rgbNW = texture(texture1, texCoord + vec2(-1, -1) * texelSize);
    vec4 rgbNE = texture(texture1, texCoord + vec2(1, -1) * texelSize);
    vec4 rgbSW = texture(texture1, texCoord + vec2(-1, 1) * texelSize);
    vec4 rgbSE = texture(texture1, texCoord + vec2(1, 1) * texelSize);
    vec4 rgbM = texture(texture1, texCoord);
    
    
    // Calculate luminance values for each texel, taking alpha into account
    float lumaNW = dot(rgbNW.rgb, vec3(0.299, 0.587, 0.114)) * (0.5 + rgbNW.a * 0.5);
    float lumaNE = dot(rgbNE.rgb, vec3(0.299, 0.587, 0.114)) * (0.5 + rgbNE.a * 0.5);
    float lumaSW = dot(rgbSW.rgb, vec3(0.299, 0.587, 0.114)) * (0.5 + rgbSW.a * 0.5);
    float lumaSE = dot(rgbSE.rgb, vec3(0.299, 0.587, 0.114)) * (0.5 + rgbSE.a * 0.5);
    float lumaM = dot(rgbM.rgb, vec3(0.299, 0.587, 0.114)) * (0.5 + rgbM.a * 0.5);

    // Calculate minimum and maximum luminance values
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));


    if(lumaMin > lumaMax * 0.7f){
       FragColor = texture(texture1, texCoord);
       //FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }

    // Calculate the direction vector
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    // Reduce direction vector
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texelSize;

    // Sample neighboring texels for anti-aliasing
    vec4 rgbA = 0.5 * (texture(texture1, texCoord + dir * (1.0 / 3.0 - 0.5)) + texture(texture1, texCoord + dir * (2.0 / 3.0 - 0.5)));
    vec4 rgbB = rgbA * 0.5 + 0.25 * (texture(texture1, texCoord + dir * -0.5) + texture(texture1, texCoord + dir * 0.5));

    // Calculate luminance for rgbB
    float lumaB = dot(rgbB.rgb, vec3(0.299, 0.587, 0.114)) * rgbB.a;

    // Determine final color based on edge detection
    if (lumaB < lumaMin || lumaB > lumaMax) {
        FragColor = rgbA;
    } else {
        FragColor = rgbB;
    }
}



///////////////////////////////////////////////////////////////////////

void main() {
		
        if(FXAA){
            fxaa();
        }
        else{
            FragColor = texture(texture1, texCoord);
        }
        
        if(FragColor.a < 0.01){
            discard;
        }
}

