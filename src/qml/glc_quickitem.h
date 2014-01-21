/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/
//! \file glc_quickitem.h interface for the GLC_QuickItem class.

#ifndef GLC_QUICKITEM_H
#define GLC_QUICKITEM_H

#include <QQuickItem>
#include <QtOpenGL>
#include <QUrl>
#include <QOpenGLFramebufferObject>
#include <QPair>

#include "../viewport/glc_viewport.h"
#include "../shading/glc_light.h"
#include "../sceneGraph/glc_world.h"
#include "../viewport/glc_movercontroller.h"
#include "../viewport/glc_viewhandler.h"
#include "../maths/glc_vector3d.h"

#include "../glc_config.h"

class QSGSimpleTextureNode;
class QGLFramebufferObject;

//////////////////////////////////////////////////////////////////////
//! \class GLC_QuickItem
/*! \brief GLC_QuickItem : Provide a way to use GLC_lib into a QQuickItem*/

/*! The GLC_QuickItem make it possible to render a GLC_World int a QML scene Graph
*/

//////////////////////////////////////////////////////////////////////
class GLC_LIB_EXPORT GLC_QuickItem : public QQuickItem
{
    Q_OBJECT

    //! The world to render into this QQuickItem
    Q_PROPERTY(QVariant viewHandler READ viewHandler WRITE setViewhandler)

public:
    explicit GLC_QuickItem(GLC_QuickItem* pParent= NULL);
    virtual ~GLC_QuickItem();

    //! Return the used GLC_ViewHandler as a QVariant
    virtual QVariant viewHandler() const;

public slots:
    virtual void setViewhandler(QVariant viewHandler);
    void invalidateSelectionBuffer();

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode* updatePaintNode(QSGNode* pNode, UpdatePaintNodeData* pData);
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseMoveEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void wheelEvent(QWheelEvent * e);
    virtual void touchEvent(QTouchEvent * e);

protected:
    virtual void setOpenGLState();
    void render(QSGSimpleTextureNode* pTextureNode, UpdatePaintNodeData* pData);

    void renderForSelection();

    GLC_uint selectBody(GLC_uint instanceId, int x, int y);
    QPair<GLC_uint, GLC_uint> selectPrimitive(GLC_uint instanceId, int x, int y);

    virtual void doRender();
    virtual void do3DWidgetRender();
    void setupFbo(int width, int height, QSGSimpleTextureNode *pTextureNode);
    void setupAuxFbo(int width, int height);
    void pushOpenGLMatrix();
    void popOpenGLMatrix();

protected:
    GLC_ViewHandler* m_pViewhandler;
    QOpenGLFramebufferObject* m_pSourceFbo;
    QOpenGLFramebufferObject* m_pTargetFbo;
    QOpenGLFramebufferObject* m_pAuxFbo;
    bool m_SelectionBufferIsDirty;
    GLC_Point3d m_UnprojectedPoint;
};

#endif // GLC_QUICKITEM_H
