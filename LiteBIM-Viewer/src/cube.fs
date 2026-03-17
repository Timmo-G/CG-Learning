#version 330 core
out vec4 FragColor;

// 定义材质结构体 (东大深度：展示你对GLSL高级语法的掌握)
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess; // 反光度（数值越大，高光点越小、越亮）
}; 

// 定义光源结构体
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos; // 摄像机位置
uniform Material material;
uniform Light light;

void main() {
    // 1. 环境光 (Ambient)
    vec3 ambient = light.ambient * material.ambient;
  	
    // 2. 漫反射 (Diffuse)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // 3. 高光 (Blinn-Phong Specular) —— 今日核心改动
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); // 计算半程向量
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}