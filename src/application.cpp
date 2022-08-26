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
int countSceneEntities(Entity& scene);
void putEntityInSceneHierarchyPanel(Entity& parent, int& entityId, Entity* &ptrToArray);

// settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseDragEnabled = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    // ======================================================== this needed?
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // build and compile shaders
    // -------------------------
    Shader ourShader("src/1.model_loading.vs", "src/1.model_loading.fs");

    // load entities
    // -----------
    Model model = Model(FileSystem::getPath("resources/objects/planet/planet.obj"));
    Entity scene(model, "Scene");
    scene.transform.setLocalPosition({ 0, 0, -10 });
    const float scale = 0.75;
    scene.transform.setLocalScale({ scale, scale, scale });

    {
        Entity* lastEntity = &scene;

        for (unsigned int i = 0; i < 10; ++i)
        {
            lastEntity->addChild(model);
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

    scene.addChild(model);
    scene.children.back().get()->transform.setLocalPosition({ 0,0,0 });

    // BUG: Not all children being rendered - only one child at back being rendered recursively
    scene.updateSelfAndChild();

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
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

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // draw our scene graph
        //Entity* lastEntity = &scene;
        //while (lastEntity->children.size())
        //{
        //    ourShader.setMat4("model", lastEntity->transform.getModelMatrix());
        //    lastEntity->pModel->Draw(ourShader);
        //    lastEntity = lastEntity->children.back().get(); // Only draws one of the child - add functionality to draw all
        //}

        drawSceneGraph(scene, ourShader);

        //scene.transform.setLocalRotation({ 0.f, scene.transform.getLocalRotation().y + 20 * deltaTime, 0.f });
        scene.updateSelfAndChild();

        #pragma region ImGUI Panels
        //ImGUI Setup
        // --------------------------------------------------------------------------------
        //New ImGUI Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Entity* ptrToEntity = nullptr;
        float entityPosition[3];
        float entityRotation[3];
        float entityScale[3];

        // 1. Scene Graph Panel
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGuiTreeNodeFlags node_flags = 0;

            ImGui::Begin("Hierarchy");                // Create a window called "Hello, world!" and append into it.

            ImGui::Button("Add New");
           
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);

            if(ImGui::CollapsingHeader("Scene Hierarchy"))
            {
                // Populate Scene Hierarchy panel from the scene entity parent reference
                int entityId = 0; // this can also be used for no of visible entities in the hierarchy after the function below
                const int totalEntities = countSceneEntities(scene) + 1;

                
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                putEntityInSceneHierarchyPanel(scene, entityId, ptrToEntity);
                
                //Setting Position
                entityPosition[0] = ptrToEntity->transform.getLocalPosition().x;
                entityPosition[1] = ptrToEntity->transform.getLocalPosition().y;
                entityPosition[2] = ptrToEntity->transform.getLocalPosition().z;
                //Setting Roatation
                entityRotation[0] = ptrToEntity->transform.getLocalRotation().x;
                entityRotation[1] = ptrToEntity->transform.getLocalRotation().y;
                entityRotation[2] = ptrToEntity->transform.getLocalRotation().z;
                //Setting Scale
                entityScale[0] = ptrToEntity->transform.getLocalScale().x;
                entityScale[1] = ptrToEntity->transform.getLocalScale().y;
                entityScale[2] = ptrToEntity->transform.getLocalScale().z;
            }

            ImGui::End();
        }

        // 2. Object Property Panel
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Property");                // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);         

            // Setting Transformation from change
            // Position
            ImGui::DragFloat3("Position", entityPosition, 0.05f, -255.0f, 255.0f);
            ptrToEntity->transform.setLocalPosition(glm::vec3(entityPosition[0], entityPosition[1], entityPosition[2]));
            // Rotation
            ImGui::DragFloat3("Rotation", entityRotation, 0.05f, -255.0f, 255.0f);
            ptrToEntity->transform.setLocalRotation(glm::vec3(entityRotation[0], entityRotation[1], entityRotation[2]));
            // Scale
            ImGui::DragFloat3("Scale", entityScale, 0.05f, -255.0f, 255.0f);
            ptrToEntity->transform.setLocalScale(glm::vec3(entityScale[0], entityScale[1], entityScale[2]));

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

