#include "Chunk.h"

namespace RealEngine {
    namespace Utils {
        enum FacingDirection : uint32_t {
            FRONT   = 0,
            BACK    = 1,
            LEFT    = 2,
            RIGHT   = 3,
            TOP     = 4,
            BOTTOM  = 5
        };

        PACKED_STRUCT(VertexData) {
            glm::vec3 Position;
			FacingDirection Direction;
        };
    }

	Chunk::Chunk(const glm::ivec3& chunkOffset) 
        : m_ChunkOffset(chunkOffset) {
        m_MeshData.opaqueMask = new uint64_t[CS_P2]{ 0 };
        m_MeshData.faceMasks = new uint64_t[CS_2 * 6]{ 0 };
        m_MeshData.forwardMerged = new uint8_t[CS_2]{ 0 };
        m_MeshData.rightMerged = new uint8_t[CS]{ 0 };
        m_MeshData.vertices = new std::vector<uint64_t>(10000);
        m_MeshData.maxVertices = 10000;
    }

    Chunk::~Chunk() {
		delete[] m_MeshData.opaqueMask;
		delete[] m_MeshData.faceMasks;
		delete[] m_MeshData.forwardMerged;
		delete[] m_MeshData.rightMerged;
		delete m_MeshData.vertices;
    }

    constexpr uint64_t P_MASK = ~(1ull << 63 | 1);

    static inline const int getAxisIndex(const int axis, const int a, const int b, const int c) {
        if (axis == 0) return b + (a * CS_P) + (c * CS_P2);
        else if (axis == 1) return b + (c * CS_P) + (a * CS_P2);
        else return c + (a * CS_P) + (b * CS_P2);
    }

    static inline const void insertQuad(std::vector<uint64_t>& vertices, uint64_t quad, int& vertexI, int& maxVertices) {
        if (vertexI >= maxVertices - 6) {
            vertices.resize(maxVertices * 2, 0);
            maxVertices *= 2;
        }

        vertices[vertexI] = quad;

        vertexI++;
    }

    static inline const uint64_t getQuad(uint64_t x, uint64_t y, uint64_t z, uint64_t w, uint64_t h, uint64_t type) {
        return (type << 32) | (h << 24) | (w << 18) | (z << 12) | (y << 6) | x;
    }
					
    MeshData& Chunk::Reuse() {
        RE_PROFILE_FUNCTION();

        //for (uint16_t y = 0; y < CS; y++) {
        //    for (uint8_t x = 0; x < CS; x++) {
        //        for (uint8_t z = 0; z < CS; z++) {
        //            if (y == 0) {
        //                m_Blocks[z + x * CS + y * CS * CS] = Air;
        //                meshData.opaqueMask[(y * CS) + x] |= 0ull << z;
        //            }
        //            else if (y % 2 == 0) {
        //                m_Blocks[z + x * CS + y * CS * CS] = Dirt;
        //                meshData.opaqueMask[(y * CS) + x] |= 1ull << z;
        //            }
        //            else {
        //                m_Blocks[z + x * CS + y * CS * CS] = Grass;
        //                meshData.opaqueMask[(y * CS) + x] |= 1ull << z;
        //            }
        //        }
        //    }
        //}

        for (int x = 1; x < CS_P; x++) {
            for (int y = 1; y < CS_P; y++) {
                for (int z = 1; z < CS_P; z++) {
                    if (x % 2 == 0 && y % 2 == 0 && z % 2 == 0) {
                        m_Blocks[z + (x * CS_P) + (y * CS_P2)] = 1;
                        m_MeshData.opaqueMask[(y * CS_P) + x] |= 1ull << z;

                        m_Blocks[z + (x * CS_P) + (y * CS_P2)] = 2;
                        m_MeshData.opaqueMask[((y - 1) * CS_P) + (x - 1)] |= 1ull << z;

                        m_Blocks[z + (x * CS_P) + (y * CS_P2)] = 3;
                        m_MeshData.opaqueMask[(y * CS_P) + (x - 1)] |= 1ull << (z - 1);

                        m_Blocks[z + (x * CS_P) + (y * CS_P2)] = 4;
                        m_MeshData.opaqueMask[((y - 1) * CS_P) + x] |= 1ull << (z - 1);
                    }
                }
            }
        }

        GenerateMesh();
		return m_MeshData;
    }

