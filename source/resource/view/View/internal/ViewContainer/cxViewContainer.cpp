/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxViewContainer.h"

#include <QResizeEvent>
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include <QGridLayout>
#include "cxViewUtilities.h"
#include "cxViewContainerItem.h"
#include "cxTypeConversions.h"
#include "cxGLHelpers.h"
#include "cxOSXHelper.h"

namespace cx
{

ViewContainer::ViewContainer(QWidget *parent, Qt::WindowFlags f) :
	QVTKWidget(parent, f),
	mMouseEventTarget(NULL),
	mRenderWindow(NULL)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequestedSlot(const QPoint &)));
	mMTimeHash = 0;
	mMouseEventTarget = NULL;
	this->setLayout(new QGridLayout);
	disableGLHiDPI(this->winId());
}

ViewContainer::~ViewContainer()
{
}

/**
  * Clears view container, deleting all layout objects
  */
void ViewContainer::clear()
{
	QLayoutItem *item;
	while ((item = getGridLayout()->takeAt(0)) != 0)
	{
		ViewItem* viewItem = dynamic_cast<ViewItem*>(item);
		delete viewItem;
	}
	view_utils::setStretchFactors(this->getGridLayout(), LayoutRegion(0, 0, 10, 10), 0);
	this->setModified();
	mMouseEventTarget = NULL;
}

/**
  * Return this widget's grid layout object
  */
QGridLayout* ViewContainer::getGridLayout()
{
    return dynamic_cast<QGridLayout*>(layout());
}

void ViewContainer::paintEvent(QPaintEvent* event)
{
	inherited_widget::paintEvent(event);
	this->setModified();
}

void ViewContainer::setModified()
{
	if (this->getGridLayout())
	{
		for (int i = 0; i < this->getGridLayout()->count(); ++i)
		{
			this->getViewItem(i)->getView()->setModified();
		}
	}
	mMTimeHash = 0;
}

ViewItem* ViewContainer::getViewItem(int index)
{
	return dynamic_cast<ViewItem*>(this->getGridLayout()->itemAt(index));
}

/**
  * Creates and adds a view to this container.
  * Returns a pointer to the created view item that the container owns.
  */
ViewItem *ViewContainer::addView(QString uid, LayoutRegion region, QString name)
{
	this->initializeRenderWindow();

	// Create a viewItem for this view
	ViewItem *item = new ViewItem(uid, name, this, mRenderWindow, QRect());
	if (getGridLayout())
		getGridLayout()->addItem(item,
								 region.pos.row, region.pos.col,
								 region.span.row, region.span.col);
	view_utils::setStretchFactors(this->getGridLayout(), region, 1);

	return item;
}

void ViewContainer::initializeRenderWindow()
{
	if (mRenderWindow)
		return;

	mRenderWindow = vtkRenderWindowPtr::New();
	this->SetRenderWindow(mRenderWindow);
	mRenderWindow->GetInteractor()->EnableRenderOff();

	this->addBackgroundRenderer();
}

void ViewContainer::addBackgroundRenderer()
{
	vtkRendererPtr renderer = vtkRendererPtr::New();
	mRenderWindow->AddRenderer(renderer);
	renderer->SetViewport(0,0,1,1);
	QColor background = palette().color(QPalette::Background);
	renderer->SetBackground(background.redF(), background.greenF(), background.blueF());
}

void ViewContainer::customContextMenuRequestedSlot(const QPoint& point)
{
	ViewItem* item = this->findViewItem(point);
	if (!item)
		return;

	QWidget* sender = dynamic_cast<QWidget*>(this->sender());
	QPoint pointGlobal = sender->mapToGlobal(point);

	item->customContextMenuRequestedGlobalSlot(pointGlobal);
}

void ViewContainer::mouseMoveEvent(QMouseEvent* event)
{
	inherited_widget::mouseMoveEvent(event);

	if (mMouseEventTarget)
	{
		QPoint p = this->convertToItemSpace(event->pos(), mMouseEventTarget);
		mMouseEventTarget->mouseMoveSlot(p.x(), p.y(), event->buttons());
	}
}

void ViewContainer::mousePressEvent(QMouseEvent* event)
{
	// special case for CustusX: when context menu is opened, mousereleaseevent is never called.
	// this sets the render interactor in a zoom state after each menu call. This hack prevents
	// the mouse press event in this case.
	// NOTE: this doesnt seem to be the case in this class - investigate
	if ((this->contextMenuPolicy() == Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
		return;

	inherited_widget::mousePressEvent(event);

	mMouseEventTarget = this->findViewItem(event->pos());
	if (mMouseEventTarget)
	{
		QPoint p = this->convertToItemSpace(event->pos(), mMouseEventTarget);
		mMouseEventTarget->mousePressSlot(p.x(), p.y(), event->buttons());
	}
}

QPoint ViewContainer::convertToItemSpace(const QPoint &pos, ViewItem* item) const
{
	QRect r = item->geometry();
	QPoint retval(pos.x() - r.left(), pos.y() - r.top());
	return retval;
}

ViewItem* ViewContainer::findViewItem(const QPoint &pos)
{
	for (int i = 0; getGridLayout() && i < getGridLayout()->count(); ++i)
	{
		ViewItem *item = this->getViewItem(i);
		QRect r = item->geometry();
		if (r.contains(pos))
			return item;
	}
	return NULL;
}

void ViewContainer::mouseReleaseEvent(QMouseEvent* event)
{
	inherited_widget::mouseReleaseEvent(event);

	if (mMouseEventTarget)
	{
		QPoint p = this->convertToItemSpace(event->pos(), mMouseEventTarget);
		mMouseEventTarget->mouseReleaseSlot(p.x(), p.y(), event->buttons());
		mMouseEventTarget = NULL;
	}
}

void ViewContainer::focusInEvent(QFocusEvent* event)
{
	inherited_widget::focusInEvent(event);
}

void ViewContainer::wheelEvent(QWheelEvent* event)
{
	inherited_widget::wheelEvent(event);

	ViewItem *item = this->findViewItem(event->pos());
	if (item)
	{
		QPoint p = this->convertToItemSpace(event->pos(), item);
		item->mouseWheelSlot(p.x(), p.y(),
							 event->delta(), event->orientation(), event->buttons());
	}
}

void ViewContainer::showEvent(QShowEvent* event)
{
	inherited_widget::showEvent(event);
}

void ViewContainer::renderAll()
{
	// First, calculate if anything has changed
	long hash = 0;
	for (int i = 0; getGridLayout() && i < getGridLayout()->count(); ++i)
	{
		ViewItem *item = this->getViewItem(i);
		hash += item->getView()->computeTotalMTime();
	}
	// Then, if anything has changed, render everything anew
	if (hash != mMTimeHash)
	{
		this->doRender();
		mMTimeHash = hash;

		QString msg("During rendering of ViewContainer");
		report_gl_error_text(cstring_cast(msg));
	}
}

void ViewContainer::doRender()
{
	this->getRenderWindow()->Render();
}

void ViewContainer::resizeEvent( QResizeEvent *event)
{
	inherited_widget::resizeEvent(event);
	this->setModified();
	this->getGridLayout()->update();
}


} /* namespace cx */