int countSceneEntities(Entity &scene) {
    int countChildrenEntities = scene.children.size();
    int totalEntities = 0;

    if (countChildrenEntities <= 0)
        return 0;

    // 'it' gives the address to the pointer pointing at the first/last child of the list
    for (auto it = scene.children.begin(); it != scene.children.end(); ++it)
    {
        totalEntities += countSceneEntities(**it);
        totalEntities++;
    }
    return totalEntities;
}

void drawSceneGraph(Entity& parent, Shader& ourShader) {
    int countChildrenEntities = parent.children.size();
    ourShader.setMat4("model", parent.transform.getModelMatrix());
    if (countChildrenEntities <= 0) {
        parent.pModel->Draw(ourShader);
        return;
    }

    for (auto it = parent.children.begin(); it != parent.children.end(); ++it)
    {
        parent.pModel->Draw(ourShader);
        drawSceneGraph(**it, ourShader);
    }
}

void putEntityInSceneHierarchyPanel(Entity& parent, int &entityId, Entity* &ptrToEntity) {
    static int selected_node = 0; // select the scene on start

    int countChildrenEntities = parent.children.size();
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    ImGui::Indent();

    if (countChildrenEntities <= 0) {
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (entityId == selected_node)
        {
            ptrToEntity = &parent;
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        node_flags |= ImGuiTreeNodeFlags_Leaf; // ImGuiTreeNodeFlags_Bullet
        ImGui::TreeNodeEx((void*)(intptr_t)entityId, node_flags, "%s", parent.entityName);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            ptrToEntity = &parent;
            selected_node = entityId;
        }

        ImGui::Unindent(); // After leaf Node - also offsets indent at beginning done when more than one leaf children
        return;
    }

    ImGuiTreeNodeFlags node_flags = base_flags;
    if (entityId == selected_node)
    {
        ptrToEntity = &parent;
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)entityId, node_flags, "%s", parent.entityName);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        ptrToEntity = &parent;
        selected_node = entityId;
    }

    for (auto it = parent.children.begin(); it != parent.children.end(); ++it)
    {
        if (node_open)
        {
            //ImGui::TreePush();
            // Calling a function makes it forget which tree node to push
            entityId++;
            putEntityInSceneHierarchyPanel(**it, entityId, ptrToEntity);
        }
    }
    ImGui::Unindent(); // Unindent after tree finishes - multible indentations are done to reach the bottom leaf    
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
    camera.ProcessMouseScroll(yoffset);
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


/*
                #pragma region tree example
                ImGuiTreeNodeFlags node_flags = base_flags;
                if (0 == selected_node)
                    node_flags |= ImGuiTreeNodeFlags_Selected;

                bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)0, node_flags, "Selectable Node %d", 0);
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    selected_node = 0;

                if(node_open)
                {
                    node_flags = base_flags;
                    if (1 == selected_node)
                        node_flags |= ImGuiTreeNodeFlags_Selected;

                    node_open = ImGui::TreeNodeEx((void*)(intptr_t)1, node_flags, "Selectable Leaf %d", 1);
                    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                        selected_node = 1;

                    if(node_open)
                    {
                        node_flags = base_flags;
                        if (2 == selected_node)
                            node_flags |= ImGuiTreeNodeFlags_Selected;

                        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                        ImGui::TreeNodeEx((void*)(intptr_t)2, node_flags, "Selectable Leaf %d", 2);
                        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                            selected_node = 2;

                        ImGui::TreePop();
                    }


                    node_flags = base_flags;
                    if (3 == selected_node)
                        node_flags |= ImGuiTreeNodeFlags_Selected;

                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                    ImGui::TreeNodeEx((void*)(intptr_t)3, node_flags, "Selectable Leaf %d", 3);
                    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                        selected_node = 3;

                    ImGui::TreePop();
                }
                #pragma endregion
                */