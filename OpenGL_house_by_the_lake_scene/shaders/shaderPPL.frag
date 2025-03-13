#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform vec3 pointLightPos1, pointLightPos2; //pozitii lumini punctiforme
uniform vec3 pointLightColor; // Culoarea lumina punctiforme

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
vec3 diffuse;
vec3 specular;

float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;
float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0076f;


float computeFog()
{
 float fogDensity = 0.02f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f); //constrânge o valoare să se afle între două limite 
}

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

void computePointLightComponents(vec3 pointLightPosEye)
{		
    vec3 cameraPosEye = vec3(0.0f); //in eye coordinates, the viewer is situated at the origin

    //transform normal
    vec3 normalEye = normalize(fNormal);	

    //compute light direction (point light)
    vec3 lightDirN = normalize(pointLightPosEye - fPosEye.xyz);

    //compute view direction  
    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    //compute distance to light
    float dist = length(pointLightPosEye - fPosEye.xyz);
    //compute attenuation
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));


    //compute ambient light
    ambient = att * ambientStrength * pointLightColor;

    //compute diffuse light
    diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * pointLightColor;

    //compute specular light
    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular = att * specularStrength * specCoeff * pointLightColor;
}


void main() 
{
	vec3 totalAmbient = vec3(0.0f);
    vec3 totalDiffuse = vec3(0.0f);
    vec3 totalSpecular = vec3(0.0f);


	// Verificare transparență și eliminare fragmente transparente
    vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
    if (colorFromTexture.a < 0.1) {
        discard; // Elimină fragmentul
    }
	
	computeLightComponents(); //lumina directionala
	totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;


	computePointLightComponents(pointLightPos1); //lumina punctiforma no 1
	totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;

	computePointLightComponents(pointLightPos2); //lumina punctiforma no 2
	totalAmbient += ambient;
    totalDiffuse += diffuse;
    totalSpecular += specular;

	
	totalAmbient *= texture(diffuseTexture, fTexCoords).rgb;
    totalDiffuse *= texture(diffuseTexture, fTexCoords).rgb;
    totalSpecular *= texture(specularTexture, fTexCoords).rgb;
    

    vec3 color = min((totalAmbient + totalDiffuse) + totalSpecular, 1.0f);

    
    fColor = vec4(color, 1.0f);

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	fColor = mix(fogColor, fColor, fogFactor);  //sau fColor = fogColor * (1 – fogFactor) + color * fogFactor;
}
