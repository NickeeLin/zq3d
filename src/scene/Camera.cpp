#include "Camera.h"
#include "utils/Utils.h"
#include <time/TimeManager.h>
#include <render/RenderEngine.h>

Camera::Camera(const glm::vec3& position, const glm::vec3& lookAt, float fovRadians, float _near, float _far) : Component("Camera", position),
m_far(_far), m_near(_near), m_fovRadians(fovRadians)
{
	this->m_pitch = 0;
	this->m_yaw = -(glm::pi<float>() * 0.5f);
	this->m_type = COMPONENT_CAMERA;
	this->m_isValid = true;

	this->init(position, lookAt);
}

Camera::Camera() : Component("Camera")
{
	this->Reset();
}

float Camera::Far()
{
	return m_far;
}

bool Camera::InputKeyboard(char key)
{
	glm::vec3    moveVector;
	const double MOVE_SPEED = 20.0;
	float        moveModifier = (TimeManager::DeltaTime * MOVE_SPEED);
	glm::vec3    moveAmount = { moveModifier, moveModifier, moveModifier };
	bool         result = false;

	switch (toupper(key)) {
	case 'W':
		moveVector = (this->m_forward * moveAmount);
		result = true;
		break;
	case 'A':
		moveVector = glm::normalize(glm::cross(this->m_forward, this->m_up));
		moveVector *= -moveAmount;
		result = true;
		break;
	case 'S':
		moveVector = (this->m_forward * -moveAmount);
		result = true;
		break;
	case 'D':
		moveVector = glm::normalize(glm::cross(this->m_forward, this->m_up));
		moveVector *= moveAmount;
		result = true;
		break;
	default:
		break;
	}

	if (result)
		this->MoveBy(moveVector);

	return result;
}

void Camera::InputMouseMove(const wxMouseEvent& event, const MouseState& mouseState)
{
	glm::vec3    moveVector;
	const double MOVE_SPEED = 3.0;
	glm::vec2    mouseMovement = { (event.GetX() - mouseState.Position.x), (event.GetY() - mouseState.Position.y) };
	glm::vec2    moveModifier = { (mouseMovement.x * TimeManager::DeltaTime * MOVE_SPEED), (mouseMovement.y * TimeManager::DeltaTime * MOVE_SPEED) };
	glm::vec3    moveAmountX = { -moveModifier.x, -moveModifier.x, -moveModifier.x };
	glm::vec3    moveAmountY = { -moveModifier.y, -moveModifier.y, -moveModifier.y };

	// MOVE/PAN HORIZONTAL/VERTICAL
	if (event.GetModifiers() == wxMOD_SHIFT)
	{
		moveVector = glm::normalize(glm::cross(this->m_forward, this->m_up));
		moveVector *= moveAmountX;

		this->MoveBy(moveVector);
		this->MoveBy({ 0, moveModifier.y, 0 });
		
	}
	// MOVE/PAN FORWARD/BACK (Z)
	else if (event.GetModifiers() == wxMOD_CONTROL) {
		this->MoveBy(this->m_forward * moveAmountY);
		
	}
	// ROTATE HORIZONTAL/VERTICAL (YAW/PITCH)
	else {
		this->RotateBy({ -(moveModifier.y * 0.01f), (moveModifier.x * 0.01f), 0 });
	}
}

void Camera::InputMouseScroll(const wxMouseEvent& event)
{
	glm::vec3    moveVector;
	const double MOVE_SPEED = 20.0;
	float        moveModifier = ((std::signbit((float)event.GetWheelRotation()) ? -1.0 : 1.0) * TimeManager::DeltaTime * MOVE_SPEED);
	glm::vec3    moveAmount = { moveModifier, moveModifier, moveModifier };

	// UP / DOWN (Y)
	if (event.GetModifiers() == wxMOD_SHIFT) {
		moveVector = { 0, moveModifier, 0 };
		// LEFT / RIGHT (X)
	}
	else if (event.GetModifiers() == wxMOD_CONTROL) {
		moveVector = glm::normalize(glm::cross(this->m_forward, this->m_up));
		moveVector *= moveAmount;
		// FORWARD / BACK (Z)
	}
	else {
		moveVector = (this->m_forward * moveAmount);
	}

	this->MoveBy(moveVector);
}

