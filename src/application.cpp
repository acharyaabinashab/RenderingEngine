#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/entity.h>

#include "texture.h"
#include "material.h"
#include "shape.h"

#ifndef ENTITY_H
#define ENTITY_H

#include <list> //std::list
#include <memory> //std::unique_ptr

class Entity : public Model
{
public:
    list<unique_ptr<Entity>> children;
    Entity* parent;
};
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

bool putEntityInSceneHierarchyPanel(Entity& parent, Entity*& ptrToSelectedEntity);
bool rightClickMenu(Entity& m_entity);
bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

void renderQuad();
void renderCube();

void gBufferSetup();
void saoSetup();
void postprocessSetup();
void screenSetup();
void iblSetup();

// settings
unsigned int SCR_WIDTH = 1400;
unsigned int SCR_HEIGHT = 900;
unsigned int viewportWidth = 1400;
unsigned int viewportHeight = 900;

// camera
Camera camera(glm::vec3(0.0f, 6.0f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;     // for glfw window not imgui window
float lastY = SCR_HEIGHT / 2.0f;
bool mouseDragEnabled = false;


GLuint screenQuadVAO, screenQuadVBO;
GLuint gBuffer, zBuffer, gPosition, gNormal, gAlbedo, gEffects;
GLuint saoFBO, saoBlurFBO, saoBuffer, saoBlurBuffer;
GLuint postprocessFBO, postprocessBuffer;
GLuint screenFBO, screenBuffer;
GLuint envToCubeFBO, irradianceFBO, prefilterFBO, brdfLUTFBO, envToCubeRBO, irradianceRBO, prefilterRBO, brdfLUTRBO;

GLint gBufferView = 1;
GLint tonemappingMode = 1;
GLint lightDebugMode = 3;
GLint attenuationMode = 2;
GLint saoSamples = 12;
GLint saoTurns = 7;
GLint saoBlurSize = 4;
GLint motionBlurMaxSamples = 32;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat deltaGeometryTime = 0.0f;
GLfloat deltaLightingTime = 0.0f;
GLfloat deltaSAOTime = 0.0f;
GLfloat deltaPostprocessTime = 0.0f;
GLfloat deltaForwardTime = 0.0f;
GLfloat deltaGUITime = 0.0f;
GLfloat materialRoughness = 0.01f;
GLfloat materialMetallicity = 0.02f;
GLfloat ambientIntensity = 0.005f;
GLfloat saoRadius = 0.3f;
GLfloat saoBias = 0.001f;
GLfloat saoScale = 0.7f;
GLfloat saoContrast = 0.8f;
GLfloat lightPointRadius1 = 3.0f;
GLfloat lightPointRadius2 = 3.0f;
GLfloat lightPointRadius3 = 3.0f;
GLfloat cameraAperture = 16.0f;
GLfloat cameraShutterSpeed = 0.5f;
GLfloat cameraISO = 1000.0f;
GLfloat modelRotationSpeed = 0.0f;

bool cameraMode;
bool pointMode = true;
bool directionalMode = true;
bool iblMode = true;
bool saoMode = true;
bool fxaaMode = true;
bool motionBlurMode = true;
bool screenMode = false;
bool firstMouse = true;
bool guiIsOpen = true;
bool keys[1024];

float directionalLightIntensity = 1.0f;
glm::vec3 albedoColor = glm::vec3(1.0f);
glm::vec3 materialF0 = glm::vec3(0.04f);    // UE4 dielectric
glm::vec3 lightDirectionalDirection1 = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 lightDirectionalColor1 = glm::vec3(1.0f);
glm::vec3 modelPosition = glm::vec3(0.0f);
glm::vec3 modelRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 modelScale = glm::vec3(0.1f);

glm::mat4 projViewModel;
glm::mat4 prevProjViewModel = projViewModel;
glm::mat4 envMapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 envMapView[] =
{
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};


Shader gBufferShader;
Shader latlongToCubeShader;
Shader simpleShader;
Shader lightingBRDFShader;
Shader irradianceIBLShader;
Shader prefilterIBLShader;
Shader integrateIBLShader;
Shader firstpassPPShader;
Shader saoShader;
Shader saoBlurShader;

Texture objectAlbedo;
Texture objectNormal;
Texture objectRoughness;
Texture objectMetalness;
Texture objectAO;
Texture envMapHDR;
Texture envMapCube;
Texture envMapIrradiance;
Texture envMapPrefilter;
Texture envMapLUT;

Material pbrMat;

Model objectModel;


Shape quadRender;
Shape envCubeRender;

// Addable Objects
Model planetModel;
Model rockModel;
Model sponzaModel;
Model backPackModel;
Model dinosaurModel;

int selected_hierarchy_node = 0;    // select the scene on start
int gizmoType = 0;
bool mouseHoveringViewport = false;

int main()
{
    // ------------------------------
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // --------------------
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSwapInterval(0);                                                // Disable Vsync


    // ---------------------------------------
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);


    // -----------------------------
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // ------------
    // imgui: setup
    // ------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(4.0));    // make it same as glfwWindowHint


    // -------------
    // load entities : [ Replace this with custom new scene ]
    // -------------
    Model model = Model(FileSystem::getPath("resources/objects/planet/planet.obj")); // Works only after glad is loaded
    Entity scene = Entity("Scene Root");
    scene.transform.setLocalPosition({ 0, 0, 0 });


    const float scale = 0.75f;
    {
        scene.addChild(model, "Planets");
        Entity* lastEntity = scene.children.back().get();
        lastEntity->transform.setLocalScale({ 0.25f, 0.25f, 0.25f });


        for (unsigned int i = 0; i < 10; ++i)
        {
            lastEntity->addChild(true, "New Light");
            lastEntity->addChild(model, "New Child");


            //Set tranform values of light
            lastEntity->children.front().get()->transform.setLocalPosition({ -10, 0, 0 });
            lastEntity->children.front().get()->transform.setLocalScale({ scale, scale, scale });


            //Set tranform values of model entity
            lastEntity->children.back().get()->transform.setLocalPosition({ -10, 4, 0 });
            lastEntity->children.back().get()->transform.setLocalScale({ scale, scale, scale });


            lastEntity = lastEntity->children.back().get();
        }
    }


    //----------
    // Shader(s)
    //----------
    gBufferShader.setShader("resources/shaders/gBuffer.vert", "resources/shaders/gBuffer.frag");
    saoShader.setShader("resources/shaders/postprocess/sao.vert", "resources/shaders/postprocess/sao.frag");
    saoBlurShader.setShader("resources/shaders/postprocess/sao.vert", "resources/shaders/postprocess/saoBlur.frag");
    latlongToCubeShader.setShader("resources/shaders/latlongToCube.vert", "resources/shaders/latlongToCube.frag");
    irradianceIBLShader.setShader("resources/shaders/lighting/irradianceIBL.vert", "resources/shaders/lighting/irradianceIBL.frag");
    prefilterIBLShader.setShader("resources/shaders/lighting/prefilterIBL.vert", "resources/shaders/lighting/prefilterIBL.frag");
    integrateIBLShader.setShader("resources/shaders/lighting/integrateIBL.vert", "resources/shaders/lighting/integrateIBL.frag");
    lightingBRDFShader.setShader("resources/shaders/lighting/lightingBRDF.vert", "resources/shaders/lighting/lightingBRDF.frag");
    firstpassPPShader.setShader("resources/shaders/postprocess/postprocess.vert", "resources/shaders/postprocess/firstpass.frag");
    simpleShader.setShader("resources/shaders/lighting/simple.vert", "resources/shaders/lighting/simple.frag");
    cout << "Shaders Compiled \n";


    // -----------
    // Textures(s)
    // -----------
    // Environment Map HDRI Setup
    // --------------------------
    envMapHDR.setTextureHDR("resources/textures/hdr/canyon.hdr", "canyonHDR", true);
    envMapCube.setTextureCube(512, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
    envMapIrradiance.setTextureCube(32, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR);
    envMapPrefilter.setTextureCube(128, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
    envMapPrefilter.computeTexMipmap();
    envMapLUT.setTextureHDR(512, 512, GL_RG, GL_RG16F, GL_FLOAT, GL_LINEAR);


    //---------------
    // Shape(s)
    //---------------
    envCubeRender.setShape("cube", glm::vec3(0.0f));
    quadRender.setShape("quad", glm::vec3(0.0f));


    //---------------------------------------------------------
    // Set the samplers for the lighting/post-processing passes
    //---------------------------------------------------------
    lightingBRDFShader.use();
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "gAlbedo"), 1);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "gNormal"), 2);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "gEffects"), 3);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "sao"), 4);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "envMap"), 5);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "envMapIrradiance"), 6);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "envMapPrefilter"), 7);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "envMapLUT"), 8);


    saoShader.use();
    glUniform1i(glGetUniformLocation(saoShader.ID, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(saoShader.ID, "gNormal"), 1);


    firstpassPPShader.use();
    glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "sao"), 1);
    glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "gEffects"), 2);


    latlongToCubeShader.use();
    glUniform1i(glGetUniformLocation(latlongToCubeShader.ID, "envMap"), 0);


    irradianceIBLShader.use();
    glUniform1i(glGetUniformLocation(irradianceIBLShader.ID, "envMap"), 0);


    prefilterIBLShader.use();
    glUniform1i(glGetUniformLocation(prefilterIBLShader.ID, "envMap"), 0);


    // --------------------
    // Render buffers setup
    // --------------------
    gBufferSetup();     // G-Buffer setup
    saoSetup();         // SAO setup
    postprocessSetup(); // Postprocessing setup
    screenSetup();      // Screen setup
    iblSetup();         // IBL setup


    //------------------------------
    // Queries setting for profiling
    //------------------------------
    GLuint64 startGeometryTime, startLightingTime, startSAOTime, startPostprocessTime, startForwardTime, startGUITime;
    GLuint64 stopGeometryTime, stopLightingTime, stopSAOTime, stopPostprocessTime, stopForwardTime, stopGUITime;


    unsigned int queryIDGeometry[2];
    unsigned int queryIDLighting[2];
    unsigned int queryIDSAO[2];
    unsigned int queryIDPostprocess[2];
    unsigned int queryIDForward[2];
    unsigned int queryIDGUI[2];


    glGenQueries(2, queryIDGeometry);
    glGenQueries(2, queryIDLighting);
    glGenQueries(2, queryIDSAO);
    glGenQueries(2, queryIDPostprocess);
    glGenQueries(2, queryIDForward);
    glGenQueries(2, queryIDGUI);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    // ---------------------
    // Models Initialization
    // ---------------------
    // It is initialized here because it needs the glad loader to finish for it to work
    // Also doesn't work when this is called from outside the function from where it is declared
    // Doing this here also prevents UV fckup (why tho?)
    // -------------------------------------------------
    planetModel = Model(FileSystem::getPath("resources/objects/planet/planet.obj"));
    rockModel = Model(FileSystem::getPath("resources/objects/rock/rock.obj"));
    sponzaModel = Model(FileSystem::getPath("resources/objects/sponza/sponza.obj"));
    backPackModel = Model(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    //dinosaurModel = Model(FileSystem::getPath("resources/objects/dinosaur/Dinosaur_texture.obj"));
    std::cout << "Loaded all Models" << "\n";


    // -----------
    // Render Loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processInput(window);   // User input given to window created by glfw


        scene.updateSelfAndChild(); // Update model transforms changed in previouse frame


        // ------------
        // Camera Setup
        // ------------
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)viewportWidth / (float)viewportHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        const Frustum camFrustum = createFrustumFromCamera(camera, (float)viewportWidth / (float)viewportHeight, glm::radians(camera.Zoom), 0.1f, 100.0f);


        //------------------------
        // Geometry Pass rendering
        //------------------------
        glQueryCounter(queryIDGeometry[0], GL_TIMESTAMP);   // Start geometry pass timer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);         // Binding the framebuffer to gBuffer object so that the shader outputs to buffers inside gBuffer object
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        gBufferShader.use();                                                                                                // Setting up resources used by gBufferShader
        glUniformMatrix4fv(glGetUniformLocation(gBufferShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));      // Camera Projection
        glUniformMatrix4fv(glGetUniformLocation(gBufferShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));                  // Camera View
        glUniform3f(glGetUniformLocation(gBufferShader.ID, "albedoColor"), albedoColor.r, albedoColor.g, albedoColor.b);        // Default albedo Color white


        unsigned int total = 0, display = 0;
        scene.drawSelfAndChild(camFrustum, gBufferShader, display, total);  // Draw our Scene Graph while passing remaining resources to the shader
        std::cout << "Total process in CPU : " << total << " / Total send to GPU : " << display << std::endl;


        glBindFramebuffer(GL_FRAMEBUFFER, 0);               // Resets the non rendering framebuffer to direct to window framebuffer
        glQueryCounter(queryIDGeometry[1], GL_TIMESTAMP);   // stop geometry pass timer


        //---------------
        // SAO rendering
        //---------------
        glQueryCounter(queryIDSAO[0], GL_TIMESTAMP);    // Start SAO pass timer
        glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
        glClear(GL_COLOR_BUFFER_BIT);


        if (saoMode)
        {
            // SAO noisy texture
            saoShader.use();
            glActiveTexture(GL_TEXTURE0);                                                       // Setting up resources used by saoShader
            glBindTexture(GL_TEXTURE_2D, gPosition);                                                // pass gPosition texture from gBuffer to sao shader (generated by geometry pass)
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);                                                  // pass gNormal texture from gBuffer to sao shader (generated by geometry pass)
            glUniform1i(glGetUniformLocation(saoShader.ID, "saoSamples"), saoSamples);              // setting sao variables ...
            glUniform1f(glGetUniformLocation(saoShader.ID, "saoRadius"), saoRadius);
            glUniform1i(glGetUniformLocation(saoShader.ID, "saoTurns"), saoTurns);
            glUniform1f(glGetUniformLocation(saoShader.ID, "saoBias"), saoBias);
            glUniform1f(glGetUniformLocation(saoShader.ID, "saoScale"), saoScale);
            glUniform1f(glGetUniformLocation(saoShader.ID, "saoContrast"), saoContrast);
            glUniform1i(glGetUniformLocation(saoShader.ID, "viewportWidth"), viewportWidth);
            glUniform1i(glGetUniformLocation(saoShader.ID, "viewportHeight"), viewportHeight);
            quadRender.drawShape();                                                             // Apply saoShader over the whole screen


            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // SAO blur pass
            glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);  // Separate frame buffer object for blurred SAO
            glClear(GL_COLOR_BUFFER_BIT);


            saoBlurShader.use();
            glUniform1i(glGetUniformLocation(saoBlurShader.ID, "saoBlurSize"), saoBlurSize);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, saoBuffer);
            quadRender.drawShape();
        }


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glQueryCounter(queryIDSAO[1], GL_TIMESTAMP);    // Stop SAO pass timer


        //------------------------
        // Lighting Pass rendering
        //------------------------
        glQueryCounter(queryIDLighting[0], GL_TIMESTAMP);   // Start lighting pass timer
        glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        lightingBRDFShader.use();                       // Setting up resources used by lighting pass shader
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);            // Position of vertex
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);              // Albedo & Roughness (in .a)
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gNormal);              // Normal & Metalness (in .a)
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gEffects);             // Texture Ambient Occlusion
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);        // Screenspace AO
        glActiveTexture(GL_TEXTURE5);
        envMapHDR.useTexture();                             // Environment Map for background
        glActiveTexture(GL_TEXTURE6);
        envMapIrradiance.useTexture();                      // Environment Map sample for ambient lighting
        glActiveTexture(GL_TEXTURE7);
        envMapPrefilter.useTexture();                       // Env MipMap 
        glActiveTexture(GL_TEXTURE8);
        envMapLUT.useTexture();                             // Environment Map for reflection


        unsigned int totalLights = 0;
        scene.drawPointLights(lightingBRDFShader, totalLights, camera);     // point light info pass to shader

        
        // Directional light info pass to shader
        glm::vec3 lightDirectionViewSpace = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightDirectionalDirection1, 0.0f));
        glUniform3f(glGetUniformLocation(lightingBRDFShader.ID, "lightDirectionalArray[0].color"),
            lightDirectionalColor1.x * directionalLightIntensity,
            lightDirectionalColor1.y * directionalLightIntensity,
            lightDirectionalColor1.z * directionalLightIntensity);
        glUniform3f(glGetUniformLocation(lightingBRDFShader.ID, "lightDirectionalArray[0].direction"),
            lightDirectionViewSpace.x,
            lightDirectionViewSpace.y,
            lightDirectionViewSpace.z);


        glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader.ID, "inverseView"), 1, GL_FALSE, glm::value_ptr(glm::transpose(view)));      // Camera view for vert shader
        glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader.ID, "inverseProj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(projection)));  // Camera projection for vert shader
        glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));                             // Camera view for frag shader
        glUniform1f(glGetUniformLocation(lightingBRDFShader.ID, "materialRoughness"), materialRoughness);                                       // [ Not set up ]
        glUniform1f(glGetUniformLocation(lightingBRDFShader.ID, "materialMetallicity"), materialMetallicity);                                   // [ Not set up ]
        glUniform3f(glGetUniformLocation(lightingBRDFShader.ID, "materialF0"), materialF0.r, materialF0.g, materialF0.b);                       // Sth to do with fresnel effect
        glUniform1f(glGetUniformLocation(lightingBRDFShader.ID, "ambientIntensity"), ambientIntensity);                                         // [ Not set up ]
        glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "gBufferView"), gBufferView);                                                   // Choose differend debug view, eg: normal, SAO, metallic, etc.
        glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "pointMode"), pointMode);                                                       // Point light flag
        glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "directionalMode"), directionalMode);                                           // Directional light flag
        glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "iblMode"), iblMode);                                                           // Image Based Lighting flag
        glUniform1i(glGetUniformLocation(lightingBRDFShader.ID, "attenuationMode"), attenuationMode);                                           // UE4 or Quadratic attenuation


        quadRender.drawShape();     // Apply lighting pass over the whole screen


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glQueryCounter(queryIDLighting[1], GL_TIMESTAMP);   // Stop lighting pass timer


        //-------------------------------
        // Post-processing Pass rendering
        //-------------------------------
        glQueryCounter(queryIDPostprocess[0], GL_TIMESTAMP);    // Start post-processing pass timer
        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);           // Setting up the buffer object used my ImGui viewport to render scene
        glClear(GL_COLOR_BUFFER_BIT);

        firstpassPPShader.use();                                                                                                    // Setting up resources used by post processing
        glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "gBufferView"), gBufferView);                                            // Used as debug view flag to skip post processing
        glUniform2f(glGetUniformLocation(firstpassPPShader.ID, "screenTextureSize"), 1.0f / viewportWidth, 1.0f / viewportHeight);      // For FXAA offset calculation
        glUniform1f(glGetUniformLocation(firstpassPPShader.ID, "cameraAperture"), cameraAperture);                                      // Physical camera aperture sim
        glUniform1f(glGetUniformLocation(firstpassPPShader.ID, "cameraShutterSpeed"), cameraShutterSpeed);                              // Physical camera shutter speed sim
        glUniform1f(glGetUniformLocation(firstpassPPShader.ID, "cameraISO"), cameraISO);                                                // Physical camera ISO sim
        glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "saoMode"), saoMode);                                                    // SAO flag
        glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "fxaaMode"), fxaaMode);                                                  // FXAA flag
        glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "motionBlurMode"), motionBlurMode);                                      // Motion Blur Flag
        glUniform1f(glGetUniformLocation(firstpassPPShader.ID, "motionBlurScale"), int(ImGui::GetIO().Framerate) / 60.0f);              // Motion Blur Scale
        glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "motionBlurMaxSamples"), motionBlurMaxSamples);                          // Motion Blur Samples
        glUniform1i(glGetUniformLocation(firstpassPPShader.ID, "tonemappingMode"), tonemappingMode);                                    // Tonemapping mode

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postprocessBuffer);    // Result of lighting pass sent for post-processing
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);        // SAO for post-processing
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gEffects);             // Texture AO for post-processing

        quadRender.drawShape();                             // Apply post-processing on whole screen

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glQueryCounter(queryIDPostprocess[1], GL_TIMESTAMP);    // Stop post-processing pass timer


        //-----------------------
        // Forward Pass rendering
        //-----------------------
        glQueryCounter(queryIDForward[0], GL_TIMESTAMP);    // Start timer for non PBR rendering
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


        // Copy the depth informations from the Geometry Pass into the default framebuffer
        glBlitFramebuffer(0, 0, viewportWidth, viewportHeight, 0, 0, viewportWidth, viewportHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // TODO: Shape(s) rendering
        //if (pointMode)
        //{
        //    simpleShader.use();
        //    glUniformMatrix4fv(glGetUniformLocation(simpleShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        //    glUniformMatrix4fv(glGetUniformLocation(simpleShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        //    for (int i = 0; i < Light::lightPointList.size(); i++)
        //    {
        //        glUniform4f(glGetUniformLocation(simpleShader.ID, "lightColor"), Light::lightPointList[i].getLightColor().r, Light::lightPointList[i].getLightColor().g, Light::lightPointList[i].getLightColor().b, Light::lightPointList[i].getLightColor().a);

        //        if (Light::lightPointList[i].isMesh())
        //            Light::lightPointList[i].lightMesh.drawShape(simpleShader, view, projection, camera);
        //    }
        //}
        glQueryCounter(queryIDForward[1], GL_TIMESTAMP);    // Stop timer for non PBR rendering


#pragma region ImGUI Panels
        // -----------
        // ImGUI Setup
        // -----------
        
        
        //New ImGUI Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();


        Entity* ptrToSelectedEntity = &scene;
        float entityPosition[3];
        float entityRotation[3];
        float entityScale[3];


        //Setting Position for selected entity
        entityPosition[0] = ptrToSelectedEntity->transform.getLocalPosition().x;
        entityPosition[1] = ptrToSelectedEntity->transform.getLocalPosition().y;
        entityPosition[2] = ptrToSelectedEntity->transform.getLocalPosition().z;


        //Setting Roatation for selected entity
        entityRotation[0] = ptrToSelectedEntity->transform.getLocalRotation().x;
        entityRotation[1] = ptrToSelectedEntity->transform.getLocalRotation().y;
        entityRotation[2] = ptrToSelectedEntity->transform.getLocalRotation().z;


        //Setting Scale for selected entity
        entityScale[0] = ptrToSelectedEntity->transform.getLocalScale().x;
        entityScale[1] = ptrToSelectedEntity->transform.getLocalScale().y;
        entityScale[2] = ptrToSelectedEntity->transform.getLocalScale().z;


        // ----------
        // 0. Docking
        // ----------
        bool open = true;
        bool* p_open = &open;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;


        // ---------------------------------------------------------------------------------------------
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        // ---------------------------------------------------------------------------------------------
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }


        // ---------------------------------------------------------------------------------------------
        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        // ---------------------------------------------------------------------------------------------
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;


        // ---------------------------------------------------------------------------------------------
        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        // ---------------------------------------------------------------------------------------------
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", p_open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();
        if (opt_fullscreen)
            ImGui::PopStyleVar(2);


        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        // --------
        // Menu Bar
        // --------
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // ---------------------------------------------------------------------------------------------
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                // ---------------------------------------------------------------------------------------------
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();


                if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();


                if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                    *p_open = false;
                ImGui::EndMenu();
            }


            ImGui::EndMenuBar();
        }


        // --------------------
        // 1. Scene Graph Panel
        // --------------------
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGuiTreeNodeFlags node_flags = 0;


            ImGui::Begin("Hierarchy");


            if (ImGui::Button("Add Entity +", {150.0f, 25.0f}))
                ImGui::OpenPopup("my_select_popup");
            if (ImGui::BeginPopup("my_select_popup"))
            {
                ImGui::Dummy({ 100.0f, 0.0f });

                if (ImGui::MenuItem("Planet")) {
                    scene.addChild(planetModel, "New Planet");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }
                if (ImGui::MenuItem("Rock")) {
                    scene.addChild(rockModel, "New Rock");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }
                if (ImGui::MenuItem("Sponza")) {
                    scene.addChild(sponzaModel, "New Sponza");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }
                if (ImGui::MenuItem("Backpack")) {
                    scene.addChild(backPackModel, "New Backpack");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }
                if (ImGui::MenuItem("PointLight")) {
                    scene.addChild(true, "New PointLight");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }
                if (ImGui::MenuItem("Dinosaur")) {
                    scene.addChild(dinosaurModel, "New Dinosaur");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }
                ImGui::EndPopup();
            }
            ImGui::Spacing();


            // ---------------
            // Scene Hierarchy   : Populate Scene Hierarchy panel from the scene entity parent reference
            // ---------------
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Scene Hierarchy"))
            {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);


                putEntityInSceneHierarchyPanel(scene, ptrToSelectedEntity);


                // Setting Position
                entityPosition[0] = ptrToSelectedEntity->transform.getLocalPosition().x;
                entityPosition[1] = ptrToSelectedEntity->transform.getLocalPosition().y;
                entityPosition[2] = ptrToSelectedEntity->transform.getLocalPosition().z;


                // Setting Roatation
                entityRotation[0] = ptrToSelectedEntity->transform.getLocalRotation().x;
                entityRotation[1] = ptrToSelectedEntity->transform.getLocalRotation().y;
                entityRotation[2] = ptrToSelectedEntity->transform.getLocalRotation().z;


                // Setting Scale
                entityScale[0] = ptrToSelectedEntity->transform.getLocalScale().x;
                entityScale[1] = ptrToSelectedEntity->transform.getLocalScale().y;
                entityScale[2] = ptrToSelectedEntity->transform.getLocalScale().z;


            }


            ImGui::End();
        }

        // ------------------------
        // 2. Object Property Panel
        // ------------------------
        {
            static float f = 0.0f;
            static int counter = 0;


            ImGui::Begin("Entity Properties");


            ImGui::Text("[ %s ]", ptrToSelectedEntity->entityName);

            
            // ----------------------------
            // Setting New Entity Transform
            // ----------------------------
            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Transform"))
            {
                ImGui::Indent();
                ImGui::DragFloat3("Position", entityPosition, 0.05f, -255.0f, 255.0f);                                                  // Position
                ptrToSelectedEntity->transform.setLocalPosition(glm::vec3(entityPosition[0], entityPosition[1], entityPosition[2]));
                ImGui::DragFloat3("Rotation", entityRotation, 0.1f, -255.0f, 255.0f);                                                   // Rotation
                ptrToSelectedEntity->transform.setLocalRotation(glm::vec3(entityRotation[0], entityRotation[1], entityRotation[2]));
                ImGui::DragFloat3("Scale", entityScale, 0.05f, -255.0f, 255.0f);                                                        // Scale
                ptrToSelectedEntity->transform.setLocalScale(glm::vec3(entityScale[0], entityScale[1], entityScale[2]));
                ImGui::Unindent();
            }
            ImGui::Spacing();


            // ----------------
            // Light Properties
            // ----------------
            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ptrToSelectedEntity->pointLight.isEntityLight)
            {
                if (ImGui::CollapsingHeader("Light Properties"))
                {
                    ImGui::Indent();
                    ImGui::ColorEdit4("Color", (float*)&ptrToSelectedEntity->pointLight.color, 0);                              // Color
                    ImGui::DragFloat("Intensity", (float*)&ptrToSelectedEntity->pointLight.intensity, 0.05f, 0.0f, 100.0f);     // Intensity
                    ImGui::DragFloat("Radius", (float*)&ptrToSelectedEntity->pointLight.radius, 0.05f, 0.0f, 100.0f);           // Radius
                    ImGui::Unindent();
                }
            }
            ImGui::End();
        }


        // -------------------
        // 3. World Properties
        // -------------------
        {
            ImGui::Begin("World Properties");


            // -----------------
            // Directional Light
            // -----------------
            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Directional Light"))
            {
                ImGui::Indent();
                ImGui::DragFloat3("Direction", (float*)&lightDirectionalDirection1, 0.01f, -1, 1);      // Direction
                ImGui::ColorEdit4("Color", (float*)&lightDirectionalColor1, 0);                         // Color
                ImGui::DragFloat("Intensity", (float*)&directionalLightIntensity, 0.05f, 0.0f, 100.0f); // Intensity
                ImGui::Unindent();
            }
            ImGui::Spacing();


            // ---------------
            // Environment Map
            // ---------------
            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Environment map"))
            {
                ImGui::Indent();
                if (ImGui::Button("Appartment", {150.0f, 25.0f}))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/appart.hdr", "appartHDR", true);    // Apartment
                    iblSetup();
                }
                if (ImGui::Button("Pisa", { 150.0f, 25.0f }))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/pisa.hdr", "pisaHDR", true);        // Pisa
                    iblSetup();
                }
                if (ImGui::Button("Canyon", { 150.0f, 25.0f }))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/canyon.hdr", "canyonHDR", true);    // Canyon
                    iblSetup();
                }
                if (ImGui::Button("Loft", { 150.0f, 25.0f }))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/loft.hdr", "loftHDR", true);        // Loft
                    iblSetup();
                }
                if (ImGui::Button("Path", { 150.0f, 25.0f }))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/path.hdr", "pathHDR", true);        // Path
                    iblSetup();
                }
                if (ImGui::Button("Circus", { 150.0f, 25.0f }))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/circus.hdr", "circusHDR", true);    // Circus
                    iblSetup();
                }
                if (ImGui::Button("Hills", { 150.0f, 25.0f }))
                {
                    envMapHDR.setTextureHDR("resources/textures/hdr/hills.hdr", "hillsHDR", true);      // Hills
                    iblSetup();
                }
                ImGui::Unindent();
            }
            ImGui::Spacing();


            // ------------------------
            // Post Processing Settings
            // ------------------------
            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Post Processing"))
            {
                ImGui::Indent();
                ImGui::Checkbox("Ambient Occlusion", (bool*)&saoMode);      // SAO
                ImGui::Checkbox("FXAA", (bool*)&fxaaMode);                  // FXAA
                if (ImGui::TreeNode("Tonemapping"))                         // Tonemapping
                {
                    ImGui::RadioButton("Reinhard", &tonemappingMode, 1);
                    ImGui::RadioButton("Filmic", &tonemappingMode, 2);
                    ImGui::RadioButton("Uncharted", &tonemappingMode, 3);


                    ImGui::TreePop();
                }
                ImGui::Unindent();
            }
            ImGui::Spacing();


            ImGui::End();
        }

        // -----------
        // 4. Viewport
        // -----------
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
            ImGui::Begin("Viewport");
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();


            if (viewportHeight != viewportPanelSize.y || viewportWidth != viewportPanelSize.x) 
            {
                viewportWidth = viewportPanelSize.x;
                viewportHeight = viewportPanelSize.y;


                //--------------------
                // Render Buffer setup
                //--------------------
                gBufferSetup();
                saoSetup();
                postprocessSetup();
                screenSetup();
                iblSetup();


                printf("Window: %d, %d\n", SCR_WIDTH, SCR_HEIGHT);
                printf("Viewport: %d, %d\n\n", viewportWidth, viewportHeight);
            }


            if (ImGui::IsWindowFocused())
                mouseHoveringViewport = true;
            else
                mouseHoveringViewport = false;


            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            ImGui::Image((void*)screenBuffer, ImVec2{ (float)viewportWidth, (float)viewportHeight }, ImVec2(0, 1), ImVec2(1, 0));


            // ------
            // Gizmos
            // ------
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();


            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportWidth, viewportHeight);


            glm::mat4 transform = ptrToSelectedEntity->transform.getTranslation();
            glm::vec3 posDiff = (glm::vec3)transform[3];
            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));
            glm::vec3 localPos = ptrToSelectedEntity->transform.getLocalPosition();
            posDiff -= (glm::vec3)transform[3];
            localPos -= posDiff;


            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                DecomposeTransform(transform, translation, rotation, scale);
                rotation += ptrToSelectedEntity->transform.getLocalRotation();


                // Setting Transform values
                ptrToSelectedEntity->transform.setLocalPosition(localPos);
                //ptrToSelectedEntity->transform.setLocalRotation(rotation);
                ptrToSelectedEntity->transform.setLocalScale(scale);
            }


            ImGui::PopStyleVar();
            ImGui::End();
        }

        
        // ------------
        // 5. Profiling
        // ------------
        {
            ImGui::Begin("Profiling");


            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Profiling"))
            {
                ImGui::Indent();
                ImGui::Text("Geometry Pass :    %.4f ms", deltaGeometryTime);
                ImGui::Text("Lighting Pass :    %.4f ms", deltaLightingTime);
                ImGui::Text("SAO Pass :         %.4f ms", deltaSAOTime);
                ImGui::Text("Postprocess Pass : %.4f ms", deltaPostprocessTime);
                ImGui::Text("Forward Pass :     %.4f ms", deltaForwardTime);
                ImGui::Text("GUI Pass :         %.4f ms", deltaGUITime);
                ImGui::Unindent();
            }
            ImGui::Spacing();


            ImGui::Spacing();
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            if (ImGui::CollapsingHeader("Application Info"))
            {
                char* glInfos = (char*)glGetString(GL_VERSION);
                char* hardwareInfos = (char*)glGetString(GL_RENDERER);


                ImGui::Indent();
                ImGui::Text("OpenGL Version :");
                ImGui::Text(glInfos);
                ImGui::Text("Hardware Informations :");
                ImGui::Text(hardwareInfos);
                ImGui::Text("\nFramerate %.2f FPS / Frametime %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
                ImGui::Unindent();
            }


            ImGui::End();
        }


        // Close Dockspace
        ImGui::End();


        //ImGUI Render
        glQueryCounter(queryIDGUI[0], GL_TIMESTAMP);
        ImGui::Render();
        glQueryCounter(queryIDGUI[1], GL_TIMESTAMP);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion Editor UI


        //--------------
        // GPU profiling
        //--------------
        GLint stopGeometryTimerAvailable = 0;
        GLint stopLightingTimerAvailable = 0;
        GLint stopSAOTimerAvailable = 0;
        GLint stopPostprocessTimerAvailable = 0;
        GLint stopForwardTimerAvailable = 0;
        GLint stopGUITimerAvailable = 0;


        while (!stopGeometryTimerAvailable && !stopLightingTimerAvailable && !stopSAOTimerAvailable && !stopPostprocessTimerAvailable && !stopForwardTimerAvailable && !stopGUITimerAvailable)
        {
            glGetQueryObjectiv(queryIDGeometry[1], GL_QUERY_RESULT_AVAILABLE, &stopGeometryTimerAvailable);
            glGetQueryObjectiv(queryIDLighting[1], GL_QUERY_RESULT_AVAILABLE, &stopLightingTimerAvailable);
            glGetQueryObjectiv(queryIDSAO[1], GL_QUERY_RESULT_AVAILABLE, &stopSAOTimerAvailable);
            glGetQueryObjectiv(queryIDPostprocess[1], GL_QUERY_RESULT_AVAILABLE, &stopPostprocessTimerAvailable);
            glGetQueryObjectiv(queryIDForward[1], GL_QUERY_RESULT_AVAILABLE, &stopForwardTimerAvailable);
            glGetQueryObjectiv(queryIDGUI[1], GL_QUERY_RESULT_AVAILABLE, &stopGUITimerAvailable);
        }


        glGetQueryObjectui64v(queryIDGeometry[0], GL_QUERY_RESULT, &startGeometryTime);
        glGetQueryObjectui64v(queryIDGeometry[1], GL_QUERY_RESULT, &stopGeometryTime);
        glGetQueryObjectui64v(queryIDLighting[0], GL_QUERY_RESULT, &startLightingTime);
        glGetQueryObjectui64v(queryIDLighting[1], GL_QUERY_RESULT, &stopLightingTime);
        glGetQueryObjectui64v(queryIDSAO[0], GL_QUERY_RESULT, &startSAOTime);
        glGetQueryObjectui64v(queryIDSAO[1], GL_QUERY_RESULT, &stopSAOTime);
        glGetQueryObjectui64v(queryIDPostprocess[0], GL_QUERY_RESULT, &startPostprocessTime);
        glGetQueryObjectui64v(queryIDPostprocess[1], GL_QUERY_RESULT, &stopPostprocessTime);
        glGetQueryObjectui64v(queryIDForward[0], GL_QUERY_RESULT, &startForwardTime);
        glGetQueryObjectui64v(queryIDForward[1], GL_QUERY_RESULT, &stopForwardTime);
        glGetQueryObjectui64v(queryIDGUI[0], GL_QUERY_RESULT, &startGUITime);
        glGetQueryObjectui64v(queryIDGUI[1], GL_QUERY_RESULT, &stopGUITime);


        deltaGeometryTime = (stopGeometryTime - startGeometryTime) / 1000000.0;
        deltaLightingTime = (stopLightingTime - startLightingTime) / 1000000.0;
        deltaSAOTime = (stopSAOTime - startSAOTime) / 1000000.0;
        deltaPostprocessTime = (stopPostprocessTime - startPostprocessTime) / 1000000.0;
        deltaForwardTime = (stopForwardTime - startForwardTime) / 1000000.0;
        deltaGUITime = (stopGUITime - startGUITime) / 1000000.0;


        // -------------------------------------------------------------------------------
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //ImGUI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // ------------------------------------------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void gBufferSetup()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Albedo + Roughness
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportWidth, viewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo, 0);

    // Normals + Metalness
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

    // Effects (AO + Velocity)
    glGenTextures(1, &gEffects);
    glBindTexture(GL_TEXTURE_2D, gEffects);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEffects, 0);

    // Define the COLOR_ATTACHMENTS for the G-Buffer
    GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    // Z-Buffer
    glGenRenderbuffers(1, &zBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewportWidth, viewportHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

    // Check if the framebuffer is complete before continuing
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete !" << std::endl;
}


