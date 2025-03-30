#type vertex
#version 450

struct FaceData {
	vec3 Position;
    uint Facing;
};

layout(std430, binding = 2) buffer uFaces {
	FaceData Faces[];
};

struct TextureIds {
	uint Bottom;
    uint Side;
    uint Top;
};

layout(std430, binding = 3) buffer uSideIDs {
	TextureIds SideIds[];
};

uniform Camera {
	mat4 u_ViewProjection;
};

layout(binding = 1) uniform usampler2DArray uBlockIDs;

// Offsets for our vertices drawing this face
const vec3 facePositions[6][4] = vec3[6][4] (
	vec3[4](vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0)),	// FRONT
	vec3[4](vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0)),	// BACK
	vec3[4](vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0)),	// LEFT
	vec3[4](vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)),	// RIGHT
	vec3[4](vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)),	// TOP
	vec3[4](vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0))		// BOTTOM
);

// Winding order to access the face positions
const int indices[6] = {0, 1, 2, 2, 3, 0};

const vec2 coords[4] = vec2[](
	vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f)
);

out vec3 TexCoord;

void main() {
	const int index = gl_VertexID / 6;
	const int currVertexID = gl_VertexID % 6;
	FaceData currentFace = Faces[index];

	TextureIds currentTexture = SideIds[texture(uBlockIDs, currentFace.Position.xzy).x];

	float textureID;
	switch(currentFace.Facing) {
		case 0:
		case 1:
		case 2:
		case 3:
			textureID = round(currentTexture.Side);
			break;
		case 4:
			textureID = round(currentTexture.Top);
			break;
		case 5:
			textureID = round(currentTexture.Bottom);
			break;
	}

	// Offset the face off of origin based on direction of the face
	vec3 vertexPosition = currentFace.Position + facePositions[currentFace.Facing][indices[currVertexID]];

	TexCoord = vec3(coords[indices[currVertexID]], textureID);
	gl_Position = u_ViewProjection * vec4(vertexPosition, 1.0f);
}

#type fragment
#version 450
layout(binding = 0) uniform sampler2DArray ourTexture;

in vec3 TexCoord;

out vec4 FragColor;

void main() {
   FragColor = texture(ourTexture, TexCoord);
   // FragColor = vec4(test, 0.0f, 0.1f, 1.0f);
}