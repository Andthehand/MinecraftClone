#type vertex
#version 460 core

struct QuadData {
  uint quadData1;
  uint quadData2;
};

layout(binding = 1, std430) readonly buffer ssbo1 {
  QuadData data[];  
};

layout(binding = 0) uniform Camera {
	mat4 u_ViewProjection;
    vec3 eye_position;
};

out VS_OUT {
  out vec3 pos;
  flat vec3 normal;
  flat vec3 color;
} vs_out;

const vec3 normalLookup[6] = {
  vec3( 0, 1, 0 ),
  vec3(0, -1, 0 ),
  vec3( 1, 0, 0 ),
  vec3( -1, 0, 0 ),
  vec3( 0, 0, 1 ),
  vec3( 0, 0, -1 )
};

const vec3 colorLookup[8] = {
  vec3(0.2, 0.659, 0.839),
  vec3(0.302, 0.302, 0.302),
  vec3(0.278, 0.600, 0.141),
  vec3(0.1, 0.1, 0.6),
  vec3(0.1, 0.6, 0.6),
  vec3(0.6, 0.1, 0.6),
  vec3(0.6, 0.6, 0.1),
  vec3(0.6, 0.1, 0.1)
};

const int flipLookup[6] = int[6](1, -1, -1, 1, -1, 1);

void main() {
  ivec3 chunkOffsetPos = ivec3(gl_BaseInstance&511u, gl_BaseInstance>>9&511u, gl_BaseInstance>>18&511u) * 62;
  uint face = gl_BaseInstance>>28;

  int vertexID = int(gl_VertexID&3u);
  uint ssboIndex = gl_VertexID >> 2u;

  uint quadData1 = data[ssboIndex].quadData1;
  uint quadData2 = data[ssboIndex].quadData2;

  ivec3 iVertexPos = ivec3(quadData1, quadData1 >> 6u, quadData1 >> 12u) & 63;
  iVertexPos += chunkOffsetPos;

  int w = int((quadData1 >> 18u)&63u), h = int((quadData1 >> 24u)&63u);
  uint wDir = (face & 2) >> 1, hDir = 2 - (face >> 2);
  int wMod = vertexID >> 1, hMod = vertexID & 1;

  iVertexPos[wDir] += (w * wMod * flipLookup[face]);
  iVertexPos[hDir] += (h * hMod);

  vs_out.pos = iVertexPos;
  vs_out.normal = normalLookup[face];
  vs_out.color = colorLookup[(quadData2&255u) - 1];

  vec3 vertexPos = iVertexPos;
  vertexPos[wDir] += 0.0007 * flipLookup[face] * (wMod * 2 - 1);
  vertexPos[hDir] += 0.0007 * (hMod * 2 - 1);

  gl_Position = u_ViewProjection * vec4(vertexPos, 1);
}

#type fragment
#version 460 core

layout(location=0) out vec3 out_color;

in VS_OUT {
  vec3 pos;
  flat vec3 normal;
  flat vec3 color;
} fs_in;

layout(binding = 0) uniform Camera {
	mat4 u_ViewProjection;
    vec3 eye_position;
};

const vec3 diffuse_color = vec3(0.15, 0.15, 0.15);
const vec3 rim_color = vec3(0.04, 0.04, 0.04);
const vec3 sun_position = vec3(250.0, 1000.0, 750.0) * 10000;

void main() {
  vec3 L = normalize(sun_position - fs_in.pos);
  vec3 V = normalize(eye_position - fs_in.pos);

  float rim = 1 - max(dot(V, fs_in.normal), 0.0);
  rim = smoothstep(0.6, 1.0, rim);

  out_color = 
    fs_in.color +
    (diffuse_color * max(0, dot(L, fs_in.normal))) +
    (rim_color * vec3(rim, rim, rim));
}