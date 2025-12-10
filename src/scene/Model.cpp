#include "Model.h"
#include <utils/Utils.h>

Model::Model(const wxString& modelFile) : Component("Model")
{
	this->m_modelFile = modelFile;
	this->m_type = COMPONENT_MODEL;
	this->Children = Utils::LoadModelFile(modelFile, this);
	this->m_isValid = !this->Children.empty();
}
