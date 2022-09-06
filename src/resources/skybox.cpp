#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "skybox.h"


Skybox::Skybox()
{

}


Skybox::~Skybox()
{

}


void Skybox::setSkyboxTexture(const char* texPath)
{
    this->texSkybox.setTextureHDR(texPath, "cubemapHDR", true);
}


void Skybox::renderToShader(Shader& shaderSkybox, glm::mat4& projection, glm::mat4& view)
{
    shaderSkybox.use();
    glActiveTexture(GL_TEXTURE0);
    this->texSkybox.useTexture();

    glUniform1i(glGetUniformLocation(shaderSkybox.ID, "envMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shaderSkybox.ID, "inverseView"), 1, GL_FALSE, glm::value_ptr(glm::transpose(view)));
    glUniformMatrix4fv(glGetUniformLocation(shaderSkybox.ID, "inverseProj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(projection)));
    glUniform1f(glGetUniformLocation(shaderSkybox.ID, "cameraAperture"), this->cameraAperture);
    glUniform1f(glGetUniformLocation(shaderSkybox.ID, "cameraShutterSpeed"), this->cameraShutterSpeed);
    glUniform1f(glGetUniformLocation(shaderSkybox.ID, "cameraISO"), this->cameraISO);
}


void Skybox::setExposure(GLfloat aperture, GLfloat shutterSpeed, GLfloat iso)
{
    this->cameraAperture = aperture;
    this->cameraShutterSpeed = shutterSpeed;
    this->cameraISO = iso;
}
