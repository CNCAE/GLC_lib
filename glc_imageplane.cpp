/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.6, packaged on June, 2006.

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

//! \file glc_imagePlane.cpp implementation of the GLC_ImagePlane class.
#include <QtDebug>
#include "glc_imageplane.h"
#include "glc_viewport.h"
#include "glc_openglexception.h"

//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
GLC_ImagePlane::GLC_ImagePlane(GLC_Viewport* pViewport, const QColor& color)
:GLC_Geometry("Image Plane", color)
, m_pImgTexture(NULL)
, m_pViewport(pViewport)
, m_dLgImage(0)
, m_dZpos(0)
, m_PolyFace(GL_FRONT_AND_BACK)	// Default Faces style
, m_PolyMode(GL_FILL)			// Default polyganal mode

{
	updateZPosition();
}

GLC_ImagePlane::~GLC_ImagePlane(void)
{
	// Lib�ration de la m�moire de la texture
	if (m_pImgTexture != NULL)
	{
		delete m_pImgTexture;
		m_pImgTexture= NULL;
	}

}
/////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Load image
bool GLC_ImagePlane::loadImageFile(QGLWidget *GLWidget, const QString ImageName)
{
	if (m_pImgTexture != NULL)
	{
		delete m_pImgTexture;
	}

	m_pImgTexture= new GLC_Texture(GLWidget, ImageName);

	return (m_pImgTexture != NULL);
}

// Update image size
void GLC_ImagePlane::updatePlaneSize(void)
{

	// compute quad size
	int nCote;
	if (m_pViewport->getWinHSize() < m_pViewport->getWinVSize())
	{
		nCote= m_pViewport->getWinVSize();
	}
	else
	{
		nCote= m_pViewport->getWinHSize();
	}

	// Calcul du cot� du carr� de vision de la cam�ra
	// Le cot� du carr� de la cam�ra est mapp� sur la hauteur de la fen�tre
	const double ChampsVision = 2 * m_dZpos *  tan((m_pViewport->getFov() * PI / 180)/ 2);
	
	// Circle radius in openGL unit = RayonPixel * (dimens GL / dimens Pixel)
	m_dLgImage= ((double)nCote * ChampsVision / (double)m_pViewport->getWinVSize());

	// Invalidate OpenGL Display list
	m_ListIsValid= false;
}

// Update Plane Z position
void GLC_ImagePlane::updateZPosition(void)
{
	// Compute Plane Z position
	const double n= m_pViewport->getDistMin();
	const double f= m_pViewport->getDistMax();
	int nbrBits;
	
	//glGetIntegerv(GL_DEPTH_BITS, &nbrBits);
	// glGetIntegerv seems to not work
	// force to minimum Depth : 16 bits
	nbrBits= 16;
	
	double zw= pow(2, static_cast<double>(nbrBits)) - 2.0;
	
	m_dZpos= -f * n / (((zw - 1) / zw) * (f - n) - f);

	updatePlaneSize();
	// Invalidate OpenGL Display list
	m_ListIsValid= false;
	
}


//////////////////////////////////////////////////////////////////////
// OpenGL Functions
//////////////////////////////////////////////////////////////////////
// Plane Display
void GLC_ImagePlane::glDraw(void)
{
	const double LgImgSur2= m_dLgImage / 2;
	
	glLoadIdentity();
	glTranslated(0.0, 0.0, -m_dZpos);
	glBegin(GL_QUADS);

		glNormal3d(0.0, 0.0, 1.0);	// Z
		glTexCoord2f(0.0f, 0.0f); glVertex3d(-LgImgSur2, -LgImgSur2, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(LgImgSur2, -LgImgSur2, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(LgImgSur2, LgImgSur2, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(-LgImgSur2, LgImgSur2, 0.0);

	glEnd();
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_ImagePlane::GlDraw ", error);
		throw(OpenGlException);
	}
}
// Define Geometry property (Couleur, position, epaisseur)
void GLC_ImagePlane::glPropGeom(void)
{
	glDisable(GL_LIGHTING);

	if (m_pImgTexture != NULL)
	{		
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());			// is color
		m_pImgTexture->glcBindTexture();
	}
	else
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		//glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glColor4d(getdRed(), getdGreen(), getdBlue(), getdAlpha());			// is color
	}
	
	// Polygons display mode
	glPolygonMode(m_PolyFace, m_PolyMode);
	
	// OpenGL error handler
	GLenum error= glGetError();	
	if (error != GL_NO_ERROR)
	{
		GLC_OpenGlException OpenGlException("GLC_ImagePlane::GlPropGeom ", error);
		throw(OpenGlException);
	}

}


