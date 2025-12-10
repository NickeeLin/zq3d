#ifndef MESH_H
#define MESH_H

#include "header/globals.h"
#include "Component.h"

class Buffer;
class BoundingVolume;
class Mesh : public Component
{
public:
	Mesh(Component* parent, const wxString& name);
	Mesh();
	virtual ~Mesh();

protected:
	std::vector<unsigned int> indices;
	std::vector<float>        normals;
	std::vector<float>        textureCoords;
	std::vector<float>        vertices;
	Buffer* indexBuffer;
	Buffer* normalBuffer;
	Buffer* textureCoordsBuffer;
	Buffer* vertexBuffer;

private:
	BoundingVolume* boundingVolume;
	bool            m_isSelected;
	float           maxScale;

public:
	void BindBuffer(GLuint bufferID, GLuint shaderAttrib, GLsizei size, GLenum arrayType, GLboolean normalized, const GLvoid* offset = nullptr);
	GLuint IBO();
	GLuint NBO();
	GLuint TBO();
	GLuint VBO();
	bool IsOK();
	bool IsSelected();
	bool LoadModelFile(aiMesh* mesh, const aiMatrix4x4& transformMatrix);
	int	 LoadTextureImage(const wxString& imageFile, int index);

	size_t NrOfIndices();
	size_t NrOfVertices();

	void SetBoundingVolume(BoundingVolumeType type);
	void UpdateBoundingVolume();
protected:
	bool loadModelData(aiMesh* mesh);
	bool setModelData();
	void updateModelData();

private:
	void setMaxScale();
	void updateModelData(const aiVector3D& position, const aiVector3D& scale, aiVector3D& rotation);
};

#endif // MESH_H