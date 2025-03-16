#type vertex
#version 420 core

// 7 bits for x
// 7 bits for y
// 7 bits for y
// 8 bits for id (unused)
// 2 bits for ambient occlusion (unused)
layout (location = 0) in uint aPackage;

#define Position_Bitmask uint(0x7F)

uniform Camera {
	mat4 u_ViewProjection;
};

vec4 unpackVertex() {
    float x = float(aPackage & Position_Bitmask);          // Extract 7 bits for x
    float y = float((aPackage >> 7) & Position_Bitmask);   // Extract 7 bits for y
    float z = float((aPackage >> 14) & Position_Bitmask);  // Extract 7 bits for z
    return vec4(x, y, z + 1, 1.0f);
}

void main() {
	gl_Position = u_ViewProjection * unpackVertex();
}

#type fragment
#version 420 core
out vec4 FragColor;

void main() {
   FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}