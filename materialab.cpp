// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <GL/glew.h>

#include "materialab.h"
#include "tira/graphics_gl.h"
#include "tira/graphics/shapes/icosahedron.h"
//#include "tira/graphics/glMaterial.h"
//#include "tira/graphics/glShapes3d.h"
//#include "tira/graphics/camera.h"

#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>



GLFWwindow* window;
int WindowWidth, WindowHeight;
const char* glsl_version = "#version 130";
ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
extern ImVec2 gui_window_size;

tira::glMaterial material;
tira::glGeometry mesh;
extern Mesh mesh_type;



glm::mat4 projection;
glm::mat4 view;
glm::mat4 mvp;
tira::camera camera;


std::unordered_map<std::string, ShaderData> MaterialMap;
//std::vector<tira::glShaderUniform> UniformInfoVector;
//std::vector<void*> UniformDataVector;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if(ImGui::IsWindowFocused()) return;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        camera.OrbitFocus(0.1f, 0.0f);
        std::cout<<"LEFT PUSH"<<std::endl;
    }
}

bool mouse_dragging = false;
double mouse_pos[2];
double orbit_step = 0.01;
double fov_step = 0.01;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouse_dragging = true;
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        mouse_dragging = false;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if(mouse_dragging){
        camera.OrbitFocus(orbit_step * (mouse_pos[0] - xpos), -orbit_step * (mouse_pos[1] - ypos));
    }
    mouse_pos[0] = xpos;
    mouse_pos[1] = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.setFOV(camera.getFOV() + yoffset * fov_step);
}

void LoadMesh(Mesh mesh_id) {
    mesh.Destroy();
    switch (mesh_id) {
    case Mesh::Cube:
        mesh = tira::glGeometry::GenerateCube<float>();
        break;
    case Mesh::Sphere:
        mesh = tira::glGeometry::GenerateSphere<float>(100, 100);
        break;
    case Mesh::Icosahedron:
        //mesh = tira::glGenerateSphere<float>(10, 10);
        mesh = tira::glGeometry::GenerateIcosahedron<float>();
        break;
    case Mesh::Icosphere:
        //mesh = tira::glGenerateSphere<float>(10, 10);
        mesh = tira::glGeometry::GenerateIcosphere<float>(5, true);
        break;
    default:
        break;
    }
}

void RefreshLocalMaterialData() {
    std::vector<tira::glShaderUniform> uniform_vector = material.GetUniformList();  // get a list of uniforms from the material
    std::unordered_map<std::string, ShaderData> new_matmap;

    for (int i = 0; i < uniform_vector.size(); i++) {                               // for each uniform
        ShaderData sd;                                                              // allocate a shader data structure
        sd.location = uniform_vector[i].location;                                   // fill the shader data structure with uniform variable information
        sd.name = uniform_vector[i].name;
        sd.type = uniform_vector[i].type;
        size_t uniform_size = uniform_vector[i].size();
        std::unordered_map<std::string, ShaderData>::iterator old_u = MaterialMap.find(sd.name);    //look for the uniform variable in the old map
        if (old_u != MaterialMap.end() && old_u->second.type == sd.type) {                          // if the uniform variable exists in the OLD map
                                                                                                    // AND the data types are the same
            sd.data = old_u->second.data;                                                           // copy the pointer
            sd.mapto = old_u->second.mapto;                                                         // copy the mapping data
            old_u->second.data = NULL;                                                              // set the OLD pointer to NULL
        }
        else if (uniform_size != 0)                                                                 // otherwise IF the new uniform variable has a size
            sd.data = calloc(uniform_size, 1);                                                      // allocate space for it
        else
            sd.data = NULL;                                                                         // otherwise leave it empty (for now)
        new_matmap.emplace(sd.name, sd);
    }
    //go through every uniform variable in the OLD map
    for (std::unordered_map<std::string, ShaderData>::iterator old_u = MaterialMap.begin(); old_u != MaterialMap.end(); old_u++) {
        if (old_u->second.data != NULL)                         // if there is data allocated
            free(old_u->second.data);                           // it will now be unused so free it
    }
    MaterialMap.clear();                                        // clear the old map data
    MaterialMap = new_matmap;                                   // replace it with the new map data
}

