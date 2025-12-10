#ifndef CAMERA_H
#define CAMERA_H

#include "header/globals.h"
#include "Component.h"

class wxMouseEvent;
class Camera : public Component
{
public:
	Camera(const glm::vec3& position, const glm::vec3& lookAt, float fovRadians, float _near, float _far);
	Camera(wxGLCanvas* canvas);
	~Camera() {}

private:
	float m_far;
	glm::vec3 m_forward;
	float m_fovRadians;
	float m_near;
	glm::vec3 m_right;
	float m_pitch;
	glm::mat4 m_projection;
	const glm::vec3 m_up = { 0, 1.0f, 0 };
	glm::mat4 m_view;
	float m_yaw;

	wxGLCanvas* m_canvas;
	double DeltaTime;
public:
	void setCanvas(wxGLCanvas* canvas) { m_canvas = canvas; }
	void updateDeltaTime(double deltaTime) { DeltaTime = deltaTime; }

	float      Far();
	bool       InputKeyboard(char key);
	void       InputMouseMove(const   wxMouseEvent& event, const MouseState& mouseState);
	void       InputMouseScroll(const wxMouseEvent& event);
	void       InvertPitch();
	void       MoveBy(const glm::vec3& amount)      override;
	void       MoveTo(const glm::vec3& newPosition) override;
	glm::mat4  MVP(const glm::mat4& model, bool removeTranslation = false);
	float      Near();
	//Component* Parent();
	void       Reset();
	void       RotateBy(const glm::vec3& amountRadians)      override;
	void       RotateTo(const glm::vec3& newRotationRadions) override;
	glm::mat4  Projection();
	void       SetFOV(const wxString& fov);
	void       UpdateProjection();
	glm::vec3  Up();
	glm::mat4  View(bool removeTranslation = false);

private:
	void init(const glm::vec3& position, const glm::vec3& lookAt);
	void updatePosition();
	void updateRotation() override;
};
#endif // CAMERA_H