void Camera::InvertPitch()
{
	this->m_pitch = -this->m_pitch;
	this->RotateTo({ this->m_pitch, this->m_yaw, 0 });
}

void Camera::MoveBy(const glm::vec3& amount)
{
	this->m_position += amount;
	this->updatePosition();
}

void Camera::MoveTo(const glm::vec3& newPosition)
{
	this->m_position = newPosition;
	this->updatePosition();
}

glm::mat4 Camera::MVP(const glm::mat4& model, bool removeTranslation)
{
	return (this->m_projection * this->View(removeTranslation) * model);
}

float Camera::Near()
{
	return m_near;
}

void Camera::Reset()
{
	this->m_position = { 0.0f, 2.5f, 10.0f };
	this->m_fovRadians = (glm::pi<float>() * 0.25f);
	this->m_near = 0.1f;
	this->m_far = 100.0f;
	this->m_pitch = 0;
	this->m_yaw = -(glm::pi<float>() * 0.5f);
	this->m_type = COMPONENT_CAMERA;
	this->m_isValid = true;

	this->init(this->m_position, {});
}

void Camera::RotateBy(const glm::vec3& amountRadians)
{
	this->m_pitch += amountRadians.x;
	this->m_yaw += amountRadians.y;

	this->updateRotation();
}

void Camera::RotateTo(const glm::vec3& newRotationRadions)
{
	this->m_pitch = newRotationRadions.x;
	this->m_yaw = newRotationRadions.y;

	this->updateRotation();
}

glm::mat4 Camera::Projection()
{
	return m_projection;
}

void Camera::SetFOV(const wxString& fov)
{
	this->m_fovRadians = Utils::ToRadians((float)std::atof(fov.c_str()));
	this->UpdateProjection();
}

void Camera::UpdateProjection()
{
	const wxSize ClientSize = RenderEngine::Canvas.Size;
	float aspectRatio = (float)((float)ClientSize.GetWidth() / (float)ClientSize.GetHeight());
	this->m_projection = glm::perspective(this->m_fovRadians, aspectRatio, this->m_near, this->m_far);
}

glm::vec3 Camera::Up()
{
	return this->m_up;
}

glm::mat4 Camera::View(bool removeTranslation)
{
	return m_view;
}

void Camera::init(const glm::vec3& position, const glm::vec3& lookAt)
{
	this->m_forward = glm::normalize(lookAt - position);
	this->m_right = glm::normalize(glm::cross(this->m_up, this->m_forward));

	this->UpdateProjection();
	this->updatePosition();
	this->updateRotation();
}

void Camera::updatePosition()
{
	glm::vec3 center = (this->m_position + this->m_forward);
	this->m_view = glm::lookAt(this->m_position, center, this->m_up);
}

void Camera::updateRotation()
{
	// https://learnopengl.com/#!Getting-started/Camera
	this->m_pitch = std::max(std::min(this->m_pitch, (glm::pi<float>() * 0.5f)), -(glm::pi<float>() * 0.5f));
	this->m_rotation = { this->m_pitch, this->m_yaw, 0 };

	glm::vec3 center = {
		(std::cos(this->m_pitch) * std::cos(this->m_yaw)),	// X
		std::sin(this->m_pitch),							// Y
		(std::cos(this->m_pitch) * std::sin(this->m_yaw))	// Z
	};

	this->m_forward = glm::normalize(center);

	center = (this->m_position + this->m_forward);
	this->m_view = glm::lookAt(this->m_position, center, this->m_up);
}
