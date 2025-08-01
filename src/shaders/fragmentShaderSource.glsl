#version 330 core
in vec3 FragPos;  
in vec3 Normal;
in vec2 UV;

out vec4 FragColor;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float textureRatio;
uniform sampler2D BumpSampler;
uniform sampler2D DiffuseSampler;

void main()
{
    // 광원 방향
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    vec3 normalMap = texture(BumpSampler, UV).rgb;
    vec3 bumpedNormal = normalize(normalMap);

    // Lambert (diffuse)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    float diffBump = max(dot(bumpedNormal, lightDir), 0.0);
    vec3 diffuseBump = diffBump * lightColor;

    // ambient (약간의 기본 빛)
    vec3 ambient = 0.2 * lightColor; // 0.2 는 환경광의 세기

    // 텍스처 색상 적용
    vec3 texColor = texture(DiffuseSampler, UV).rgb;
    vec3 baseColor = mix(objectColor, texColor, textureRatio);

    // 최종 색상
    vec3 result = (ambient + mix(diffuse, diffuseBump, textureRatio)) * baseColor;
    FragColor = vec4(result, 1.0);
}