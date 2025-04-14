#pragma once

#include <RealEngine.h>

const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SENSITIVITY = 0.075f;
const float FOV = 80.0f;

namespace RealEngine {
    class GameCamera {
    public:
		GameCamera() = default;

        GameCamera(glm::vec3 position) : position(position) {
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            HandleResolution(mode->width, mode->height);
            UpdateCameraVectors();
        }

		void OnUpdate(float deltaTime) {
			RE_PROFILE_FUNCTION();
			// Update camera position based on input (WASD keys)
			if (Input::IsKeyPressed(Key::W)) {
				position += front * deltaTime * Speed;
			}
			if (Input::IsKeyPressed(Key::S)) {
				position -= front * deltaTime * Speed;
			}
			if (Input::IsKeyPressed(Key::A)) {
				position -= right * deltaTime * Speed;
			}
			if (Input::IsKeyPressed(Key::D)) {
				position += right * deltaTime * Speed;
			}
		}

        void OnEvent(Event& e) {
            RE_PROFILE_FUNCTION();

            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<MouseMovedEvent>(RE_BIND_EVENT_FN(GameCamera::OnMouseMovement));
            dispatcher.Dispatch<WindowResizeEvent>(RE_BIND_EVENT_FN(GameCamera::OnWindowResize));
        }

        void HandleResolution(int width, int height) {
            ratio = (float)width / (float)height;

            projection = glm::perspective(glm::radians(fov), ratio, nearD, farD);
        }

        void UpdatePosition(glm::vec3 pos) {
            position = pos;
        }

		const glm::vec3& GetPosition() {
			return position;
		}

        glm::mat4 GetViewMatrix() {
            glm::vec3 intraVoxelPos = position - floor(position);
            return glm::lookAt(intraVoxelPos, intraVoxelPos + front, up);
        }

		glm::mat4 GetViewProjection() {
			return projection * GetViewMatrix();
		}

		float GetYaw() const { return yaw; }
		float GetPitch() const { return pitch; }

        bool OnMouseMovement(MouseMovedEvent& e) {
			static float lastX, lastY;

            float xOffset = (lastX - e.GetX()) * mouseSensitivity;
            float yOffset = (lastY - e.GetY()) * mouseSensitivity;

			lastX = e.GetX();
			lastY = e.GetY();

            yaw -= xOffset;
            pitch += yOffset;

            if (pitch > 89.9f) pitch = 89.9f;
            if (pitch < -89.9f) pitch = -89.9f;

            UpdateCameraVectors();

            return false;
        }

        bool OnWindowResize(WindowResizeEvent& e) {
			HandleResolution(e.GetWidth(), e.GetHeight());
            return false;
        }
    private:
        void UpdateCameraVectors() {
            glm::vec3 f;
            f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            f.y = sin(glm::radians(pitch));
            f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(f);
            right = glm::normalize(glm::cross(front, worldUp));
            up = glm::normalize(glm::cross(right, front));
        }
	private:
        glm::vec3 position;
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right;
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 projection;
        float yaw = YAW;
        float pitch = PITCH;
        float mouseSensitivity = SENSITIVITY;
		float Speed = 5.0f; // Movement speed
        float fov = FOV;
        float nearD = 1.0f;
        float farD = 10000.0f;
        float ratio;
    };
}
