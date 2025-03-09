#type vertex
#version 420 core

layout (location = 0) in vec3 aPos;

void main() {
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#type fragment
#version 420 core
out vec4 FragColor;

layout(std140, binding = 0) uniform Color {
	vec3 color;
};

void main() {
   FragColor = vec4(color, 1.0);
}