void RefreshDeviceMaterialData() {
    material.Bind();                                            // bind the material for updates

    // for each shader uniform stored locally
    for (std::unordered_map<std::string, ShaderData>::iterator i = MaterialMap.begin(); i != MaterialMap.end(); i++) {
        switch (i->second.type) {                               // switch based on the data type
        case GL_FLOAT:
            material.SetUniform1f(i->first, *(float*)i->second.data);
            break;
        case GL_FLOAT_VEC2:
            material.SetUniform2f(i->first, ((float*)i->second.data)[0], ((float*)i->second.data)[1]);
            break;
        case GL_FLOAT_VEC3:
            material.SetUniform3f(i->first, ((float*)i->second.data)[0], ((float*)i->second.data)[1], ((float*)i->second.data)[2]);
            break;
        case GL_FLOAT_VEC4:
            material.SetUniform4f(i->first, ((float*)i->second.data)[0], ((float*)i->second.data)[1], ((float*)i->second.data)[2], ((float*)i->second.data)[3]);
            break;
        case GL_DOUBLE:
        case GL_DOUBLE_VEC2:
        case GL_DOUBLE_VEC3:
        case GL_DOUBLE_VEC4:
        case GL_INT:
            material.SetUniform1i(i->first, *(int*)i->second.data);
            break;
        case GL_INT_VEC2:
            material.SetUniform2i(i->first, ((int*)i->second.data)[0], ((int*)i->second.data)[1]);
            break;
        case GL_INT_VEC3:
            material.SetUniform3i(i->first, ((int*)i->second.data)[0], ((int*)i->second.data)[1], ((int*)i->second.data)[2]);
            break;
        case GL_INT_VEC4:
            material.SetUniform4ui(i->first, ((int*)i->second.data)[0], ((int*)i->second.data)[1], ((int*)i->second.data)[2], ((int*)i->second.data)[3]);
            break;
        case GL_UNSIGNED_INT:
            material.SetUniform1ui(i->first, *(unsigned int*)i->second.data);
            break;
        case GL_UNSIGNED_INT_VEC2:
            material.SetUniform2ui(i->first, ((unsigned int*)i->second.data)[0], ((unsigned int*)i->second.data)[1]);
            break;
        case GL_UNSIGNED_INT_VEC3:
            material.SetUniform3ui(i->first, ((unsigned int*)i->second.data)[0], ((unsigned int*)i->second.data)[1], ((unsigned int*)i->second.data)[2]);
            break;
        case GL_UNSIGNED_INT_VEC4:
            material.SetUniform4ui(i->first, ((unsigned int*)i->second.data)[0], ((unsigned int*)i->second.data)[1], ((unsigned int*)i->second.data)[2], ((unsigned int*)i->second.data)[3]);
            break;
        case GL_BOOL:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
        case GL_FLOAT_MAT2:
            material.SetUniformMat2f(i->first, *(glm::mat2*)i->second.data);
            break;
        case GL_FLOAT_MAT3:
            material.SetUniformMat3f(i->first, *(glm::mat3*)i->second.data);
            break;
        case GL_FLOAT_MAT4:
            material.SetUniformMat4f(i->first, *(glm::mat4*)i->second.data);
            break;
        case GL_FLOAT_MAT2x3:
            material.SetUniformMat2x3f(i->first, *(glm::mat2x3*)i->second.data);
            break;
        case GL_FLOAT_MAT2x4:
            material.SetUniformMat2x4f(i->first, *(glm::mat2x4*)i->second.data);
            break;
        case GL_FLOAT_MAT3x2:
            material.SetUniformMat3x2f(i->first, *(glm::mat3x2*)i->second.data);
            break;
        case GL_FLOAT_MAT3x4:
            material.SetUniformMat3x4f(i->first, *(glm::mat3x4*)i->second.data);
            break;
        case GL_FLOAT_MAT4x2:
            material.SetUniformMat4x2f(i->first, *(glm::mat4x2*)i->second.data);
            break;
        case GL_FLOAT_MAT4x3:
            material.SetUniformMat4x3f(i->first, *(glm::mat4x3*)i->second.data);
            break;
        case GL_DOUBLE_MAT2:
        case GL_DOUBLE_MAT3:
        case GL_DOUBLE_MAT4:
        case GL_DOUBLE_MAT2x3:
        case GL_DOUBLE_MAT2x4:
        case GL_DOUBLE_MAT3x2:
        case GL_DOUBLE_MAT3x4:
        case GL_DOUBLE_MAT4x2:
        case GL_DOUBLE_MAT4x3:
        default:
            break;
        }
    }
}

