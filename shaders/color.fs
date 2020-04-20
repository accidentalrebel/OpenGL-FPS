#version 330 core

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_specular1;
  sampler2D emission;
  float shininess;
};

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
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

struct SpotLight {
  vec3 position;
  vec3 direction;

  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform int hasSpecular;
uniform vec3 viewPos;
uniform int pointLightCount;
uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[5];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  vec3 result = CalcDirLight(dirLight, norm, viewDir);
  for(int i = 0; i < pointLightCount; i++) {
    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
  }
  result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
  FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // combine results
  vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 specular;
	if ( hasSpecular > 0 )
		specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
  return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // attenuation
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + 
														 light.quadratic * (distance * distance));    
  // combine results
  vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specular;
	if ( hasSpecular > 0 )
		specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon   = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    
    
  if(theta > light.outerCutOff) 
	{       
		vec3 lightDir = normalize(-light.direction);
		// diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);
		// specular shading
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		// combine results
		vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
		vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
		vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

		diffuse *= intensity;
		specular *= intensity;
      
		return (ambient + diffuse + specular);
	}
  else
	{
		return light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	}
}
