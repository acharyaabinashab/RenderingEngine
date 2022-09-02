#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/entity.h>

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

void drawSceneGraph(Entity& parent, Shader& ourShader);
bool putEntityInSceneHierarchyPanel(Entity& parent, Entity* &ptrToSelectedEntity);
bool rightClickMenu(Entity& m_entity);

void renderQuad();
void renderCube();

// settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 6.0f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseDragEnabled = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Addable Objects
Model planetModel;
Model rockModel;
Model sponzaModel;
Model backPackModel;
//Model sibenikModel;

int selected_hierarchy_node = 0; // select the scene on start

int main()
{
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSwapInterval(0);

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // It is initialized here because it needs the glad loader to finish for it to work
    // Also doesn't work when this is called from outside the function from where it is declared
    planetModel = Model(FileSystem::getPath("resources/objects/planet/planet.obj"));
    rockModel = Model(FileSystem::getPath("resources/objects/rock/rock.obj"));
    sponzaModel = Model(FileSystem::getPath("resources/objects/sponza/sponza.obj"));
    backPackModel = Model(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    //sibenikModel = Model(FileSystem::getPath("resources/objects/sibenik/sibenik.obj"));

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // imgui: setup
    // ------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

    // build and compile shaders
    // -------------------------
    Shader ourShader("src/1.model_loading.vs", "src/1.model_loading.fs");

    // load entities
    // -----------
    Model model = Model(FileSystem::getPath("resources/objects/planet/planet.obj")); // Works only after glad is loaded
    Entity scene = Entity("Scene Root");
    scene.transform.setLocalPosition({ 0, 0, 0 });
    
    const float scale = 0.75f;
    {
        scene.addChild(planetModel, "Planets");
        Entity* lastEntity = scene.children.back().get();
        lastEntity->transform.setLocalScale({ 0.25f, 0.25f, 0.25f });

        for (unsigned int i = 0; i < 10; ++i)
        {
            lastEntity->addChild(model, "New Child");
            lastEntity->addChild(model);
            //Set tranform values
            lastEntity->children.front().get()->transform.setLocalPosition({ -10, 0, 0 });
            lastEntity->children.front().get()->transform.setLocalScale({ scale, scale, scale });

            //Set tranform values
            lastEntity->children.back().get()->transform.setLocalPosition({ -10, 4, 0 });
            lastEntity->children.back().get()->transform.setLocalScale({ scale, scale, scale });

            lastEntity = lastEntity->children.back().get();
        }
    }

    scene.addChild(sponzaModel, "Sponza Environment");


    // build and compile shaders
    // -------------------------
    Shader shaderGeometryPass("src/8.2.g_buffer.vs", "src/8.2.g_buffer.fs");
    Shader shaderLightingPass("src/8.2.deferred_shading.vs", "src/8.2.deferred_shading.fs");
    Shader shaderLightBox("src/8.2.deferred_light_box.vs", "src/8.2.deferred_light_box.fs");

    // load models
    // -----------
    Model backpack(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, 0.5, -3.0));
    objectPositions.push_back(glm::vec3(0.0, 0.5, -3.0));
    objectPositions.push_back(glm::vec3(3.0, 0.5, -3.0));
    objectPositions.push_back(glm::vec3(-3.0, 0.5, 0.0));
    objectPositions.push_back(glm::vec3(0.0, 0.5, 0.0));
    objectPositions.push_back(glm::vec3(3.0, 0.5, 0.0));
    objectPositions.push_back(glm::vec3(-3.0, 0.5, 3.0));
    objectPositions.push_back(glm::vec3(0.0, 0.5, 3.0));
    objectPositions.push_back(glm::vec3(3.0, 0.5, 3.0));

    // configure g-buffer framebuffer
    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec, gMetallic, gAo;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer + metal
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    //// metallic
    //glGenTextures(1, &gMetallic);
    //glBindTexture(GL_TEXTURE_2D, gMetallic);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R, SCR_WIDTH, SCR_HEIGHT, 0, GL_R, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gMetallic, 0);
    //// ao
    //glGenTextures(1, &gAo);
    //glBindTexture(GL_TEXTURE_2D, gAo);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R, SCR_WIDTH, SCR_HEIGHT, 0, GL_R, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    const unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
        float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }

    // shader configuration
    // --------------------
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);


    // -------------------------------------------------------------------------------------------------------------------------------------------------------
    // Render Loop
    // -------------------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
		// --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Update model transforms changed in previouse frame
        scene.updateSelfAndChild();

        // render
        // ------
        // 1. geometry pass: render scene's geometry/color data into gbuffer
        // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Camera Setup
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        const Frustum camFrustum = createFrustumFromCamera(camera, (float)SCR_WIDTH / (float)SCR_HEIGHT, glm::radians(camera.Zoom), 0.1f, 100.0f);

        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int i = 0; i < objectPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.25f));
            shaderGeometryPass.setMat4("model", model);
            backpack.Draw(shaderGeometryPass);
        }

        // draw our Scene Graph
        unsigned int total = 0, display = 0;
        scene.drawSelfAndChild(camFrustum, shaderGeometryPass, display, total);
        std::cout << "Total process in CPU : " << total << " / Total send to GPU : " << display << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        // send light relevant uniforms
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            // update attenuation parameters and calculate radius
            const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            const float linear = 0.7f;
            const float quadratic = 1.8f;
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
        }
        shaderLightingPass.setVec3("viewPos", camera.Position);
        // finally render quad
        renderQuad();

        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 3. render lights on top of scene
        // --------------------------------
        shaderLightBox.use();
        shaderLightBox.setMat4("projection", projection);
        shaderLightBox.setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            shaderLightBox.setMat4("model", model);
            shaderLightBox.setVec3("lightColor", lightColors[i]);
            renderCube();
        }
        

        #pragma region ImGUI Panels
        //ImGUI Setup
        // --------------------------------------------------------------------------------
        //New ImGUI Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Entity* ptrToSelectedEntity = &scene;
        float entityPosition[3];
        float entityRotation[3];
        float entityScale[3];
        //Setting Position
        entityPosition[0] = ptrToSelectedEntity->transform.getLocalPosition().x;
        entityPosition[1] = ptrToSelectedEntity->transform.getLocalPosition().y;
        entityPosition[2] = ptrToSelectedEntity->transform.getLocalPosition().z;
        //Setting Roatation
        entityRotation[0] = ptrToSelectedEntity->transform.getLocalRotation().x;
        entityRotation[1] = ptrToSelectedEntity->transform.getLocalRotation().y;
        entityRotation[2] = ptrToSelectedEntity->transform.getLocalRotation().z;
        //Setting Scale
        entityScale[0] = ptrToSelectedEntity->transform.getLocalScale().x;
        entityScale[1] = ptrToSelectedEntity->transform.getLocalScale().y;
        entityScale[2] = ptrToSelectedEntity->transform.getLocalScale().z;

        // 1. Scene Graph Panel
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGuiTreeNodeFlags node_flags = 0;

            ImGui::Begin("Hierarchy");                // Create a window called "Hello, world!" and append into it.
            
            // Simple selection popup (if you want to show the current selection inside the Button itself,
            // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
            if (ImGui::Button("Add Entity +"))
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
                    scene.addChild(backpack, "New Backpack");
                    selected_hierarchy_node = scene.children.back().get()->id;
                }

                ImGui::EndPopup();
            }

            ImGui::SameLine();
            ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
            ImGui::Spacing();
           
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);

            if(ImGui::CollapsingHeader("Scene Hierarchy"))
            {
                // Populate Scene Hierarchy panel from the scene entity parent reference
                int entityNodeId = 0; // this can also be used for no of visible entities in the hierarchy after the function below

                
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                putEntityInSceneHierarchyPanel(scene, ptrToSelectedEntity);
                
                //Setting Position
                entityPosition[0] = ptrToSelectedEntity->transform.getLocalPosition().x;
                entityPosition[1] = ptrToSelectedEntity->transform.getLocalPosition().y;
                entityPosition[2] = ptrToSelectedEntity->transform.getLocalPosition().z;
                //Setting Roatation
                entityRotation[0] = ptrToSelectedEntity->transform.getLocalRotation().x;
                entityRotation[1] = ptrToSelectedEntity->transform.getLocalRotation().y;
                entityRotation[2] = ptrToSelectedEntity->transform.getLocalRotation().z;
                //Setting Scale
                entityScale[0] = ptrToSelectedEntity->transform.getLocalScale().x;
                entityScale[1] = ptrToSelectedEntity->transform.getLocalScale().y;
                entityScale[2] = ptrToSelectedEntity->transform.getLocalScale().z;
            }

            ImGui::End();
        }

        // 2. Object Property Panel
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Transform");                // Create a window called "Hello, world!" and append into it. 

            ImGui::Text("%s", ptrToSelectedEntity->entityName);

            // Setting Transformation from change
            // Position
            ImGui::DragFloat3("Position", entityPosition, 0.05f, -255.0f, 255.0f);
            ptrToSelectedEntity->transform.setLocalPosition(glm::vec3(entityPosition[0], entityPosition[1], entityPosition[2]));
            // Rotation
            ImGui::DragFloat3("Rotation", entityRotation, 0.05f, -255.0f, 255.0f);
            ptrToSelectedEntity->transform.setLocalRotation(glm::vec3(entityRotation[0], entityRotation[1], entityRotation[2]));
            // Scale
            ImGui::DragFloat3("Scale", entityScale, 0.05f, -255.0f, 255.0f);
            ptrToSelectedEntity->transform.setLocalScale(glm::vec3(entityScale[0], entityScale[1], entityScale[2]));

            ImGui::End();
        }

        ImGui::ShowDemoWindow();

        //ImGUI Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #pragma endregion Editor UI

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //ImGUI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool putEntityInSceneHierarchyPanel(Entity& parent, Entity*& ptrToSelectedEntity) {
    bool deletedEntity = false;

    int countChildrenEntities = parent.children.size();
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    ImGui::Indent();

    if (countChildrenEntities <= 0) {
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (parent.id == selected_hierarchy_node)
        {
            ptrToSelectedEntity = &parent;
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        node_flags |= ImGuiTreeNodeFlags_Leaf; // ImGuiTreeNodeFlags_Bullet
        ImGui::TreeNodeEx((void*)(intptr_t)parent.id, node_flags, "%s", parent.entityName);
        deletedEntity = rightClickMenu(parent);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            ptrToSelectedEntity = &parent;
            selected_hierarchy_node = parent.id;
        }
        ImGui::Unindent(); // After leaf Node - also offsets indent at beginning done when more than one leaf children
        if (deletedEntity) return true;
        return false;
    }

    ImGuiTreeNodeFlags node_flags = base_flags;
    if (parent.id == selected_hierarchy_node)
    {
        ptrToSelectedEntity = &parent;
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Render Entity in Hierarchy
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)parent.id, node_flags, "%s", parent.entityName);
    deletedEntity = rightClickMenu(parent);
    if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
    {
        ptrToSelectedEntity = &parent;
        selected_hierarchy_node = parent.id;
    }
    if (deletedEntity) return true;

    bool deletedChild = false;
    // Render Entity Children in hierarchy
    for (auto it = parent.children.begin(); it != parent.children.end(); ++it)
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
            if (ImGui::MenuItem("Planet"))
            {
                // using planetModel declaration here and populating the entity with it causes the entity to not load that model correctly (says doesnt have path in debugger)
                m_entity.addChild(planetModel, "New Planet");
                selected_hierarchy_node = m_entity.children.back().get()->id;
            }
            if (ImGui::MenuItem("Rock")) {
                m_entity.addChild(rockModel, "New Rock");
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes`
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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

    if(mouseDragEnabled)
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
        if(action == GLFW_PRESS)
        {
            firstMouse = true; // Prevents Jittering
            mouseDragEnabled = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if(action == GLFW_RELEASE)
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


//void drawSceneGraph(Entity& parent, Shader& ourShader) {
//    int countChildrenEntities = parent.children.size();
//    ourShader.setMat4("model", parent.transform.getModelMatrix());
//    if (countChildrenEntities <= 0) {
//        parent.pModel->Draw(ourShader);
//        return;
//    }
//
//    for (auto it = parent.children.begin(); it != parent.children.end(); ++it)
//    {
//        parent.pModel->Draw(ourShader);
//        drawSceneGraph(**it, ourShader);
//    }
//}