void CreateDefaultMaterial() {
    
    std::string VertexSource =                                  // Source code for the default vertex shader
        "# version 130\n"
        "in vec3 vertices;\n"
        "in vec3 normals;\n"
        "in vec2 texcoords;\n"
        "uniform mat4 MVP;\n"
        "out vec4 vertex_color;\n"
        "out vec3 vertex_normals;\n"
        "out vec2 vertex_texcoord;\n"        
        "void main() {\n"
        "    gl_Position = MVP * vec4(vertices.x, vertices.y, vertices.z, 1.0f);\n"
        "    vertex_color = vec4(vertices.x + 0.5, vertices.y + 0.5, vertices.z + 0.5, 1.0);\n"
        "    vertex_texcoord = texcoords;\n"
        "    vertex_normals = normals;\n"
        "};\n";

    
    std::string FragmentSource =                               // Source code for the default fragment shader
        "# version 130\n"
        "out vec4 color;\n"
        "in vec4 vertex_color;\n"
        "in vec3 vertex_normals;\n"
        "in vec2 vertex_texcoord;\n"
        "uniform sampler2D texmap;\n"
        "void main() {\n"
        "    color = vertex_color * texture(texmap, vertex_texcoord).r;\n"
        "};\n";
    
    material.CreateShader(VertexSource, FragmentSource);        // Create a material shader from the default source code

    unsigned int grid_size = 33;
    bool flag = false;
    tira::image<unsigned char> grid(grid_size, grid_size, 1);                   // Create an image to represent the default texture
    for(unsigned int y = 0; y < grid_size; y++){
        for(unsigned int x = 0; x < grid_size; x++){
            grid(x, y) = 255 * (unsigned char)flag;
            flag = !flag;
        }
    }
    grid.save("test.bmp");
    material.SetTexture("texmap", grid, GL_RED, GL_NEAREST);
}


int main(int argc, char** argv)
{
    camera.setPosition(1, 1, 1);
    camera.LookAt(0, 0, 0);
    camera.setFOV(0.50f * 3.14159);


    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    window = glfwCreateWindow(1600, 1200, "MateriaLab", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    //set input callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    InitUI(window, glsl_version);

    GLenum err = glewInit();
    if (GLEW_OK != err){
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    
    LoadMesh(mesh_type);
    CreateDefaultMaterial();

    RefreshLocalMaterialData();

    //OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderUI();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        //ImVec2 window_size = ImGui::GetWindowSize();
        int render_w = display_w - gui_window_size.x;
        //int render_w = display_w/2;
        glViewport(gui_window_size.x, 0, render_w, display_h);

        projection = glm::perspective(camera.getFOV(), (float)(render_w)/(float)display_h, 0.01f, 100.0f);
        view = glm::lookAt(camera.getPosition(), camera.getLookAt(), camera.getUp());
        mvp = projection * view;
        material.Begin();
        mesh.Draw();
        material.End();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    mesh.Destroy();
    DestroyUI();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}