#type vertex
#version 420 core

layout (location = 0) in vec3 aPos;

uniform Camera {
	mat4 u_ViewProjection;
};

void main() {
   gl_Position = u_ViewProjection * vec4(aPos, 1.0);
}

#type fragment
#version 420 core
out vec4 FragColor;

void main() {
   FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}