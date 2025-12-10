#ifndef COMPONENT_H
#define COMPONENT_H
#include "header/globals.h"
#include "Material.h"

enum ComponentType
{
	COMPONENT_UNKNOWN = -1,
	COMPONENT_CAMERA,
	COMPONENT_HUD,
	COMPONENT_MESH,
	COMPONENT_MODEL,
	COMPONENT_SKYBOX,
	COMPONENT_TERRAIN,
	COMPONENT_WATER,
	COMPONENT_LIGHTSOURCE
};

class Texture;
class Mesh;
class Component
{
public:
	Component(const wxString& name = "", const glm::vec3& position = {});
	virtual ~Component();

public:
	bool                    AutoRotate;
	glm::vec3               AutoRotation;
	std::vector<Component*> Children;
	Material                ComponentMaterial;
	//bool                    LockToParentPosition;
	//bool                    LockToParentRotation;
	//bool                    LockToParentScale;
	wxString                Name;
	Component* Parent;
	Texture* Textures[MAX_TEXTURES];

protected:
	static uint32_t sid;

	uint32_t      m_id;
	bool          m_isValid;
	glm::mat4     m_matrix;
	wxString      m_modelFile;
	glm::vec3     m_rotation;
	glm::mat4     m_rotationMatrix;
	glm::vec3     m_scale;
	glm::mat4     m_scaleMatrix;
	glm::vec3     m_position;
	glm::mat4     m_translationMatrix;
	ComponentType m_type;

public:
	int           GetChildIndex(Component* child);
	uint32_t      ID();
	bool          IsTextured(int index);
	bool          IsValid();
	void          LoadTexture(Texture* texture, int index);
	glm::mat4     Matrix();
	wxString      ModelFile();
	virtual void  MoveBy(const glm::vec3& amount);
	virtual void  MoveTo(const glm::vec3& newPosition);
	glm::vec3     Position();
	int           RemoveChild(Mesh* child);
	glm::vec3     Rotation();
	virtual void  RotateBy(const glm::vec3& amountRadians);
	virtual void  RotateTo(const glm::vec3& newRotationRadions);
	glm::vec3     Scale();
	virtual void  ScaleBy(const glm::vec3& amount);
	virtual void  ScaleTo(const glm::vec3& newScale);
	ComponentType Type();

protected:
	void         updateMatrix();
	virtual void updateRotation();
	void         updateScale();
	void         updateTranslation();

};

#endif
