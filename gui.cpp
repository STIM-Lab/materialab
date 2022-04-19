
#include "materialab.h"
#include "imgui_impl_opengl3.h"
//#include "imgui/imgui.h"
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "tira/graphics_gl.h"


extern tira::glMaterial material;
extern std::unordered_map<std::string, ShaderData> MaterialMap;

extern glm::mat4 projection;
extern glm::mat4 view;
extern glm::mat4 mvp;
extern tira::camera camera;

// stage control parameters
float x_dest, y_dest, z_dest;
void HomeX() {};
void HomeY() {};
void HomeZ() {};

float ui_scale = 1.5f;
ImVec2 gui_window_size;

std::string ShaderFileName;

// mesh variables
std::vector<std::string> mesh_names = { "cube", "sphere", "icosahedron", "icosphere"};
Mesh mesh_type = Mesh::Cube; // Here we store our selection data as an index.

float float_step = 0.01;

void UpdateMappedUniform(std::string name, GLenum type, void* uniform_data) {
	
	switch (type) {
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_DOUBLE:
	case GL_DOUBLE_VEC2:
	case GL_DOUBLE_VEC3:
	case GL_DOUBLE_VEC4:
	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
		// update the matrix if it is mapped to an interface matrix
		if (MaterialMap[name].mapto == (int)MatrixMap::Projection)
			(*(glm::mat4*)uniform_data) = projection;
		if (MaterialMap[name].mapto == (int)MatrixMap::View)
			(*(glm::mat4*)uniform_data) = view;
		if (MaterialMap[name].mapto == (int)MatrixMap::ViewProjection)
			(*(glm::mat4*)uniform_data) = projection * view;
		material.Bind();
		material.SetUniformMat4f(name, *(glm::mat4*)(uniform_data));
		break;
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
	case GL_DOUBLE_MAT2:
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4:
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
	case GL_IMAGE_1D:
	case GL_IMAGE_2D:
	case GL_IMAGE_3D:
	case GL_IMAGE_2D_RECT:
	case GL_IMAGE_CUBE:
	case GL_IMAGE_BUFFER:
	case GL_IMAGE_1D_ARRAY:
	case GL_IMAGE_2D_ARRAY:
	case GL_IMAGE_2D_MULTISAMPLE:
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_INT_IMAGE_1D:
	case GL_INT_IMAGE_2D:
	case GL_INT_IMAGE_3D:
	case GL_INT_IMAGE_2D_RECT:
	case GL_INT_IMAGE_CUBE:
	case GL_INT_IMAGE_BUFFER:
	case GL_INT_IMAGE_1D_ARRAY:
	case GL_INT_IMAGE_2D_ARRAY:
	case GL_INT_IMAGE_2D_MULTISAMPLE:
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_1D:
	case GL_UNSIGNED_INT_IMAGE_2D:
	case GL_UNSIGNED_INT_IMAGE_3D:
	case GL_UNSIGNED_INT_IMAGE_2D_RECT:
	case GL_UNSIGNED_INT_IMAGE_CUBE:
	case GL_UNSIGNED_INT_IMAGE_BUFFER:
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_ATOMIC_COUNTER:
	default:
		break;
	}
}


