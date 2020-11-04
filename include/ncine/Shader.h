#ifndef CLASS_NCINE_SHADER
#define CLASS_NCINE_SHADER

#include "Object.h"

namespace ncine {

class GLShaderProgram;

/// Shader class
class DLL_PUBLIC Shader : public Object
{
  public:
	enum class LoadMode
	{
		STRING,
		FILE
	};
/*
	enum class Type
	{
		VERTEX,
		FRAGMENT
	};
*/

	Shader(LoadMode loadMode, const char *vertex, const char *fragment);
	//Shader(const char *string, Type type);
	//Shader(const char *filename, Type type);

	// TODO: support a way to reload shaders

	//static Shader CreateFromStrings(const char *vertexString, const char *fragmentString);
	//static Shader CreateFromFiles(const char *vertexName, const char *fragmentName);

	inline static ObjectType sType() { return ObjectType::SHADER; }

	/// Returns true if the shader is linked and can therefore be used
	bool isLinked();

  private:
	/// The OpenGL shader program
	nctl::UniquePtr<GLShaderProgram> glShaderProgram_;

	/// Deleted copy constructor
	Shader(const Shader &) = delete;
	/// Deleted assignment operator
	Shader &operator=(const Shader &) = delete;

	/// Sets the OpenGL object label for the shader program
	//void setGLShaderProgramLabel(const char *filename);

	friend class ShaderState;
};

}

#endif
