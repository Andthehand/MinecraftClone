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
        void init() {
			RE_PROFILE_FUNCTION();

			m_VAO = VertexArray::Create();
            m_CommandBuffer = CommandBuffer::Create(MAX_DRAW_COMMANDS * sizeof(DrawElementsIndirectCommand));
			m_SSBO = ShaderStorageBuffer::Create(BUFFER_SIZE, 1);

            uint32_t maxQuads = CS * CS * CS * 6;
            std::vector<uint32_t> indices;
            for (uint32_t i = 0; i < maxQuads; i++) {
                indices.push_back((i << 2) | 2u);
                indices.push_back((i << 2) | 0u);
                indices.push_back((i << 2) | 1u);
                indices.push_back((i << 2) | 1u);
                indices.push_back((i << 2) | 3u);
                indices.push_back((i << 2) | 2u);
            }
			m_IBO = IndexBuffer::Create(indices.data(), maxQuads * 6);
			m_VAO->SetIndexBuffer(m_IBO);
        };

        DrawElementsIndirectCommand* getDrawCommand(int quadCount, uint32_t baseInstance) {
            RE_PROFILE_FUNCTION();
            unsigned int requestedSize = quadCount * QUAD_SIZE;

            // Allocate at the end if possible
            if ((BUFFER_SIZE - allocationEnd) > requestedSize) {
                auto slot = new BufferSlot({ allocationEnd, requestedSize });
                usedSlots.push_back(slot);
                allocationEnd += requestedSize;
                return createCommand(*slot, baseInstance);
            }

            // Otherwise, iterate through and find gaps to allocate in
            // Slow!

            auto prev = usedSlots.begin();
            BufferFit bestFit;
            bool foundFit = false;

            unsigned int spaceInFront = usedSlots.front()->startByte;
            if (spaceInFront >= requestedSize) {
                foundFit = true;
                bestFit = BufferFit({
                  0,
                  spaceInFront,
                  usedSlots.begin()
                });
            }

            if (usedSlots.size() == 1) {
                auto slot = new BufferSlot({ usedSlots.back()->startByte + usedSlots.back()->sizeBytes, requestedSize });
                usedSlots.insert(usedSlots.end(), slot);
                return createCommand(*slot, baseInstance);
            }

            for (auto it = usedSlots.begin() + 1; it != usedSlots.end(); ++it) {
                unsigned int pos = (*prev)->startByte + (*prev)->sizeBytes;
                unsigned int spaceBetween = (*it)->startByte - pos;
                if (spaceBetween >= requestedSize && (!foundFit || spaceBetween < bestFit.space)) {
                    foundFit = true;
                    bestFit = BufferFit({
                      pos,
                      spaceBetween,
                      it
                    });
                }
                prev = it;
            }

            if (foundFit) {
                auto slot = new BufferSlot({ bestFit.pos, requestedSize });
                usedSlots.insert(bestFit.iter, slot);
                return createCommand(*slot, baseInstance);
            }

            auto slot = new BufferSlot({ usedSlots.back()->startByte + usedSlots.back()->sizeBytes, requestedSize });
            usedSlots.insert(usedSlots.end(), slot);
            return createCommand(*slot, baseInstance);
        };

        void removeDrawCommand(const DrawElementsIndirectCommand* command) {
            RE_PROFILE_FUNCTION();

            for (auto it = begin(usedSlots); it != end(usedSlots); ++it) {
                if ((*it)->startByte == (command->baseQuad >> 2) * QUAD_SIZE) {
                    usedSlots.erase(it);
                    return;
                }
            }
        }

        void buffer(const DrawElementsIndirectCommand& command, void* vertices) {
            RE_PROFILE_FUNCTION();
			m_SSBO->SetData(vertices, (command.indexCount / 6) * QUAD_SIZE, (command.baseQuad >> 2) * QUAD_SIZE);
        }

        inline void addDrawCommand(const DrawElementsIndirectCommand& command) {
            RE_PROFILE_FUNCTION();
            drawCommands.insert(drawCommands.end(), command);
        };

        void render(Ref<Shader> shader) {
            RE_PROFILE_FUNCTION();
            uint32_t numCommands = (uint32_t)drawCommands.size();

            if (numCommands == 0) {
                return;
            }

			// Render
            shader->Bind();
			m_CommandBuffer->SetData(drawCommands.data(), numCommands * sizeof(DrawElementsIndirectCommand), 0);
			m_CommandBuffer->Bind();
            RenderCommands::DrawMutliIndexedIndirect(m_VAO, numCommands);

            // clear but don't deallocate
            drawCommands.clear();
        };

    private:
        DrawElementsIndirectCommand* createCommand(BufferSlot& slot, uint32_t baseInstance) {
            return new DrawElementsIndirectCommand({
              (slot.sizeBytes / QUAD_SIZE) * 6,
              1,
              0,
              (slot.startByte / QUAD_SIZE) << 2,
              baseInstance
            });
        }

		Ref<VertexArray> m_VAO;
		Ref<IndexBuffer> m_IBO;
        Ref<ShaderStorageBuffer> m_SSBO;
		Ref<CommandBuffer> m_CommandBuffer;

        std::vector<BufferSlot*> usedSlots;
        std::vector<DrawElementsIndirectCommand> drawCommands;
        uint32_t allocationEnd = 0;
    };
}
