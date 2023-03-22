//ta klasa to w rzeczywistoœci interfejs do kodu shaderów oraz obejmuje zbudowanie programu dla GPU
#include "Shader.h"
#include <cstring>

Shader::Shader() {
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
    uniformKolor = 0;
    uniformTextureOnPar = 0;
    pointLightCount = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) { 
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation) {        //³aduje kod z plików tekstowych
    std::string vertexString = ReadFile(vertexLocation);
    std::string fragmentString = ReadFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();
    CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation) {
    std::string content;
    std::ifstream filestream;
    filestream.open(fileLocation, std::ios::in);
    if (!filestream.is_open()) {
        std::cout << "Failed to open file: " << fileLocation << std::endl;
        return "";
    }
    std::string line = "";
    while (!filestream.eof()) {
        std::getline(filestream, line);
        content.append(line + "\n");
    }
    filestream.close();
    return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) {      //kompiluje 
    shaderID = glCreateProgram();
    if (!shaderID) {
        std::cout << "Error creating shaders!" << std::endl;
        return;
    }
    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout << "error linking program: " << eLog << std::endl;
        return;
    }
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout << "error validating program: " << eLog << std::endl;
        return;
    }
    //poni¿sze "wyci¹ga" identyfikatory zmiennych z shaderów aby by³ do nich dostêp
    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformKolor = glGetUniformLocation(shaderID, "kolor");
    uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
    uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");
    uniformTextureOnPar = glGetUniformLocation(shaderID, "textureOnPar");
    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        char locBuff[100] = { '\0' };
        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.colour", i);
        uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
    }
}
//funkcje na potrzeby interfejsu
GLuint Shader::GetProjectionLocation() {
    return uniformProjection;
}

GLuint Shader::GetModelLocation() {
    return uniformModel;
}

GLuint Shader::GetViewLocation() {
    return uniformView;
}

GLuint Shader::GetKolorLocation() {
    return uniformKolor;
}

GLuint Shader::GetTextureOnParLocation() {
    return uniformTextureOnPar;
}

GLuint Shader::GetAmbientColourLocation() {
    return uniformDirectionalLight.uniformColour;
}

GLuint Shader::GetAmbientIntensityLocation() {
    return uniformDirectionalLight.uniformAmbientIntensity;
}

GLuint Shader::GetDiffuseIntensityLocation() {
    return uniformDirectionalLight.uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation() {
    return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::GetSpecularIntensityLocation() {
    return uniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation() {
    return uniformShininess;
}

GLuint Shader::GetEyePositionLocation() {
    return uniformEyePosition;
}

void Shader::SetDirectionalLight(DirectionalLight* dLight) {
    dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColour,
        uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLight(PointLight* pLight, unsigned int lightCount) {
    if (lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;
    glUniform1i(uniformPointLightCount, lightCount);
    for (int i = 0; i < lightCount; ++i) {
        pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour, uniformPointLight[i].uniformDiffuseIntensity,
            uniformPointLight[i].uniformPosition,
            uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
    }
}

void Shader::UseShader() {
    glUseProgram(shaderID);
}

void Shader::ClearShader() {
    if (shaderID != 0) {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }
    uniformModel = 0;
    uniformProjection = 0;
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);
    const GLchar* theCode[1];
    theCode[0] = shaderCode;
    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);
    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout << "error compiling shader: " << shaderType << " : " << eLog << std::endl;
        return;
    }
    glAttachShader(theProgram, theShader);
}

Shader::~Shader() {
    ClearShader();
}
