# shader vertex
# version 140

layout(location = 0) in vec3 vertices;
layout(location = 2) in vec2 texcoords;

uniform mat4 MVP;

out vec4 vertex_color;
out vec2 vertex_texcoord;

void main(){		
	gl_Position = MVP * vec4(vertices.x, vertices.y, vertices.z, 1.0f);
	vertex_color = vec4(vertices.x + 0.5, vertices.y + 0.5, vertices.z + 0.5, 1.0);
	vertex_texcoord = texcoords;
};

# shader fragment
# version 140

layout(location = 0) out vec4 color;

in vec4 vertex_color;
in vec2 vertex_texcoord;
uniform sampler2D texmap;

void main(){
	color = vertex_color * texture(texmap, vertex_texcoord);
	//color = vec4(vertex_texcoord.x, vertex_texcoord.y, 0.0, 1.0);
};