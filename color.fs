#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
  FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f); //lightColor * objectColor, 1.0);
}
