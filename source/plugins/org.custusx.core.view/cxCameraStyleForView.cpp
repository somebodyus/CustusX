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

#include "cxCameraStyleForView.h"

#include <vtkRenderer.h>
#include <vtkCamera.h>

#include "cxTrackingService.h"
#include "cxToolRep3D.h"
#include "cxView.h"
#include "boost/bind.hpp"
#include <vtkRenderWindow.h>
#include "vtkInteractorStyleUnicam.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "cxCoreServices.h"
#include "cxViewportListener.h"

#include "cxTool.h"
#include <vtkRenderWindowInteractor.h>
#include "cxPatientModelService.h"
#include "cxRepContainer.h"
#include "cxLogger.h"
#include "cxRegionOfInterestMetric.h"
#include "cxNavigationAlgorithms.h"

namespace cx
{

CameraStyleForView::CameraStyleForView(CoreServicesPtr backend) :
	mBlockCameraUpdate(false),
	mBackend(backend)
{
	mViewportListener.reset(new ViewportListener);
	mViewportListener->setCallback(boost::bind(&CameraStyleForView::viewportChangedSlot, this));

	mPreRenderListener.reset(new ViewportPreRenderListener);
	mPreRenderListener->setCallback(boost::bind(&CameraStyleForView::onPreRender, this));

	connect(mBackend->tracking().get(), &TrackingService::activeToolChanged,
			this, &CameraStyleForView::activeToolChangedSlot);
}

void CameraStyleForView::setView(ViewPtr widget)
{
	mViewportListener->stopListen();
	mPreRenderListener->stopListen();

	this->disconnectTool();
	mView = widget;
	this->connectTool();

	mViewportListener->startListen(this->getRenderer());
	mPreRenderListener->startListen(this->getRenderer());
}


ViewPtr CameraStyleForView::getView() const
{
	return mView;
}

void CameraStyleForView::viewportChangedSlot()
{
	if (mBlockCameraUpdate)
		return;
	this->setModified();
}

void CameraStyleForView::onPreRender()
{
	this->applyCameraStyle();
}

ToolRep3DPtr CameraStyleForView::getToolRep() const
{
	if (!this->getView())
		return ToolRep3DPtr();

	ToolRep3DPtr rep = RepContainer(this->getView()->getReps()).findFirst<ToolRep3D>(mFollowingTool);
	return rep;
}

vtkRendererPtr CameraStyleForView::getRenderer() const
{
	if (!this->getView())
		return vtkRendererPtr();
	return this->getView()->getRenderer();
}

vtkCameraPtr CameraStyleForView::getCamera() const
{
	if (!this->getRenderer())
		return vtkCameraPtr();
	return this->getRenderer()->GetActiveCamera();
}

void CameraStyleForView::setModified()
{
	mPreRenderListener->setModified();
}

void CameraStyleForView::applyCameraStyle()
{	        
	vtkCameraPtr camera = this->getCamera();
	if (!camera)
		return;

	double cameraOffset = camera->GetDistance();
	Vector3D pos_old(camera->GetPosition());
	Vector3D focus_old(camera->GetFocalPoint());
	Vector3D vup_old(camera->GetViewUp());

	Vector3D camera_r = pos_old;
	Vector3D focus_r = focus_old;
	Vector3D vup_r = vup_old;

	if (mFollowingTool)
	{
		Transform3D rMpr = mBackend->patient()->get_rMpr();
		Transform3D prMt = mFollowingTool->get_prMt();
		Transform3D rMt = rMpr * prMt;
		double offset = mFollowingTool->getTooltipOffset();

		// view up is relative to tool
		vup_r = rMt.vector(Vector3D(-1, 0, 0));

		if (mStyle.mFocusFollowTool)
		{
			// set focus to tool offset point
			focus_r = rMt.coord(Vector3D(0, 0, offset));
		}

		if (mStyle.mCameraFollowTool)
		{
			// set camera on the tool line, at a distance 'cameraOffset' from the focus.
			Vector3D tooloffset = rMt.coord(Vector3D(0, 0, offset));
			Vector3D e_tool = rMt.vector(Vector3D(0, 0, 1));
			camera_r = NavigationAlgorithms::findCameraPosOnLineFixedDistanceFromFocus(tooloffset,
																					   e_tool,
																					   cameraOffset,
																					   focus_r);
		}
	}

	if (mStyle.mTableLock)
	{
		Vector3D table_up = mBackend->patient()->getOperatingTable().getVectorUp();
		vup_r = table_up;
	}

	if (mStyle.mCameraFollowTool)
		camera_r = NavigationAlgorithms::elevateCamera(mStyle.mElevation, camera_r, focus_r, vup_r);

	// reset vup based on vpn
	if (!similar(vup_r, vup_old))
	{
		Vector3D vpn_r = (camera_r-focus_r).normal();
		vup_r = NavigationAlgorithms::orthogonalize_vup(vup_r, vpn_r, vup_old);
	}

	if (!mStyle.mAutoZoomROI.isEmpty())
	{
		DoubleBoundingBox3D roi_r = this->getROI();
		if (roi_r != DoubleBoundingBox3D::zero())
		{
			double viewAngle = camera->GetViewAngle()/180.0*M_PI;
			Vector3D vpn = (camera_r-focus_r).normal();

//			Transform3D rMpr = mBackend->patient()->get_rMpr();
//			Transform3D prMt = mBackend->tracking()->getActiveTool()->get_prMt();
//			Transform3D rMt = rMpr * prMt;
//			Vector3D tp = rMt.coord(Vector3D(0, 0, 0));

//			double t_dist = this->findCameraDistance(viewAngle, focus_r, vpn, tp);

			this->getRenderer()->ComputeAspect();
			double aspect[2];
			this->getRenderer()->GetAspect(aspect);

			double viewAngle_vertical = viewAngle;
			double viewAngle_horizontal = viewAngle*aspect[0];

			Vector3D camera_r_t = NavigationAlgorithms::findCameraPosByZoomingToROI(viewAngle_vertical,
																					viewAngle_horizontal,
																					focus_r,
																					vup_r,
																					vpn,
																					roi_r);


//					std::cout << "      roi_r: <" << roi_r << std::endl;
//					std::cout << "      camera_r: < " << camera_r << " >" << std::endl;
//					std::cout << "      vpn: < " << vpn << " >" << std::endl;
//					std::cout << "      focus_r: < " << focus_r << " >" << std::endl;
//					std::cout << "      dist: < " << dist << " >" << std::endl;
//					std::cout << "      t_dist: < " << dist << " >" << std::endl;
//					std::cout << "      camera_r_t: < " << camera_r_t << " >" << std::endl;
//					std::cout << "      "  << std::endl;
			camera_r = camera_r_t;
		}
	}

//	CX_LOG_CHANNEL_DEBUG("CA") << " pos " << Vector3D(camera->GetPosition());
//	CX_LOG_CHANNEL_DEBUG("CA") << " foc " << Vector3D(camera->GetFocalPoint());
//	CX_LOG_CHANNEL_DEBUG("CA") << " vup " << Vector3D(camera->GetViewUp());
//	CX_LOG_CHANNEL_DEBUG("CA") << " vpn " << Vector3D(camera->GetViewPlaneNormal());
//	CX_LOG_CHANNEL_DEBUG("CA") << "view angle " << camera->GetViewAngle();
//	CX_LOG_CHANNEL_DEBUG("CA") << "";

	if (similar(pos_old, camera_r, 0.1) && similar(focus_old, focus_r, 0.1) && similar(vup_old, vup_r,0.1 ))
		return; // break update loop: this event is triggered by camera change.

	mBlockCameraUpdate = true;
	camera->SetPosition(camera_r.begin());
	camera->SetFocalPoint(focus_r.begin());
	camera->SetViewUp(vup_r.begin());
	camera->SetClippingRange(1, std::max<double>(1000, cameraOffset * 10));
	if (mStyle.mCameraFollowTool && mFollowingTool)
		camera->SetClippingRange(1, std::max<double>(1000, cameraOffset * 1.5));
	mBlockCameraUpdate = false;
}

DoubleBoundingBox3D CameraStyleForView::getROI()
{
	DataPtr data = mBackend->patient()->getData(mStyle.mAutoZoomROI);
	RegionOfInterestMetricPtr roi = boost::dynamic_pointer_cast<RegionOfInterestMetric>(data);
	if (roi)
		return roi->getROI();
	return DoubleBoundingBox3D::zero();
}

void CameraStyleForView::activeToolChangedSlot()
{
	ToolPtr newTool = mBackend->tracking()->getActiveTool();
	if (newTool == mFollowingTool)
		return;

	this->disconnectTool();
	this->connectTool();
}

bool CameraStyleForView::isToolFollowingStyle() const
{
	return (mStyle.mCameraFollowTool || mStyle.mFocusFollowTool);
}

void CameraStyleForView::connectTool()
{
	if (!this->isToolFollowingStyle())
		return;

	mFollowingTool = mBackend->tracking()->getActiveTool();

	if (!mFollowingTool)
		return;

	if (!this->getView())
		return;

	connect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(setModified()));