void saoSetup()
{
    // SAO Buffer
    glGenFramebuffers(1, &saoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
    glGenTextures(1, &saoBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Framebuffer not complete !" << std::endl;

    // SAO Blur Buffer
    glGenFramebuffers(1, &saoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);
    glGenTextures(1, &saoBlurBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, viewportWidth, viewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBlurBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Blur Framebuffer not complete !" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void screenSetup()
{
    // Post-processing Buffer
    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    glGenTextures(1, &screenBuffer);
    glBindTexture(GL_TEXTURE_2D, screenBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Postprocess Framebuffer not complete !" << std::endl;
}


void postprocessSetup()
{
    // Post-processing Buffer
    glGenFramebuffers(1, &postprocessFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);

    glGenTextures(1, &postprocessBuffer);
    glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewportWidth, viewportHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Postprocess Framebuffer not complete !" << std::endl;
}


void iblSetup()
{
    // Latlong to Cubemap conversion
    glGenFramebuffers(1, &envToCubeFBO);
    glGenRenderbuffers(1, &envToCubeRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, envToCubeFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, envToCubeRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapCube.getTexWidth(), envMapCube.getTexHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, envToCubeRBO);

    latlongToCubeShader.use();

    glUniformMatrix4fv(glGetUniformLocation(latlongToCubeShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(envMapProjection));
    glActiveTexture(GL_TEXTURE0);
    envMapHDR.useTexture();

    glViewport(0, 0, envMapCube.getTexWidth(), envMapCube.getTexHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, envToCubeFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(latlongToCubeShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(envMapView[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapCube.getTexID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        envCubeRender.drawShape();
    }

    envMapCube.computeTexMipmap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Diffuse irradiance capture
    glGenFramebuffers(1, &irradianceFBO);
    glGenRenderbuffers(1, &irradianceRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, irradianceRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapIrradiance.getTexWidth(), envMapIrradiance.getTexHeight());

    irradianceIBLShader.use();

    glUniformMatrix4fv(glGetUniformLocation(irradianceIBLShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(envMapProjection));
    glActiveTexture(GL_TEXTURE0);
    envMapCube.useTexture();

    glViewport(0, 0, envMapIrradiance.getTexWidth(), envMapIrradiance.getTexHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(irradianceIBLShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(envMapView[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapIrradiance.getTexID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        envCubeRender.drawShape();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Prefilter cubemap
    prefilterIBLShader.use();

    glUniformMatrix4fv(glGetUniformLocation(prefilterIBLShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(envMapProjection));
    envMapCube.useTexture();

    glGenFramebuffers(1, &prefilterFBO);
    glGenRenderbuffers(1, &prefilterRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, prefilterFBO);

    unsigned int maxMipLevels = 5;

    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = envMapPrefilter.getTexWidth() * std::pow(0.5, mip);
        unsigned int mipHeight = envMapPrefilter.getTexHeight() * std::pow(0.5, mip);

        glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);

        glUniform1f(glGetUniformLocation(prefilterIBLShader.ID, "roughness"), roughness);
        glUniform1f(glGetUniformLocation(prefilterIBLShader.ID, "cubeResolutionWidth"), envMapPrefilter.getTexWidth());
        glUniform1f(glGetUniformLocation(prefilterIBLShader.ID, "cubeResolutionHeight"), envMapPrefilter.getTexHeight());

        for (unsigned int i = 0; i < 6; ++i)
        {
            glUniformMatrix4fv(glGetUniformLocation(prefilterIBLShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(envMapView[i]));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapPrefilter.getTexID(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            envCubeRender.drawShape();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // BRDF LUT
    glGenFramebuffers(1, &brdfLUTFBO);
    glGenRenderbuffers(1, &brdfLUTRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, brdfLUTFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, brdfLUTRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapLUT.getTexWidth(), envMapLUT.getTexHeight());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envMapLUT.getTexID(), 0);

    glViewport(0, 0, envMapLUT.getTexWidth(), envMapLUT.getTexHeight());
    integrateIBLShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quadRender.drawShape();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, viewportWidth, viewportHeight);
}

bool putEntityInSceneHierarchyPanel(Entity& entity, Entity*& ptrToSelectedEntity) 
{
    bool deletedEntity = false;                                             // Flag to skip rendering if entity deleted
    int countChildrenEntities = entity.children.size();
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow 
        | ImGuiTreeNodeFlags_OpenOnDoubleClick 
        | ImGuiTreeNodeFlags_SpanAvailWidth 
        | ImGuiTreeNodeFlags_NoTreePushOnOpen;


    ImGui::Indent();


    // ---------------------------
    // If current entity is a leaf
    // ---------------------------
    if (countChildrenEntities <= 0)                     
    {
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (entity.id == selected_hierarchy_node)                                               // If that node is selected
        {
            ptrToSelectedEntity = &entity;
            node_flags |= ImGuiTreeNodeFlags_Selected;                                              // Select that node in imgui
        }
        node_flags |= ImGuiTreeNodeFlags_Leaf;                                                  // Make the node a leaf


        ImGui::TreeNodeEx((void*)(intptr_t)entity.id, node_flags, "%s", entity.entityName);     // New Tree Child Node as a leaf


        deletedEntity = rightClickMenu(entity);                                                 // Right click menu for the leaf node rendered just before


        if (ImGui::IsItemClicked())                                                             // If the tree node create just before is clicked
        {
            ptrToSelectedEntity = &entity;                                                          // Set the pointer to selected entity as itself
            selected_hierarchy_node = entity.id;                                                    // Set the selected node id
        }


        ImGui::Unindent();                                                                      // After leaf Node - also offsets indent at beginning done when more than one leaf children
        if (deletedEntity) return true;                                                         // Skip rendering UI for one frame if it's deleted
        return false;                                                                           // Else continue rendering UI as is
    }


    ImGuiTreeNodeFlags node_flags = base_flags;
    if (entity.id == selected_hierarchy_node)
    {
        ptrToSelectedEntity = &entity;
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }


    // Render Entity in Hierarchy
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)entity.id, node_flags, "%s", entity.entityName);    // New Tree Node
    deletedEntity = rightClickMenu(entity);
    if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
    {
        ptrToSelectedEntity = &entity;
        selected_hierarchy_node = entity.id;
    }
    if (deletedEntity) return true;

    bool deletedChild = false;
    // Render Entity Children in hierarchy
    for (auto it = entity.children.begin(); it != entity.children.end(); ++it)
    {
        //printf("\n %d", it);
        if (node_open)
        {
            //ImGui::TreePush();
            // Calling a function makes it forget which tree node to push
            //nodeEntityId++;
            deletedChild = putEntityInSceneHierarchyPanel(**it, ptrToSelectedEntity);
            if (deletedChild) break; // if my child is deleted i have to reset the process - i cannot with my previous values of 'it'
        }
    }
    ImGui::Unindent(); // Unindent after tree finishes - multible indentations are done to reach the bottom leaf 
    if (deletedChild) return true;
    return false;
}

bool rightClickMenu(Entity& m_entity) {
    bool deleted = false;
    // Right Click Menu
    ImGui::PushID(m_entity.id);
    if (ImGui::BeginPopupContextItem("my_item_popup"))
    {
        if (ImGui::BeginMenu("Add"))
        {
            ImGui::Dummy({ 100.0f, 0.0f });

            if (ImGui::MenuItem("Planet")) {
                m_entity.addChild(planetModel, "New Planet");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            if (ImGui::MenuItem("Rock")) {
                m_entity.addChild(rockModel, "New Rock");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            if (ImGui::MenuItem("Sponza")) {
                m_entity.addChild(sponzaModel, "New Sponza");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            if (ImGui::MenuItem("Backpack")) {
                m_entity.addChild(backPackModel, "New Backpack");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            if (ImGui::MenuItem("PointLight")) {
                m_entity.addChild(true, "New PointLight");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            if (ImGui::MenuItem("Dinosaur")) {
                m_entity.addChild(dinosaurModel, "New Dinosaur");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::Spacing();

        // Rename
        // ------
        if (ImGui::Button("Rename", ImVec2(100.0f, 0.0f)))
            ImGui::OpenPopup("Rename Entity");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Rename Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::InputText("New Name", m_entity.entityName, IM_ARRAYSIZE(m_entity.entityName));
            ImGui::Spacing();
            if (ImGui::Button("Done", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        // Delete
        // ------
        if (ImGui::Button("Delete", ImVec2(100.0f, 0.0f)))
            ImGui::OpenPopup("Delete?");

        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s will be deleted\nThis operation cannot be undone!\n\n", m_entity.entityName);
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                Entity* parent = m_entity.parent;
                for (auto it = parent->children.begin(); it != parent->children.end(); ++it)
                {
                    if (**it == m_entity)
                    {
                        parent->children.remove(*it); // removes unique_ptr element that points at the element1
                        //cannot increment iterator after removing?
                        deleted = true;
                        selected_hierarchy_node = parent->id;
                        break;
                    }
                }
            }
            ImGui::SetItemDefaultFocus();

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();

            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopID();
    return deleted;
}

#pragma region Input Processing

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && mouseDragEnabled)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && mouseDragEnabled)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && mouseDragEnabled)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && mouseDragEnabled)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    //Gizmos
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !mouseDragEnabled)
        gizmoType = ImGuizmo::OPERATION::TRANSLATE;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !mouseDragEnabled)
        gizmoType = ImGuizmo::OPERATION::ROTATE;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !mouseDragEnabled)
        gizmoType = ImGuizmo::OPERATION::SCALE;

    if ((glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS))
        gBufferView = 1;

    if ((glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS))
        gBufferView = 2;

    if ((glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS))
        gBufferView = 3;

    if ((glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS))
        gBufferView = 4;

    if ((glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS))
        gBufferView = 5;

    if ((glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS))
        gBufferView = 6;

    if ((glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS))
        gBufferView = 7;

    if ((glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS))
        gBufferView = 8;

    if ((glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS))
        gBufferView = 9;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes`
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (mouseDragEnabled)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset, mouseDragEnabled);
}

// glfw: whenever the mouse button is pressed, this callback is called
// -------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS && mouseHoveringViewport)
        {
            firstMouse = true; // Prevents Jittering
            mouseDragEnabled = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseDragEnabled = false;
        }
    }
}

#pragma endregion

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (
        epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
    Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
    if (dot(Row[0], Pdum3) < 0)
    {
        for (length_t i = 0; i < 3; i++)
        {
            scale[i] *= static_cast<T>(-1);
            Row[i] *= static_cast<T>(-1);
        }
    }
#endif

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }


    return true;
}
