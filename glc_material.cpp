/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.4, packaged on January, 2006.

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

//! \file glc_material.cpp implementation of the GLC_Material class.

#include <QtDebug>

#include "glc_material.h"
#include "glc_collection.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_Material::GLC_Material(const char *pName ,const GLfloat *pAmbiantColor)
:GLC_Object(pName)
, m_fShininess(50.0)		// Par d�faut, shininess 50
, m_pTexture(NULL)			// Pas de texture
{

	// Initialisation de tableau contenant la couleur
	if (pAmbiantColor != 0)
	{
		m_AmbientColor[0]= pAmbiantColor[0];
		m_AmbientColor[1]= pAmbiantColor[1];
		m_AmbientColor[2]= pAmbiantColor[2];
		m_AmbientColor[3]= pAmbiantColor[3];
	}
	else
	{	// By default light's color is dark grey.
		m_AmbientColor[0]= 0.2f;
		m_AmbientColor[1]= 0.2f;
		m_AmbientColor[2]= 0.2f;
		m_AmbientColor[3]= 1.0f;
	}
	// Les autres 
	// Couleur diffuse
	m_DiffuseColor[0]= 1.0f;
	m_DiffuseColor[1]= 1.0f;
	m_DiffuseColor[2]= 1.0f;
	m_DiffuseColor[3]= 1.0f;

	// Couleur sp�culaire
	m_SpecularColor[0]= 1.0f;
	m_SpecularColor[1]= 1.0f;
	m_SpecularColor[2]= 1.0f;
	m_SpecularColor[3]= 1.0f;

	// Emission de lumi�re
	m_LightEmission[0]= 0.0f;
	m_LightEmission[1]= 0.0f;
	m_LightEmission[2]= 0.0f;
	m_LightEmission[3]= 1.0f;

}
GLC_Material::GLC_Material(GLC_Texture* pTexture, const char *pName)
:GLC_Object(pName)
, m_fShininess(50.0)		// By default shininess 50
//! \todo something to do about texture memory allocation.
, m_pTexture(pTexture)		// Init texture 
{
	// Ambiente Color
	m_AmbientColor[0]= 0.2f;
	m_AmbientColor[1]= 0.2f;
	m_AmbientColor[2]= 0.2f;
	m_AmbientColor[3]= 1.0f;

	// Diffuse Color
	m_DiffuseColor[0]= 1.0f;
	m_DiffuseColor[1]= 1.0f;
	m_DiffuseColor[2]= 1.0f;
	m_DiffuseColor[3]= 1.0f;

	// Specular Color
	m_SpecularColor[0]= 1.0f;
	m_SpecularColor[1]= 1.0f;
	m_SpecularColor[2]= 1.0f;
	m_SpecularColor[3]= 1.0f;

	// Lighting emit
	m_LightEmission[0]= 0.0f;
	m_LightEmission[1]= 0.0f;
	m_LightEmission[2]= 0.0f;
	m_LightEmission[3]= 1.0f;

}

// Copy constructor
GLC_Material::GLC_Material(const GLC_Material &InitMaterial)
:GLC_Object(InitMaterial.GetName())
, m_fShininess(InitMaterial.m_fShininess)
, m_pTexture(NULL)
{
	if (NULL != InitMaterial.m_pTexture)
	{
		m_pTexture= new GLC_Texture(*(InitMaterial.m_pTexture));
		assert(m_pTexture != NULL);
	}
	
	// Ambiente Color
	m_AmbientColor[0]= InitMaterial.m_AmbientColor[0];
	m_AmbientColor[1]= InitMaterial.m_AmbientColor[1];
	m_AmbientColor[2]= InitMaterial.m_AmbientColor[2];
	m_AmbientColor[3]= InitMaterial.m_AmbientColor[3];

	// Diffuse Color
	m_DiffuseColor[0]= InitMaterial.m_DiffuseColor[0];
	m_DiffuseColor[1]= InitMaterial.m_DiffuseColor[1];
	m_DiffuseColor[2]= InitMaterial.m_DiffuseColor[2];
	m_DiffuseColor[3]= InitMaterial.m_DiffuseColor[3];

	// Specular Color
	m_SpecularColor[0]= InitMaterial.m_SpecularColor[0];
	m_SpecularColor[1]= InitMaterial.m_SpecularColor[1];
	m_SpecularColor[2]= InitMaterial.m_SpecularColor[2];
	m_SpecularColor[3]= InitMaterial.m_SpecularColor[3];

	// Lighting emit
	m_LightEmission[0]= InitMaterial.m_LightEmission[0];
	m_LightEmission[1]= InitMaterial.m_LightEmission[1];
	m_LightEmission[2]= InitMaterial.m_LightEmission[2];
	m_LightEmission[3]= InitMaterial.m_LightEmission[3];
		
}

