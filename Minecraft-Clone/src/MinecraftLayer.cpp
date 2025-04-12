#include "MinecraftLayer.h"

#include "Block.h"

#include <imgui.h>
#include <ryml.hpp>

namespace RealEngine {
	MinecraftLayer::MinecraftLayer()
		: Layer("MinecraftLayer") {
	}

	void MinecraftLayer::OnAttach() {
		RE_PROFILE_FUNCTION();
		m_Shader = Shader::Create("assets/shaders/basic.shader");

		glm::vec3 color = { 0.0f, 0.0f, 1.0f };
		m_CameraUniformBuffer = UniformBuffer::Create(&color, sizeof(CameraData), 0);

		m_Camera = EditorCamera(90.0f, 1.778f, 0.1f, 1000.0f);

		m_ChunkRenderer.init();
		m_Chunk = CreateScope<Chunk>(glm::vec3(0.0f, 0.0f, 0.0f));
		MeshData meshData = m_Chunk->Generate();

		glm::ivec3 chunkPos = m_Chunk->GetChunkOffset();
		std::array<DrawElementsIndirectCommand*, 6> commands = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		for (uint32_t i = 0; i < commands.size(); i++) {
			if (meshData.faceVertexLength[i]) {
				uint32_t baseInstance = (i << 24) | (chunkPos.z << 16) | (chunkPos.y << 8) | chunkPos.x;

				auto drawCommand = m_ChunkRenderer.getDrawCommand(meshData.faceVertexLength[i], baseInstance);

				commands[i] = drawCommand;

				m_ChunkRenderer.buffer(*drawCommand, meshData.vertices->data() + meshData.faceVertexBegin[i]);
			}
		}
		m_ChunkRenderData = ChunkRenderData({ commands });

		RenderCommands::SetFaceCulling(true);
	}

	void MinecraftLayer::OnUpdate(float deltaTime) {
		RE_PROFILE_FUNCTION();
		RenderCommands::Clear();

		// Update Camera and UBO
		m_Camera.OnUpdate(deltaTime);
		glm::mat4 viewProjection = m_Camera.GetViewProjection();
		glm::vec3 cameraPosition = m_Camera.GetPosition();
		CameraData cameraData = { viewProjection, cameraPosition };
		m_CameraUniformBuffer->SetData(&cameraData, sizeof(CameraData));

		// Render Chunks
		for (int i = 0; i < m_ChunkRenderData.faceDrawCommands.size(); i++) {
			if (m_ChunkRenderData.faceDrawCommands[i]) {
				m_ChunkRenderer.addDrawCommand(*m_ChunkRenderData.faceDrawCommands[i]);
			}
		}
		m_ChunkRenderer.render(m_Shader);
	}

	void MinecraftLayer::OnImGui() {
		RE_PROFILE_FUNCTION();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("OpenGL")) {
				static bool wireframe = false;
				if (ImGui::MenuItem("Wireframe", nullptr, &wireframe)) {
					RenderCommands::SetWireframe(wireframe);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void MinecraftLayer::OnEvent(Event& e) {
		m_Camera.OnEvent(e);
	}
}
