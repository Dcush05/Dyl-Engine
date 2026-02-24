#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 color;
uniform vec3 lightPos;

uniform vec3 viewPos;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;


void main()
{
	//FragColor = vec4(color, 1.0);
	vec3 N = normalize(Normal);
	vec3 normalTex = texture(normal, TexCoords).rgb;
	normalTex = normalize(normalTex * 2.0 - 1.0);
	N = normalize(N + normalTex * 0.5);

	vec3 L = normalize(lightPos - FragPos); 

	vec3 V = normalize(viewPos - FragPos); 
	vec3 R = reflect(-L, N);


	vec3 diffuseColor = texture(diffuse, TexCoords).rgb;
	vec3 specularColor = texture(specular, TexCoords).rgb;

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(V,R), 0.0), 32.0);

	vec3 ambient = 0.1 * diffuseColor;
	vec3 diffuseLight = diff * diffuseColor;
	vec3 specularLight = spec * specularColor;

	vec3 model_color = ambient + diffuseLight + specularLight;
	FragColor = vec4(model_color, 1.0);
}