	ToolRep3DPtr rep = this->getToolRep();
	if (rep)
	{
		rep->setOffsetPointVisibleAtZeroOffset(false);
		if (mStyle.mCameraFollowTool && fabs(mStyle.mElevation) < 0.01)
			rep->setStayHiddenAfterVisible(true);
	}

	this->setModified();

	report("Camera is following " + mFollowingTool->getName());
}

void CameraStyleForView::disconnectTool()
{
	if (mFollowingTool)
	{
		disconnect(mFollowingTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
			SLOT(setModified()));

		ToolRep3DPtr rep = this->getToolRep();
		if (rep)
		{
			rep->setOffsetPointVisibleAtZeroOffset(true);
			rep->setStayHiddenAfterVisible(false);
		}

		mFollowingTool.reset();
	}
}

void CameraStyleForView::setCameraStyle(CameraStyleData style)
{
	if (mStyle == style)
		return;

	this->disconnectTool();

	if (style.mUniCam)
		this->setInteractor(vtkInteractorStyleUnicamPtr::New());
	else
		this->setInteractor(vtkInteractorStyleTrackballCameraPtr::New());

	mStyle = style;

	this->connectTool();
}

void CameraStyleForView::setInteractor(vtkSmartPointer<vtkInteractorStyle> style)
{
	ViewPtr view = this->getView();
	if (!view)
		return;
	vtkRenderWindowInteractor* interactor = view->getRenderWindow()->GetInteractor();
	interactor->SetInteractorStyle(style);
}

CameraStyleData CameraStyleForView::getCameraStyle()
{
	return mStyle;
}

}//namespace cx
