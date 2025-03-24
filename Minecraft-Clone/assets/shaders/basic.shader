#type vertex
#version 450

// 7 bits for x						(0)
// 7 bits for y						(7)
// 7 bits for y						(14)
// 2 bits for UV					(21)
// 2 bits for side					(23)
// 2 bits for ambient occlusion (unused)(25)
layout (location = 0) in uint aPackage;
layout (location = 1) in uint aID;

uniform Camera {
	mat4 u_ViewProjection;
};

out vec3 v_TexCoord;

#define Position_Bitmask uint(0x7F)

const vec2 coords[4] = vec2[](
	vec2(1.0f, 1.0f), vec2(0.0f, 1.0f), vec2(0.0f, 0.0f), vec2(1.0f, 0.0f)
);

vec4 unpackVertex() {
    float x = float(aPackage & Position_Bitmask);          // Extract 7 bits for x
    float y = float((aPackage >> 7) & Position_Bitmask);   // Extract 7 bits for y
    float z = float((aPackage >> 14) & Position_Bitmask);  // Extract 7 bits for z
    return vec4(x, y, z + 1, 1.0f);
}

vec3 unpackTextureCoords() {
	uint UV = (aPackage >> 21) & 0x03;

	return vec3(coords[UV], aID);
}

void main() {
	v_TexCoord = unpackTextureCoords();

	gl_Position = u_ViewProjection * unpackVertex();
}

#type fragment
#version 450
uniform sampler2DArray ourTexture;

in vec3 v_TexCoord;

out vec4 FragColor;

void main() {
   FragColor = texture(ourTexture, v_TexCoord);
   // FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}