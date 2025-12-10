#include "Mesh.h"
#include "Texture.h"
#include "Buffer.h"
#include "utils/Utils.h"

Mesh::Mesh(Component* parent, const wxString& name) : Component(name)
{
	//this->boundingVolume = nullptr;
	this->m_isSelected = false;
	this->maxScale = 0.0f;
	this->Parent = parent;
	this->indexBuffer = nullptr;
	this->normalBuffer = nullptr;
	this->textureCoordsBuffer = nullptr;
	this->m_type = parent->Type();
	this->vertexBuffer = nullptr;
}

Mesh::Mesh() : Component("")
{
	//this->boundingVolume = nullptr;
	this->m_isSelected = false;
	this->maxScale = 0.0f;
	this->indexBuffer = nullptr;
	this->normalBuffer = nullptr;
	this->textureCoordsBuffer = nullptr;
	this->m_type = COMPONENT_MESH;
	this->vertexBuffer = nullptr;
}

Mesh::~Mesh()
{
	this->indices.clear();
	this->normals.clear();
	this->textureCoords.clear();
	this->vertices.clear();

	//_DELETEP(this->indexBuffer);
	//_DELETEP(this->normalBuffer);
	//_DELETEP(this->textureCoordsBuffer);
	//_DELETEP(this->vertexBuffer);

	//_DELETEP(this->boundingVolume);
}

