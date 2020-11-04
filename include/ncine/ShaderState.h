#ifndef CLASS_NCINE_SHADERSTATE
#define CLASS_NCINE_SHADERSTATE

#include "common_defines.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class Material;
class Shader;

/// Shader state class
class DLL_PUBLIC ShaderState
{
  public:
	ShaderState(Material &material);

	inline const Shader *shader() const { return shader_; }
	bool setShader(Shader *shader);

	bool setAttribute(const char *name, int stride, unsigned long int pointer);

	bool setUniformInt(const char *blockName, const char *name, const int *vector);
	bool setUniformInt(const char *blockName, const char *name, int value0);
	bool setUniformInt(const char *blockName, const char *name, int value0, int value1);
	bool setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2);
	bool setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2, int value3);

	bool setUniformFloat(const char *blockName, const char *name, const float *vector);
	bool setUniformFloat(const char *blockName, const char *name, float value0);
	bool setUniformFloat(const char *blockName, const char *name, float value0, float value1);
	bool setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2);
	bool setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2, float value3);

	// TODO: Set texture (maybe more than one?) and recheck `updateRenderCommand`

  private:
	/// The shader object
	Shader *shader_;
	Material &material_;
	int previousShaderType_;

	/// Deleted copy constructor
	ShaderState(const ShaderState &) = delete;
	/// Deleted assignment operator
	ShaderState &operator=(const ShaderState &) = delete;
};

}

#endif
