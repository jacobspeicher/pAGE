#pragma once

#include <glew/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum class CAMERA_MOVEMENT {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
};

// default camera values
const float YAW			= -90.0f;
const float PITCH		=   -45.0f;
const float SPEED		=   5.0f;
const float SENSITIVITY =   0.1f;
const float ZOOM		=  45.0f;

class Camera {
public:
	// camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// euler angles
	float Yaw;
	float Pitch;

	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// returns the view matrix calculated using euler angles and the LookAt matrix
	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

	// process keyboard input
	void ProcessKeyboard(CAMERA_MOVEMENT direction, float deltaTime) {
		float velocity = MovementSpeed * deltaTime;

		switch (direction) {
		case CAMERA_MOVEMENT::FORWARD:
			Position += Front * velocity;
			break;
		case CAMERA_MOVEMENT::BACKWARD:
			Position -= Front * velocity;
			break;
		case CAMERA_MOVEMENT::LEFT:
			Position -= Right * velocity;
			break;
		case CAMERA_MOVEMENT::RIGHT:
			Position += Right * velocity;
			break;
		}
	}

	// process mouse input
	void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
		xOffset *= MouseSensitivity;
		yOffset *= MouseSensitivity;

		Yaw += xOffset;
		Pitch += yOffset;

		// make sure that when pitch is out of bounds, the screen doesn't get flipepd
		if (constrainPitch) {
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			else if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

private:
	// calculates the Front vector from the camera's euler angles
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		// recalculates the Up and Right vectors
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};