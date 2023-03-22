//ta klasa obs³ugujê materia³ obiektów, tj. zachowanie co do odbijania œwiat³a
#include "Material.h"

Material::Material() {
	specularIntensity = 0.0f;		//wspó³czynnik odbicia bezpoœrednio do oka (specular light)
	shininess = 0.0f;		//g³adkoœæ
}

Material::Material(GLfloat sIntensity, GLfloat shine) {
	specularIntensity = sIntensity;
	shininess = shine;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) {
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}