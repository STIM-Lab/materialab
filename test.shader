# shader vertex
# version 130

in vec3 vertices;
in vec3 normals;
in vec2 texcoords;

uniform mat4 MVP;

out vec4 vertex_color;
out vec3 vertex_normals;
out vec2 vertex_texcoord;

void main(){		
	gl_Position = MVP * vec4(vertices.x, vertices.y, vertices.z, 1.0f);
	vertex_color = vec4(vertices.x + 0.5, vertices.y + 0.5, vertices.z + 0.5, 1.0);
	vertex_texcoord = texcoords;
	vertex_normals = normals;
};

# shader fragment
# version 130

out vec4 color;

in vec4 vertex_color;
in vec3 vertex_normals;
in vec2 vertex_texcoord;
uniform sampler2D texmap;

void main(){
	color = vertex_color * texture(texmap, vertex_texcoord);
	//color = vec4(vertex_texcoord.x, vertex_texcoord.y, 0.0, 1.0);
};