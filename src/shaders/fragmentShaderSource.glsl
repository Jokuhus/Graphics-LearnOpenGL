#version 330 core
in vec3 FragPos;  
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // 광원 방향
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Lambert (diffuse)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // ambient (약간의 기본 빛)
    vec3 ambient = 0.2 * lightColor; // 0.2 는 환경광의 세기

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}