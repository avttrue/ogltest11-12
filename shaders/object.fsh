struct MaterialProperty
{
    vec3 DiffuseColor;
    vec3 AmbienceColor;
    vec3 SpecularColor;
    float Shines;
};

uniform bool u_IsUseDiffuseMap;
uniform bool u_IsUseNormalMap;
uniform bool u_IsDrawShadow;
uniform MaterialProperty u_MaterialProperty;
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_ShadowMap;
uniform highp float u_ShadowMapSize;
uniform highp float u_ShadowPointCloudFilteringQuality;
uniform highp float u_MainLightPower;

varying highp vec4 v_position;
varying highp vec2 v_textcoord;
varying highp vec3 v_normal;
varying highp mat3 v_tbnMatrix;
varying highp vec4 v_lightDirection;
varying highp vec4 v_positionLightMatrix;

float SampleShadowMap(sampler2D map, vec2 coords, float compare)
{
    vec4 v = texture2D(map, coords);
    float value = v.x * 255.0f + (v.y * 255.0f + (v.z * 255.0f + v.w) / 255.0f) / 255.0f;
    return step(compare, value);
}

float SampleShadowMapLinear(sampler2D map, vec2 coords, float compare, vec2 texelsize)
{
    vec2 pixsize = coords / texelsize + 0.5f;
    vec2 pixfractpart = fract(pixsize);
    vec2 starttexel = (pixsize - pixfractpart) * texelsize;
    float bltexel = SampleShadowMap(map, starttexel, compare);
    float brtexel = SampleShadowMap(map, starttexel + vec2(texelsize.x, 0.0f), compare);
    float tltexel = SampleShadowMap(map, starttexel + vec2(0.0f, texelsize.y), compare);
    float trtexel = SampleShadowMap(map, starttexel + texelsize, compare);

    float mixL = mix(bltexel, tltexel, pixfractpart.y);
    float mixR = mix(brtexel, trtexel, pixfractpart.y);

    return mix(mixL, mixR, pixfractpart.x);
}

// point cloud filtering
float SampleShadowMapPCF(sampler2D map, vec2 coords, float compare, vec2 texelsize)
{
    float result = 0.0f;
    float spcfq = u_ShadowPointCloudFilteringQuality;
    for(float y = -spcfq; y < spcfq; y += 1.0f)
        for(float x = -spcfq; x < spcfq; x += 1.0f)
        {
            vec2 offset = vec2(x, y) * texelsize;
            result += SampleShadowMapLinear(map, coords + offset, compare, texelsize);
        }
    return result / 9.0f;
}

float CalcShadowAmount(sampler2D map, vec4 plm)
{
    vec3 value = (plm.xyz / plm.w) * vec3(0.5f) + vec3(0.5f);
    float offset = 3.8f * dot(v_normal, v_lightDirection.xyz); // первый коэффициент должен влиять на качество тени
    return SampleShadowMapPCF(u_ShadowMap, value.xy, value.z * 255.0f + offset, vec2(1.0f / u_ShadowMapSize));
}

void main(void)
{
    vec3 usingNormal = v_normal; // используемая нормаль
    if(u_IsUseNormalMap) usingNormal = normalize(texture2D(u_NormalMap, v_textcoord).rgb * 2.0f - 1.0f);

    vec4 eyePosition = vec4(0.0f, 0.0f, 0.0f, 1.0f); // позиция наблюдателя

    vec3 eyeVec = normalize(v_position.xyz - eyePosition.xyz); // направление взгляда
    if(u_IsUseNormalMap) eyeVec = normalize(v_tbnMatrix * eyeVec);

    vec3 lightVec = normalize(v_lightDirection.xyz); // вектор освещения
    if(u_IsUseNormalMap) lightVec = normalize(v_tbnMatrix * lightVec);

    vec4 resultColor = vec4(0.0f, 0.0f, 0.0f, 0.0f); // результирующий цвет чёрный
    vec4 diffMatColor = texture2D(u_DiffuseMap, v_textcoord); // диффузный цвет
    vec3 reflectLight = normalize(reflect(lightVec, usingNormal)); // отражённый свет
    float len = length(v_position.xyz - eyePosition.xyz); // расстояние от наблюдателя до точки
    float specularFactor = u_MaterialProperty.Shines; // размер пятна блика
    float ambientFactor = 0.1f; // светимость материала
    vec4 reflectionColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); //цвет блика белый

    if(u_IsUseDiffuseMap == false) diffMatColor = vec4(u_MaterialProperty.DiffuseColor, 1.0f);

    vec4 diffColor = diffMatColor * u_MainLightPower * max(0.0f, dot(usingNormal, -lightVec));
    resultColor += diffColor;

    vec4 ambientColor = ambientFactor * diffMatColor;
    resultColor += ambientColor * vec4(u_MaterialProperty.AmbienceColor, 1.0f);

    vec4 specularColor = reflectionColor * u_MainLightPower * pow(max(0.0f, dot(reflectLight, -eyeVec)), specularFactor);
    resultColor += specularColor * vec4(u_MaterialProperty.SpecularColor, 1.0f);

    highp float shadowCoef = 1.0f;
    if(u_IsDrawShadow)
    {
        shadowCoef = CalcShadowAmount(u_ShadowMap, v_positionLightMatrix);
        shadowCoef += 0.15f; // избавляемся от абсолютной черноты тени
        if(shadowCoef > 1.0f) shadowCoef = 1.0f;
    }

    gl_FragColor = resultColor * shadowCoef;
}