void ShaderUniformUI(std::string name, GLenum type, void* uniform_data){
	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
	size_t texture_id;
	switch (type) {
	case GL_FLOAT:
		ImGui::Text("%s", ("(float) " + name).c_str());
		if(ImGui::DragScalar(("##0"+name).c_str(), ImGuiDataType_Float, (float*)uniform_data, 0.005f)) {
			material.Bind();
			material.SetUniform1f(name, *(float*)(uniform_data));
		}
		break;
	case GL_FLOAT_VEC2:
		ImGui::DragFloat2(name.c_str(), (float*)uniform_data);
		break;
	case GL_FLOAT_VEC3:
		ImGui::DragFloat3(name.c_str(), (float*)uniform_data);
		break;
	case GL_FLOAT_VEC4:
		ImGui::DragFloat4(name.c_str(), (float*)uniform_data);
		break;
	case GL_DOUBLE:
		ImGui::Text("%s", (name + " (double)").c_str());
		if (ImGui::DragScalar(("##0" + name).c_str(), ImGuiDataType_Double, (double*)uniform_data, 0.00005f)) {
			material.Bind();
			material.SetUniform1f(name, *(double*)(uniform_data));
		}
		break;
	case GL_DOUBLE_VEC2:
	case GL_DOUBLE_VEC3:
	case GL_DOUBLE_VEC4:
	case GL_INT:
		ImGui::DragInt(name.c_str(), (int*)uniform_data);
		break;
	case GL_INT_VEC2:
		ImGui::DragInt2(name.c_str(), (int*)uniform_data);
		break;
	case GL_INT_VEC3:
		ImGui::DragInt3(name.c_str(), (int*)uniform_data);
		break;
	case GL_INT_VEC4:
		ImGui::DragInt4(name.c_str(), (int*)uniform_data);
		break;
	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
		ImGui::Text("%s", (name + " (mat4)").c_str());
		ImGui::SameLine();

		if (ImGui::BeginCombo(("##0" + name).c_str(), matrix_map_names[(int)MaterialMap[name].mapto].c_str())) {
			for (int n = 0; n < matrix_map_names.size(); n++) {
				const bool is_selected = (MaterialMap[name].mapto == n);
				if (ImGui::Selectable(matrix_map_names[n].c_str(), is_selected)) {
					MaterialMap[name].mapto = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::DragFloat4(("##0" + name).c_str(), (float*)uniform_data);
		ImGui::DragFloat4(("##1" + name).c_str(), ((float*)uniform_data) + 4);
		ImGui::DragFloat4(("##2" + name).c_str(), ((float*)uniform_data) + 8);
		ImGui::DragFloat4(("##3" + name).c_str(), ((float*)uniform_data) + 12);

		break;
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
	case GL_DOUBLE_MAT2:
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4:
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
		ImGui::Text("%s", (name + " (sampler2d)").c_str());
		if (ImGui::Button("Load Image")) {
			ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bmp,.png,.jpg", ".");
		}
		ImGui::SameLine();
		texture_id = material.GetTexture(name).ID();
		ImGui::Image((ImTextureID)texture_id, ImVec2(100, 100), uv_min, uv_max, tint_col, border_col);

		// display
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk()) {
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				// action
				tira::image<unsigned char> teximage(filePathName);
				material.SetTexture(name, teximage);
			}
			// close
			ImGuiFileDialog::Instance()->Close();
		}
		break;
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
	case GL_IMAGE_1D:
	case GL_IMAGE_2D:
	case GL_IMAGE_3D:
	case GL_IMAGE_2D_RECT:
	case GL_IMAGE_CUBE:
	case GL_IMAGE_BUFFER:
	case GL_IMAGE_1D_ARRAY:
	case GL_IMAGE_2D_ARRAY:
	case GL_IMAGE_2D_MULTISAMPLE:
	case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_INT_IMAGE_1D:
	case GL_INT_IMAGE_2D:
	case GL_INT_IMAGE_3D:
	case GL_INT_IMAGE_2D_RECT:
	case GL_INT_IMAGE_CUBE:
	case GL_INT_IMAGE_BUFFER:
	case GL_INT_IMAGE_1D_ARRAY:
	case GL_INT_IMAGE_2D_ARRAY:
	case GL_INT_IMAGE_2D_MULTISAMPLE:
	case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_1D:
	case GL_UNSIGNED_INT_IMAGE_2D:
	case GL_UNSIGNED_INT_IMAGE_3D:
	case GL_UNSIGNED_INT_IMAGE_2D_RECT:
	case GL_UNSIGNED_INT_IMAGE_CUBE:
	case GL_UNSIGNED_INT_IMAGE_BUFFER:
	case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_ATOMIC_COUNTER:
	default:
		break;
	}
}

void RenderInterfaceVariables() {
	ImGui::BeginDisabled();

	glm::vec3 position = camera.getPosition();
	ImGui::Text("Camera Position");
	ImGui::DragFloat3("##0 Camera Position", (float*)&position);

	ImGui::Text("View Matrix");
	ImGui::DragFloat4("##0 View Matrix", (float*)&view);
	ImGui::DragFloat4("##1 View Matrix", ((float*)&view) + 4);
	ImGui::DragFloat4("##2 View Matrix", ((float*)&view) + 8);
	ImGui::DragFloat4("##3 View Matrix", ((float*)&view) + 12);

	ImGui::Text("Projection Matrix");
	ImGui::DragFloat4("##0 Projection Matrix", (float*)&projection);
	ImGui::DragFloat4("##1 Projection Matrix", ((float*)&projection) + 4);
	ImGui::DragFloat4("##2 Projection Matrix", ((float*)&projection) + 8);
	ImGui::DragFloat4("##3 Projection Matrix", ((float*)&projection) + 12);

	glm::mat4 PV = projection * view;
	ImGui::Text("P * V");
	ImGui::DragFloat4("##0 P * V", (float*)&PV);
	ImGui::DragFloat4("##1 P * V", ((float*)&PV) + 4);
	ImGui::DragFloat4("##2 P * V", ((float*)&PV) + 8);
	ImGui::DragFloat4("##3 P * V", ((float*)&PV) + 12);

	ImGui::EndDisabled();
}

void RenderUI() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	
    static float f = 0.0f;
    static int counter = 0;


    ImGui::Begin("Active Shader");                          // Create a window called "Hello, world!" and append into it.
	{
		/// Handle the button and dialog box for loading and refreshing shaders //
		if (ImGui::Button("Load Shader"))					// create a button for loading the shader
			ImGuiFileDialog::Instance()->OpenDialog("ChooseShaderFile", "Choose Shader File", ".shader,.*", ".");
		ImGui::SameLine();
		if (ImGuiFileDialog::Instance()->Display("ChooseShaderFile")){				// if the user opened a file dialog
			if (ImGuiFileDialog::Instance()->IsOk()){								// and clicks okay, they've probably selected a file
				ShaderFileName = ImGuiFileDialog::Instance()->GetFilePathName();	// get the name of the file
				material.LoadShader(ShaderFileName);								// load a shader with the corresponding pathname
				RefreshLocalMaterialData();											// refresh the local shader properties (uniform variables)
			}
			ImGuiFileDialog::Instance()->Close();									// close the file dialog box		
		}

		if (ShaderFileName == "") 													// If the user hasn't opened a shader (the file name is empty)
			ImGui::BeginDisabled();													// then the "Refresh" button will be disabled.
		{
			if (ImGui::Button("Refresh")) {											// If the user clicks the "Refresh" button,
				material.LoadShader(ShaderFileName);								// the current shader file will be loaded
				RefreshLocalMaterialData();											// and the material data in memory and on the device
				RefreshDeviceMaterialData();										// will be refreshed.
			}
			if (ShaderFileName == "") 
				ImGui::EndDisabled();
		}

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Uniforms"))
			{
				for (std::unordered_map<std::string, ShaderData>::iterator i = MaterialMap.begin();
					i != MaterialMap.end(); i++) {
					ShaderUniformUI(i->second.name, i->second.type, i->second.data);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Interface"))
			{
				ImGui::Text("%s", "Mesh");
				if (ImGui::BeginCombo("##0 Meshes", mesh_names[(int)mesh_type].c_str())) {
					for (int n = 0; n < mesh_names.size(); n++) {
						const bool is_selected = (mesh_type == (Mesh)n);
						if (ImGui::Selectable(mesh_names[n].c_str(), is_selected)) {		// if a new mesh is selected
							mesh_type = (Mesh)n;											// change the mesh type based on user selection
							LoadMesh(mesh_type);											// load the user-selected mesh
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				RenderInterfaceVariables();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::Separator();

		gui_window_size = ImGui::GetWindowSize();
		ImGui::End();
	}
        

    // Update mapped variables
	for (std::unordered_map<std::string, ShaderData>::iterator i = MaterialMap.begin(); i != MaterialMap.end(); i++) {
		UpdateMappedUniform(i->second.name, i->second.type, i->second.data);				// update any uniforms mapped to interface data
	}

    // Rendering
    ImGui::Render();
}

void InitUI(GLFWwindow* window, const char* glsl_version) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(ui_scale);
    io.FontGlobalScale = ui_scale;
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", ui_scale * 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
}

void DestroyUI() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
