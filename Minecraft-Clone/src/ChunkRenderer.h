#pragma once

#include <vector>
#include <utility>

#include <RealEngine.h>

static const uint32_t BUFFER_SIZE = (uint32_t)5e8; // 500 mb
static const uint32_t QUAD_SIZE = 8;
static const int MAX_DRAW_COMMANDS = 100000;

namespace RealEngine {
    struct BufferSlot {
        uint32_t startByte;
        uint32_t sizeBytes;
    };

    struct DrawElementsIndirectCommand {
        uint32_t indexCount;    // (count) Quad count * 6
        uint32_t instanceCount; // 1
        uint32_t firstIndex;    // 0
        uint32_t baseQuad;      // (baseVertex) Starting index in the SSBO
        uint32_t baseInstance;  // Chunk x, y z, face index
    };

    struct BufferFit {
        uint32_t pos;
        uint32_t space;
        std::vector<BufferSlot*>::iterator iter;
    };

    class ChunkRenderer {
    public:
        void init();

        DrawElementsIndirectCommand* getDrawCommand(int quadCount, uint32_t baseInstance);
        inline void addDrawCommand(const DrawElementsIndirectCommand& command) {
            drawCommands.insert(drawCommands.end(), command);
        }
        void removeDrawCommand(const DrawElementsIndirectCommand* command);

        void buffer(const DrawElementsIndirectCommand& command, void* vertices);
        void render(Ref<Shader> shader);

    private:
        DrawElementsIndirectCommand* createCommand(BufferSlot& slot, uint32_t baseInstance);

		Ref<VertexArray> m_VAO;
		Ref<IndexBuffer> m_IBO;
        Ref<ShaderStorageBuffer> m_SSBO;
		Ref<CommandBuffer> m_CommandBuffer;

        std::vector<BufferSlot*> usedSlots;
        std::vector<DrawElementsIndirectCommand> drawCommands;
        uint32_t allocationEnd = 0;
    };
}
