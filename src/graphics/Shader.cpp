#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>
#include "Shader.h"
#include "GLShaderProgram.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

// TODO: add a way to set a stage from a default shader

Shader::Shader(LoadMode loadMode, const char *vertex, const char *fragment)
    : Object(ObjectType::SHADER, "Custom Shader"),
      glShaderProgram_(nctl::makeUnique<GLShaderProgram>(GLShaderProgram::QueryPhase::IMMEDIATE))
{
	ZoneScoped;
	ZoneText("Custom Shader", nctl::strnlen("Custom Shader", nctl::String::MaxCStringLength));

	glShaderProgram_->setFatalAssertOnErrors(false);
	switch (loadMode)
	{
		case LoadMode::STRING:
			glShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, vertex);
			glShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, fragment);
		break;
		case LoadMode::FILE:
		default:
			glShaderProgram_->attachShader(GL_VERTEX_SHADER, vertex);
			glShaderProgram_->attachShader(GL_FRAGMENT_SHADER, fragment);
			break;
	}

	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);
}

/*
Shader Shader::CreateFromStrings(const char *vertexString, const char *fragmentString)
{
	return Shader(LoadMode::STRING, vertexString, fragmentString);
}

Shader Shader::CreateFromFiles(const char *vertexName, const char *fragmentName)
{
	return Shader(LoadMode::FILE, vertexName, fragmentName);
}
*/

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Shader::isLinked()
{
	return (glShaderProgram_->status() != GLShaderProgram::Status::NOT_LINKED &&
	        glShaderProgram_->status() != GLShaderProgram::Status::LINKING_FAILED);
}

}
