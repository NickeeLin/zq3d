#ifndef MODEL_H
#define MODEL_H

#include "Component.h"
class Model : public Component
{
public:
	Model(const wxString& modelFile);
	Model() {}
	~Model() {}
};

#endif // MODEL_H