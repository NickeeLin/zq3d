#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "header/globals.h"

class ShaderProgram;
class ShaderManager
{
public:
	ShaderManager() {};
	~ShaderManager() {};

public:
	static ShaderProgram* Programs[NR_OF_SHADERS];

public:
	static void Close();
	static int  Init();
};

#endif // !SHADERMANAGER_H
