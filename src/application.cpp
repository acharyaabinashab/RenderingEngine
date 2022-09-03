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
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/string_cast.hpp>
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
    std::cout << FileSystem::getPath("resources/objects/planet/planet.obj") << "\n";
    planetModel = Model(FileSystem::getPath("resources/objects/planet/planet.obj"));
    rockModel = Model(FileSystem::getPath("resources/objects/rock/rock.obj"));
    sponzaModel = Model(FileSystem::getPath("resources/objects/sponza/sponza.obj"));
    backPackModel = Model(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    std::cout << "Loaded all Models" << "\n";
    //sibenikModel = Model(FileSystem::getPath("resources/objects/sibenik/sibenik.obj"));

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // imgui: setup
    // ------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
            lastEntity->addChild(true, "New Light");
            lastEntity->addChild(model, "New Child");
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

    Model backpack(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    // configure g-buffer framebuffer
    // ------------------------------
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec, gMetallic, gAo, gFinalFrame;
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
    // color + specular color buffer
    glGenTextures(1, &gFinalFrame);
    glBindTexture(GL_TEXTURE_2D, gFinalFrame);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gFinalFrame, 0);

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

        // draw our Scene Graph
        unsigned int total = 0, display = 0;
        scene.drawSelfAndChild(camFrustum, shaderGeometryPass, display, total);
        //std::cout << "Total process in CPU : " << total << " / Total send to GPU : " << display << std::endl;

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

        // shader configuration
         // --------------------
        shaderLightingPass.setInt("gPosition", 0);
        shaderLightingPass.setInt("gNormal", 1);
        shaderLightingPass.setInt("gAlbedoSpec", 2);
        
        // send light relevant uniforms
        unsigned int totalLights = 0;
        scene.drawPointLights(shaderLightingPass, totalLights);
        shaderLightingPass.setVec3("viewPos", camera.Position);
        // finally render quad
        renderQuad();


        

        #pragma region ImGUI Panels
        //ImGUI Setup
        // --------------------------------------------------------------------------------
        //New ImGUI Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

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

        // 0. Docking
        bool open = true;
        bool *p_open = &open;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
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

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
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
                if (ImGui::MenuItem("PointLight")) {
                    scene.addChild(true, "New PointLight");
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

        // 3. Viewport
        {
            ImGui::Begin("Viewport");

            ImGui::Image((void*)gAlbedoSpec, ImVec2{ 256.0f, 256.0f });

            // Gizmos
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float windowHeight = (float)ImGui::GetWindowHeight();
            float windowWidth = (float)ImGui::GetWindowWidth();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowHeight, windowWidth);

            glm::mat4 transform = ptrToSelectedEntity->transform.getTranslation();
            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform));

            ImGui::End();
        }

        ImGui::ShowDemoWindow();

        // Close Dockspace
        ImGui::End();

        //ImGUI Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #pragma endregion Editor UI





        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
        // ----------------------------------------------------------------------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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