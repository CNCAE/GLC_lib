/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.0.0, packaged on August, 2008.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_shader.cpp implementation of the GLC_Shader class.

#include "glc_shader.h"
#include <QTextStream>
#include "glc_exception.h"

QStack<GLuint> GLC_Shader::m_ProgrammStack;

GLuint GLC_Shader::m_CurrentProgramm= 0;

GLC_Shader::GLC_Shader()
: m_VertexByteArray()
, m_VertexShader(0)
, m_FragmentByteArray()
, m_FragmentShader(0)
, m_ProgramShader(0)
{
}
// Construct shader with specifie vertex and fragment
GLC_Shader::GLC_Shader(QFile& vertex, QFile& fragment)
: m_VertexByteArray()
, m_VertexShader(0)
, m_FragmentByteArray()
, m_FragmentShader(0)
, m_ProgramShader(0)
{
	setVertexAndFragmentShader(vertex, fragment);
}

GLC_Shader::~GLC_Shader()
{
	
	if (m_ProgramShader != 0)
	{
		// Detach shaders associted with the programm
		glDetachShader(m_ProgramShader, m_VertexShader);
		glDetachShader(m_ProgramShader, m_FragmentShader);
		// Delete the shader
		glDeleteShader(m_VertexShader);
		glDeleteShader(m_FragmentShader);
		// Delete the programm
		glDeleteProgram(m_ProgramShader);
	}
	// If m_ProgramShader == 0 then there is no vertes and fragment shader
}
//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Use this shader programm
void GLC_Shader::useProgramm()
{
	if (m_CurrentProgramm != 0)
	{
		m_ProgrammStack.push(m_CurrentProgramm);
	}
	m_CurrentProgramm= m_ProgramShader;
	glUseProgram(m_CurrentProgramm);
}

// Use previous programm shader
void GLC_Shader::usePreviousProgramm()
{
	if (m_ProgrammStack.isEmpty())
	{
		m_CurrentProgramm= 0;
	}
	else
	{
		m_CurrentProgramm= m_ProgrammStack.pop();
	}
	glUseProgram(m_CurrentProgramm);
}
// Compile and attach shaders to a program shader
void GLC_Shader::createAndCompileProgrammShader()
{
	createAndLinkVertexShader();
	createAndLinkFragmentShader();
	
	m_ProgramShader = glCreateProgram();
	glAttachShader(m_ProgramShader, m_VertexShader);
	glAttachShader(m_ProgramShader, m_FragmentShader);

	glLinkProgram(m_ProgramShader);

	// Check if the program as been linked successfully	
	GLint params;
	glGetProgramiv(m_ProgramShader, GL_LINK_STATUS, &params);
	if (params != GL_TRUE)
	{
		QString message("GLC_Shader::setVertexAndFragmentShader Failed to link program ");
		GLC_Exception exception(message);
		throw(exception);
	}	
}

// Create and compile vertex shader
void GLC_Shader::createAndLinkVertexShader()
{
	
	const char* pVertexShaderData= m_VertexByteArray.data();
	
	m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_VertexShader, 1, &pVertexShaderData, NULL);
	glCompileShader(m_VertexShader);
	
	// Check if the shader compilation is successfull
	GLint params;
	glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE)
	{
		QString message("GLC_Shader::createAndLinkVertexShader Failed to compile Vertex shader");
		GLC_Exception exception(message);
		throw(exception);
	}	
}

// Create and compile fragment shader
void GLC_Shader::createAndLinkFragmentShader()
{
	const char* pFragmentShaderData= m_FragmentByteArray.data();
	
	m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_FragmentShader, 1, &pFragmentShaderData, NULL);
	glCompileShader(m_FragmentShader);
		
	// Check if the shader compilation is successfull
	GLint params;
	glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &params);
	if (params != GL_TRUE)
	{
		QString message("GLC_Shader::createAndLinkFragmentShader Failed to compile fragment shader");
		GLC_Exception exception(message);
		throw(exception);
	}	
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Vertex and fragment shaders
void GLC_Shader::setVertexAndFragmentShader(QFile& vertexFile, QFile& fragmentFile)
{
	setVertexShader(vertexFile);
	setFragmentShader(fragmentFile);	
}

//////////////////////////////////////////////////////////////////////
// Private services Functions
//////////////////////////////////////////////////////////////////////
// Set Vertex shader
void GLC_Shader::setVertexShader(QFile& vertexFile)
{
	m_VertexByteArray= readShaderFile(vertexFile);
}

// Set fragment shaders
void GLC_Shader::setFragmentShader(QFile& fragmentFile)
{
	m_FragmentByteArray= readShaderFile(fragmentFile);
}

// Return char* of an Ascii file
QByteArray GLC_Shader::readShaderFile(QFile& shaderFile)
{
	if (!shaderFile.open(QIODevice::ReadOnly))
	{
		QString message(QString("GLC_Shader::readShaderFile Failed to open file : ") + shaderFile.fileName());
		GLC_Exception exception(message);
		throw(exception);
	}
	QByteArray result(shaderFile.readAll());
	result.append('\0');
	return result;
}

