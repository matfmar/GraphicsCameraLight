//ta klasa obs�uguj� materia� obiekt�w, tj. zachowanie co do odbijania �wiat�a
#include "Material.h"

Material::Material() {
	specularIntensity = 0.0f;		//wsp�czynnik odbicia bezpo�rednio do oka (specular light)
	shininess = 0.0f;		//g�adko��
}

Material::Material(GLfloat sIntensity, GLfloat shine) {
	specularIntensity = sIntensity;
	shininess = shine;
}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) {
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}