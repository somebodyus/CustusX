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

#include "cxGraphicalAxes3D.h"

#include "boost/bind.hpp"
#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkAssembly.h>

#include "cxTypeConversions.h"
#include "cxView.h"
#include "cxGraphicalPrimitives.h"

namespace cx
{

GraphicalAxes3D::GraphicalAxes3D(vtkRendererPtr renderer) : m_vtkAxisLength(100)
{
    mViewportListener.reset(new ViewportListener);
    mViewportListener->setCallback(boost::bind(&GraphicalAxes3D::rescale, this));

    mActor = vtkAxesActorPtr::New();
	mActor->SetAxisLabels(false);
	mActor->SetTotalLength( m_vtkAxisLength, m_vtkAxisLength, m_vtkAxisLength );

    this->setAxisLength(0.2);

    this->setShowAxesLabels(true);
    setTransform(Transform3D::Identity());
    setFontSize(0.04);

    this->setRenderer(renderer);
}

void GraphicalAxes3D::setRenderer(vtkRendererPtr renderer)
{
    if (mRenderer)
    {
		mRenderer->RemoveActor(mActor);
        for (unsigned i=0; i<mCaption.size(); ++i)
            mRenderer->RemoveActor(mCaption[i]);
        mViewportListener->stopListen();
    }

    mRenderer = renderer;

    if (mRenderer)
    {
		mRenderer->AddActor(mActor);
        for (unsigned i=0; i<mCaption.size(); ++i)
            mRenderer->AddActor(mCaption[i]);
        mViewportListener->startListen(mRenderer);
        this->rescale();
    }
}

GraphicalAxes3D::~GraphicalAxes3D()
{
    this->setRenderer(NULL);
}

void GraphicalAxes3D::setVisible(bool on)
{
	mActor->SetVisibility(on);
	this->resetAxesLabels();
}

void GraphicalAxes3D::setShowAxesLabels(bool on)
{
	mShowLabels = on;
	this->resetAxesLabels();
}

void GraphicalAxes3D::resetAxesLabels()
{
	if (mRenderer)
	{
		for (unsigned i=0; i<mCaption.size(); ++i)
			mRenderer->RemoveActor(mCaption[i]);
	}
	mCaption.clear();
	mCaptionPos.clear();

	if (!mShowLabels)
		return;

	this->addCaption("x", Vector3D(1,0,0), Vector3D(1,0,0));
	this->addCaption("y", Vector3D(0,1,0), Vector3D(0,1,0));
	this->addCaption("z", Vector3D(0,0,1), Vector3D(0,0,1));

	if (mRenderer)
	{
		for (unsigned i=0; i<mCaption.size(); ++i)
			mRenderer->AddActor(mCaption[i]);
	}
}

void GraphicalAxes3D::setCaption(const QString& caption, const Vector3D& color)
{
    this->addCaption(caption, Vector3D(0,0,0), color);
}

/**set font size to a fraction of the normalized viewport.
 *
 */
void GraphicalAxes3D::setFontSize(double size)
{
    mFontSize = size;
	this->resetAxesLabels();
}

/**set axis length to a world length
 *
 */
void GraphicalAxes3D::setAxisLength(double length)
{
	mSize = length;
    this->rescale();
}

/**Set the position of the axis.
 *
 */
void GraphicalAxes3D::setTransform(Transform3D rMt)
{
	m_rMt = rMt;
	this->rescale();
}

void GraphicalAxes3D::rescale()
{
	if (!mViewportListener->isListening())
		return;

	double size = mViewportListener->getVpnZoom();
	double axisSize = mSize/size;
	double scale = axisSize / m_vtkAxisLength;

	// NOTE: vtkAxesActor dislikes small values for SetTotalLength, thus we
	// keep that value constant at m_vtkAxisLength and instead scale the transform.
	Transform3D rMq = m_rMt * createTransformScale(Vector3D(scale,scale,scale));

	mActor->SetUserMatrix(rMq.getVtkMatrix());

	for (unsigned i=0; i<mCaption.size(); ++i)
	{
		Vector3D pos = rMq.coord(axisSize*mCaptionPos[i]);
		mCaption[i]->SetAttachmentPoint(pos.begin());
	}
}

void GraphicalAxes3D::addCaption(const QString& label, Vector3D pos, Vector3D color)
{
    vtkCaptionActor2DPtr cap = vtkCaptionActor2DPtr::New();
    cap->SetCaption(cstring_cast(label));
    cap->GetCaptionTextProperty()->SetColor(color.begin());
    cap->LeaderOff();
    cap->BorderOff();
    cap->GetCaptionTextProperty()->ShadowOff();
	cap->SetHeight(mFontSize);
	cap->SetVisibility(mActor->GetVisibility());
	mCaption.push_back(cap);
    mCaptionPos.push_back(pos);	
}


} // namespace cx