// Destructor
GLC_Material::~GLC_Material(void)
{

	// Remove material from Geometries
	CGeomMap::iterator iEntry= m_WhereUsed.begin();
	
    while (iEntry != m_WhereUsed.constEnd())
    {
        // Remove material from geometry
        iEntry.value()->DelMaterial(iEntry.value());
        ++iEntry;
    }
    // clear whereUSED Hash table
    m_WhereUsed.clear();
    
    if (NULL != m_pTexture)
    {
   		delete m_pTexture;
    	m_pTexture= NULL;
    }
    

}


//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////

// Get Ambiant color
void GLC_Material::GetAmbientColor(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_AmbientColor[0];
	pfCol[1]= m_AmbientColor[1];
	pfCol[2]= m_AmbientColor[2];
	pfCol[3]= m_AmbientColor[3];
}

// Get diffuse color
void GLC_Material::GetDiffuseColor(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_DiffuseColor[0];
	pfCol[1]= m_DiffuseColor[1];
	pfCol[2]= m_DiffuseColor[2];
	pfCol[3]= m_DiffuseColor[3];
}

// Get specular color
void GLC_Material::GetSpecularColor(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_SpecularColor[0];
	pfCol[1]= m_SpecularColor[1];
	pfCol[2]= m_SpecularColor[2];
	pfCol[3]= m_SpecularColor[3];
}

// Get the emissive color
void GLC_Material::GetLightEmission(GLfloat* pfCol) const	// pfCol[4]
{
	pfCol[0]= m_LightEmission[0];
	pfCol[1]= m_LightEmission[1];
	pfCol[2]= m_LightEmission[2];
	pfCol[3]= m_LightEmission[3];
}

//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

// Set Ambiant Color
void GLC_Material::SetAmbientColor(const GLfloat* pfCol)
{
	m_AmbientColor[0]= pfCol[0];
	m_AmbientColor[1]= pfCol[1];
	m_AmbientColor[2]= pfCol[2];
	m_AmbientColor[3]= pfCol[3];

	UpdateUsed();
}

// Set Diffuse color
void GLC_Material::SetDiffuseColor(const GLfloat* pfCol)
{
	m_DiffuseColor[0]= pfCol[0];
	m_DiffuseColor[1]= pfCol[1];
	m_DiffuseColor[2]= pfCol[2];
	m_DiffuseColor[3]= pfCol[3];

	UpdateUsed();
}

// Set Specular color
void GLC_Material::SetSpecularColor(const GLfloat* pfCol)
{
	m_SpecularColor[0]= pfCol[0];
	m_SpecularColor[1]= pfCol[1];
	m_SpecularColor[2]= pfCol[2];
	m_SpecularColor[3]= pfCol[3];

	UpdateUsed();
}

// Set Emissive
void GLC_Material::SetLightEmission(const GLfloat* pfCol)
{
	m_LightEmission[0]= pfCol[0];
	m_LightEmission[1]= pfCol[1];
	m_LightEmission[2]= pfCol[2];
	m_LightEmission[3]= pfCol[3];

	UpdateUsed();
}

// Add Geometry to where used hash table
bool GLC_Material::AddGLC_Geom(GLC_Geometry* pGeom)
{
	CWhereUsed::iterator iGeom= m_WhereUsed.find(pGeom->GetID());
	
	if (iGeom == m_WhereUsed.end())
	{	// Ok, ID doesn't exist		
		// Add Geometry to where used hash table
		m_WhereUsed.insert(pGeom->GetID(), pGeom);		
		return true;
	}
	else
	{	// KO, ID exist
		qDebug("GLC_Material::AddGLC_Geom : Geometry not added");
		return false;
	}
	
}
// Supprime une g�om�trie de la collection
bool GLC_Material::DelGLC_Geom(GLC_uint Key)
{
	CWhereUsed::iterator iGeom= m_WhereUsed.find(Key);
	
	if (iGeom != m_WhereUsed.end())
	{	// Ok, ID exist		
		m_WhereUsed.remove(Key);	// Remove container
		
		return true;
	}
	else
	{	// KO doesn't exist
		qDebug("GLC_Material::DelGLC_Geom : Geometry not remove");
		return false;
	}
	
}

//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////

// Execute OpenGL Material
bool GLC_Material::GlExecute(GLenum Mode)
{
	if (m_pTexture != NULL)
	{
		// for blend managing
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		m_pTexture->GlBindTexture();
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_AmbientColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_DiffuseColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_SpecularColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_LightEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &m_fShininess);

	// OpenGL Error handler
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Material::GlExecute OpenGL Error %s", errString);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////
// Private servicies Functions
//////////////////////////////////////////////////////////////////////

// Update geometries which used material
void GLC_Material::UpdateUsed(void)
{
	CGeomMap::iterator iEntry= m_WhereUsed.begin();
	
    while (iEntry != m_WhereUsed.constEnd())
    {
        // Indique aux g�om�trie utilisant la mati�re que celle ci � chang�e
        iEntry.value()->SetMaterial(this);
        ++iEntry;
    }
	

}

