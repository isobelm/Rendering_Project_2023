#version 330 core
out vec4 FragColor;

#define PI 3.14159265

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	float fresnel;
};

struct PointLight {
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 2

uniform vec3 ambientLight;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform float roughness = 0.15;
uniform float k = 0.2;

// function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	// == =====================================================
	// Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
	// For each phase, a calculate function is defined that calculates the corresponding color
	// per lamp. In the main() function we take all the calculated colors and sum them up for
	// this fragment's final color.
	// == =====================================================
	// phase 1: init lighting
	vec3 result = ambientLight * vec3(texture(material.diffuse, TexCoords));
	// phase 2: point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1.0);
}


// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
//	float diff = max(dot(normal, lightDir), 0.0);
//	// specular shading
//	vec3 reflectDir = reflect(-lightDir, normal);
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//	// attenuation
//	float distance = length(light.position - fragPos);
//	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//	ambient *= attenuation;
//	diffuse *= attenuation;
//	specular *= attenuation;
//	return (ambient + diffuse + specular);
	vec3 materialDiffuseColor = vec3(texture(material.diffuse, TexCoords));
	vec3 materialSpecularColor = vec3(texture(material.specular, TexCoords));
	
	float NdotL = max(0, dot(normal, lightDir));
	float Rs = 0.0;
	if (NdotL > 0)
	{
		vec3 H = normalize(lightDir + viewDir);
		float NdotH = max(0, dot(normal, H));
		float NdotV = max(0, dot(normal, viewDir));
		float VdotH = max(0, dot(lightDir, H));

		// Fresnel reflectance
		float F = pow(1.0 - VdotH, 5.0);
		F *= (1.0 - material.fresnel);
		F += material.fresnel;

		// Microfacet distribution by Beckmann
		float m_squared = roughness * roughness;
		float r1 = 1.0 / (4.0 * m_squared * pow(NdotH, 4.0));
		float r2 = (NdotH * NdotH - 1.0) / (m_squared * NdotH * NdotH);
		float D = r1 * exp(r2);

		// Geometric shadowing
		float two_NdotH = 2.0 * NdotH;
		float g1 = (two_NdotH * NdotV) / VdotH;
		float g2 = (two_NdotH * NdotL) / VdotH;
		float G = min(1.0, min(g1, g2));

		Rs = (F * D * G) / (PI * NdotL * NdotV);
	}
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	return materialDiffuseColor * light.diffuse * NdotL * attenuation + light.specular * materialSpecularColor * NdotL * (k + Rs * (1.0 - k)) * attenuation + ambient * attenuation;
	
}
