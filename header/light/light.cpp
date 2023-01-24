#include "light.h"

Light::Light(LightType type, glm::vec3 model, glm::vec3 pos) {
	this->type = type;
	this->model = model;
	this->pos = pos;
	this->ambient = glm::vec3(0.3f);
	this->diffuse = glm::vec3(0.5f);
	this->specular = glm::vec3(0.3f);
}

void Light::setProp(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}