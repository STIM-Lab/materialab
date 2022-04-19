#include "tira/graphics/glShader.h"
#include "imgui_impl_glfw.h"

enum class MatrixMap { Manual, View, Projection, ViewProjection };
static std::vector<std::string> matrix_map_names = { "Manual", "View", "Projection", "P*V" };

struct ShaderData : public tira::glShaderUniform {
	void* data = NULL;
	int mapto = (int)MatrixMap::Manual;
};

void RefreshLocalMaterialData();
void RefreshDeviceMaterialData();

enum class Mesh { Cube, Sphere, Icosahedron, Icosphere };
void LoadMesh(Mesh mesh_id);


void RenderUI();

void InitUI(GLFWwindow* window, const char* glsl_version);

void DestroyUI();