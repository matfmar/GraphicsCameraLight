//klasa bazowa dla �r�de� �wiat�a
#include "Light.h"

Light::Light() {
	colour = glm::vec3(1.0f, 1.0f, 1.0f); //how much of that colors is shown
	ambientIntensity = 1.0f;
	diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity){
	colour = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
	diffuseIntensity = dIntensity;
}

Light::~Light() {

}