    void Chunk::GenerateMesh() {
        RE_PROFILE_FUNCTION();
        
        m_MeshData.vertexCount = 0;
        int vertexI = 0;

        uint64_t* opaqueMask = m_MeshData.opaqueMask;
        uint64_t* faceMasks = m_MeshData.faceMasks;
        uint8_t* forwardMerged = m_MeshData.forwardMerged;

        // Hidden face culling
        for (int a = 1; a < CS_P - 1; a++) {
            const int aCS_P = a * CS_P;

            for (int b = 1; b < CS_P - 1; b++) {
                const uint64_t columnBits = opaqueMask[(a * CS_P) + b] & P_MASK;
                const int baIndex = (b - 1) + (a - 1) * CS;
                const int abIndex = (a - 1) + (b - 1) * CS;

                faceMasks[baIndex + 0 * CS_2] = (columnBits & ~opaqueMask[aCS_P + CS_P + b]) >> 1;
                faceMasks[baIndex + 1 * CS_2] = (columnBits & ~opaqueMask[aCS_P - CS_P + b]) >> 1;

                faceMasks[abIndex + 2 * CS_2] = (columnBits & ~opaqueMask[aCS_P + (b + 1)]) >> 1;
                faceMasks[abIndex + 3 * CS_2] = (columnBits & ~opaqueMask[aCS_P + (b - 1)]) >> 1;

                faceMasks[baIndex + 4 * CS_2] = columnBits & ~(opaqueMask[aCS_P + b] >> 1);
                faceMasks[baIndex + 5 * CS_2] = columnBits & ~(opaqueMask[aCS_P + b] << 1);
            }
        }

        // Greedy meshing faces 0-3
        for (uint8_t face = 0; face < 4; face++) {
            const int axis = face / 2;

            const int faceVertexBegin = vertexI;

            for (uint8_t layer = 0; layer < CS; layer++) {
                const int bitsLocation = layer * CS + face * CS_2;

                for (uint8_t forward = 0; forward < CS; forward++) {
                    uint64_t bitsHere = faceMasks[forward + bitsLocation];
                    if (bitsHere == 0) continue;

                    const uint64_t bitsNext = forward + 1 < CS ? faceMasks[(forward + 1) + bitsLocation] : 0;

                    uint8_t rightMerged = 1;
                    while (bitsHere) {
                        unsigned long bitPos;
#ifdef _MSC_VER
                        _BitScanForward64(&bitPos, bitsHere);
#else
                        bitPos = __builtin_ctzll(bitsHere);
#endif

                        const uint8_t type = m_Blocks[getAxisIndex(axis, forward + 1, bitPos + 1, layer + 1)];
                        uint8_t& forwardMergedRef = forwardMerged[bitPos];

                        if ((bitsNext >> bitPos & 1) && type == m_Blocks[getAxisIndex(axis, forward + 2, bitPos + 1, layer + 1)]) {
                            forwardMergedRef++;
                            bitsHere &= ~(1ull << bitPos);
                            continue;
                        }

                        for (int right = bitPos + 1; right < CS; right++) {
                            if (!(bitsHere >> right & 1) || forwardMergedRef != forwardMerged[right] || type != m_Blocks[getAxisIndex(axis, forward + 1, right + 1, layer + 1)]) break;
                            forwardMerged[right] = 0;
                            rightMerged++;
                        }
                        bitsHere &= ~((1ull << (bitPos + rightMerged)) - 1);

                        const uint8_t meshFront = forward - forwardMergedRef;
                        const uint8_t meshLeft = (uint8_t)bitPos;
                        const uint8_t meshUp = layer + (~face & 1);

                        const uint8_t meshWidth = rightMerged;
                        const uint8_t meshLength = forwardMergedRef + 1;

                        forwardMergedRef = 0;
                        rightMerged = 1;

                        uint64_t quad = 0;
                        switch (face) {
                            case 0:
                            case 1:
                                quad = getQuad(meshFront + (face == 1 ? meshLength : 0), meshUp, meshLeft, meshLength, meshWidth, type);
                                break;
                            case 2:
                            case 3:
                                quad = getQuad(meshUp, meshFront + (face == 2 ? meshLength : 0), meshLeft, meshLength, meshWidth, type);
                                break;
						    default:
							    RE_ASSERT(false, "Invalid face index");
							    break;
                        }

                        insertQuad(*m_MeshData.vertices, quad, vertexI, m_MeshData.maxVertices);
                    }
                }
            }

            const int faceVertexLength = vertexI - faceVertexBegin;
            m_MeshData.faceVertexBegin[face] = faceVertexBegin;
            m_MeshData.faceVertexLength[face] = faceVertexLength;
        }

        uint8_t* rightMerged = m_MeshData.rightMerged;

        // Greedy meshing faces 4-5
        for (uint8_t face = 4; face < 6; face++) {
            const int axis = face / 2;

            const int faceVertexBegin = vertexI;

            for (uint8_t forward = 0; forward < CS; forward++) {
                const int bitsLocation = forward * CS + face * CS_2;
                const int bitsForwardLocation = (forward + 1) * CS + face * CS_2;

                for (uint8_t right = 0; right < CS; right++) {
                    uint64_t bitsHere = faceMasks[right + bitsLocation];
                    if (bitsHere == 0) continue;

                    const uint64_t bitsForward = forward < CS - 1 ? faceMasks[right + bitsForwardLocation] : 0;
                    const uint64_t bitsRight = right < CS - 1 ? faceMasks[right + 1 + bitsLocation] : 0;
                    const int rightCS = right * CS;

                    while (bitsHere) {
                        unsigned long bitPos;
#ifdef _MSC_VER
                        _BitScanForward64(&bitPos, bitsHere);
#else
                        bitPos = __builtin_ctzll(bitsHere);
#endif

                        bitsHere &= ~(1ull << bitPos);

                        const uint8_t type = m_Blocks[getAxisIndex(axis, right + 1, forward + 1, bitPos)];
                        uint8_t& forwardMergedRef = forwardMerged[rightCS + (bitPos - 1)];
                        uint8_t& rightMergedRef = rightMerged[bitPos - 1];

                        if (rightMergedRef == 0 && (bitsForward >> bitPos & 1) && type == m_Blocks[getAxisIndex(axis, right + 1, forward + 2, bitPos)]) {
                            forwardMergedRef++;
                            continue;
                        }

                        if ((bitsRight >> bitPos & 1) && forwardMergedRef == forwardMerged[(rightCS + CS) + (bitPos - 1)] && type == m_Blocks[getAxisIndex(axis, right + 2, forward + 1, bitPos)]) {
                            forwardMergedRef = 0;
                            rightMergedRef++;
                            continue;
                        }

                        const uint8_t meshLeft = right - rightMergedRef;
                        const uint8_t meshFront = forward - forwardMergedRef;
                        const uint8_t meshUp = (uint8_t)(bitPos - 1 + (~face & 1));

                        const uint8_t meshWidth = 1 + rightMergedRef;
                        const uint8_t meshLength = 1 + forwardMergedRef;

                        forwardMergedRef = 0;
                        rightMergedRef = 0;

                        const uint64_t quad = getQuad(meshLeft + (face == 4 ? meshWidth : 0), meshFront, meshUp, meshWidth, meshLength, type);

                        insertQuad(*m_MeshData.vertices, quad, vertexI, m_MeshData.maxVertices);
                    }
                }
            }

            const int faceVertexLength = vertexI - faceVertexBegin;
            m_MeshData.faceVertexBegin[face] = faceVertexBegin;
            m_MeshData.faceVertexLength[face] = faceVertexLength;
        }

        m_MeshData.vertexCount = vertexI + 1;
    }
}
