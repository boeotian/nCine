#include "ShaderState.h"
#include "Shader.h"
#include "Material.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ShaderState::ShaderState(Material &material)
    : material_(material)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ShaderState::setShader(Shader *shader)
{
	bool shaderHasChanged = false;

	if (shader != shader_)
	{
		if (shader == nullptr)
		{
			const Material::ShaderProgramType programType = static_cast<Material::ShaderProgramType>(previousShaderType_);
			material_.setShaderProgramType(programType);
		}
		else if (shader->isLinked())
		{
			if (material_.shaderProgramType() != Material::ShaderProgramType::CUSTOM)
				previousShaderType_ = static_cast<int>(material_.shaderProgramType());

			shader_ = shader;
			material_.setShaderProgram(shader_->glShaderProgram_.get());
			material_.setUniformsDataPointer(nullptr);
		}

		shaderHasChanged = true;
	}

	return shaderHasChanged;
}

bool ShaderState::setAttribute(const char *name, int stride, unsigned long int pointer)
{
	// TODO: Add semantics
	// TODO: Check if changing an attribute from a state affects all states that share the same shader, write a note in case

	if (shader_ == nullptr || name == nullptr)
		return false;

	if (material_.attribute(name))
	{
		material_.attribute(name)->setVboParameters(stride, reinterpret_cast<void*>(pointer));
		return true;
	}

	return false;
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, const int *vector)
{
	if (shader_ == nullptr || name == nullptr || vector == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setIntVector(vector);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setIntVector(vector);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setIntValue(value0);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setIntValue(value0);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0, int value1)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setIntValue(value0, value1);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setIntValue(value0, value1);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setIntValue(value0, value1, value2);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setIntValue(value0, value1, value2);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2, int value3)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setIntValue(value0, value1, value2, value3);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setIntValue(value0, value1, value2, value3);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, const float *vector)
{
	if (shader_ == nullptr || name == nullptr || vector == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setFloatVector(vector);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setFloatVector(vector);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setFloatValue(value0);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setFloatValue(value0);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0, float value1)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setFloatValue(value0, value1);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setFloatValue(value0, value1);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setFloatValue(value0, value1, value2);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setFloatValue(value0, value1, value2);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2, float value3)
{
	if (shader_ == nullptr || name == nullptr)
		return false;

	bool uniformHasChanged = false;

	if (blockName)
	{
		if (material_.uniformBlock(blockName) && material_.uniformBlock(blockName)->uniform(name))
		{
			material_.uniformBlock(blockName)->uniform(name)->setFloatValue(value0, value1, value2, value3);
			uniformHasChanged = true;
		}
	}
	else
	{
		if (material_.uniform(name))
		{
			material_.uniform(name)->setFloatValue(value0, value1, value2, value3);
			uniformHasChanged = true;
		}
	}

	return uniformHasChanged;
}

}