void Mesh::BindBuffer(GLuint bufferID, GLuint shaderAttrib, GLsizei size, GLenum arrayType, GLboolean normalized, const GLvoid* offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glVertexAttribPointer(shaderAttrib, size, GL_FLOAT, normalized, Utils::GetStride(size, arrayType), offset);
	glEnableVertexAttribArray(shaderAttrib);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint Mesh::IBO()
{
	return (this->indexBuffer != nullptr ? this->indexBuffer->ID() : 0);
}

GLuint Mesh::NBO()
{
	return (this->normalBuffer != nullptr ? this->normalBuffer->ID() : 0);
}

GLuint Mesh::TBO()
{
	return (this->textureCoordsBuffer != nullptr ? this->textureCoordsBuffer->ID() : 0);
}

GLuint Mesh::VBO()
{
	return (this->vertexBuffer != nullptr ? this->vertexBuffer->ID() : 0);
}

bool Mesh::IsOK()
{
	return ((this->IBO() > 0) && (this->VBO() > 0));
}

bool Mesh::IsSelected()
{
	return m_isSelected;
}

bool Mesh::LoadModelFile(aiMesh* mesh, const aiMatrix4x4& transformMatrix)
{
	if (mesh == nullptr)
		return false;

	//this->Name = (mesh->mName.length > 0 ? mesh->mName.C_Str() : "Mesh");

	if (!this->loadModelData(mesh))
		return false;

	if (!this->setModelData())
		return false;

	// http://assimp.sourceforge.net/lib_html/classai_matrix4x4t.html
	// RenderEngine::Canvas.Window->SetStatusText("Decomposing the Transformation Matrix ...");

	aiVector3D position, rotation, scale;
	transformMatrix.Decompose(scale, rotation, position);

	//if (this->Parent->ModelFile() == Utils::RESOURCE_MODELS[ID_ICON_PLANE])
	if (this->m_type == COMPONENT_WATER) {
		scale.z = 10.0f;
		this->ComponentMaterial.specular.shininess = 20.0f;
	}

	this->updateModelData(position, scale, rotation);
	this->setMaxScale();
	this->SetBoundingVolume(BOUNDING_VOLUME_BOX);

	this->m_isValid = this->IsOK();

	return this->m_isValid;
}

int Mesh::LoadTextureImage(const wxString& imageFile, int index)
{
	if (textureCoords.empty()) {
		wxMessageBox("ERROR: The model is missing texture coordinates.", "$$"/*RenderEngine::Canvas.Window->GetTitle().c_str()*/, wxOK | wxICON_ERROR);
		return -1;
	}

	this->Textures[index] = new Texture(imageFile, (index == 0));

	return 0;
}

size_t Mesh::NrOfIndices()
{
	return this->indices.size();
}

size_t Mesh::NrOfVertices()
{
	return (this->vertices.size() / 3);
}

void Mesh::SetBoundingVolume(BoundingVolumeType type)
{
	//if (this->boundingVolume != nullptr)
	//	_DELETEP(this->boundingVolume);

	//if (type != BOUNDING_VOLUME_NONE) {
	//	this->boundingVolume = new BoundingVolume(this, type, (this->maxScale + 0.01f));
	//	this->boundingVolume->Update();
	//}
}

void Mesh::UpdateBoundingVolume()
{
	//if (this->boundingVolume != nullptr)
	//	this->boundingVolume->Update();
}

bool Mesh::loadModelData(aiMesh* mesh)
{
	if (mesh == nullptr)
		return false;
	unsigned int i, j;
	// INDICES (FACES)
	//RenderEngine::Canvas.Window->SetStatusText("Loading the Indices ...");
	for (i = 0; i < mesh->mNumFaces; i++) {
		for (j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			this->indices.push_back(mesh->mFaces[i].mIndices[j]);
	}

	// NORMALS
	//RenderEngine::Canvas.Window->SetStatusText("Loading the Normals ...");
	for (i = 0; i < mesh->mNumVertices; i++) {
		this->normals.push_back(mesh->mNormals[i].x);
		this->normals.push_back(mesh->mNormals[i].y);
		this->normals.push_back(mesh->mNormals[i].z);
	}

	// TEXTURE COORDINATES
	//RenderEngine::Canvas.Window->SetStatusText("Loading the Texture Coordinates ...");
	for (i = 0; i < mesh->mNumVertices; i++) {
		if ((mesh->mTextureCoords != nullptr) && (mesh->mTextureCoords[0] != nullptr)) {
			this->textureCoords.push_back(mesh->mTextureCoords[0][i].x);
			this->textureCoords.push_back(mesh->mTextureCoords[0][i].y);
		}
	}

	// VERTICES (POSITION/LOCATIONS)
	//RenderEngine::Canvas.Window->SetStatusText("Loading the Vertices ...");
	for (i = 0; i < mesh->mNumVertices; i++) {
		this->vertices.push_back(mesh->mVertices[i].x);
		this->vertices.push_back(mesh->mVertices[i].y);
		this->vertices.push_back(mesh->mVertices[i].z);
	}

	return true;
}

bool Mesh::setModelData()
{
	if (!this->indices.empty())
		this->indexBuffer = new Buffer(this->indices);

	if (!this->normals.empty())
		this->normalBuffer = new Buffer(this->normals);

	if (!this->textureCoords.empty())
		this->textureCoordsBuffer = new Buffer(this->textureCoords);

	if (!this->vertices.empty())
		this->vertexBuffer = new Buffer(this->vertices);
	return true;
}

void Mesh::updateModelData()
{
	this->MoveTo(this->m_position);
	this->ScaleTo(this->m_scale);
	this->RotateTo(this->m_rotation);

	for (int i = 0; i < MAX_TEXTURES; i++) {
		if (this->Textures[i] == nullptr)
			//this->LoadTexture(SceneManager::EmptyTexture, i);
			this->LoadTexture(new Texture(""), i);
	}
}

void Mesh::setMaxScale()
{
	for (auto vertex : this->vertices)
		this->maxScale = std::max(this->maxScale, std::abs(vertex));
}

void Mesh::updateModelData(const aiVector3D& position, const aiVector3D& scale, aiVector3D& rotation)
{
	this->MoveTo(glm::vec3(position.x, position.y, position.z));
	this->ScaleTo(glm::vec3(scale.x, scale.y, scale.z));
	this->RotateTo(glm::vec3(rotation.x, rotation.y, rotation.z));

	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (!this->ComponentMaterial.textures[i].empty())
			this->LoadTextureImage(this->ComponentMaterial.textures[i], i);

		if (this->Textures[i] == nullptr)
			this->LoadTexture(new Texture(""), i);
	}
}
