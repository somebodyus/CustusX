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
#ifndef CXRMPCWIDGET_H
#define CXRMPCWIDGET_H

#include <QPushButton>
#include <QDomElement>
#include "cxRegistrationBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxICPRegistrationBaseWidget.h"

namespace cx
{
class WidgetObscuredListener;
class ICPWidget;
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
//typedef boost::shared_ptr<class BronchoscopyRegistration> BronchoscopyRegistrationPtr;
typedef std::map<QString, ToolPtr> ToolMap;
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
typedef boost::shared_ptr<class SeansVesselReg> SeansVesselRegPtr;
typedef boost::shared_ptr<class MeshInView> MeshInViewPtr;
typedef boost::shared_ptr<class SpaceListener> SpaceListenerPtr;

/**
 *
 * Register a point cloud in patient space to a point cloud in reference space.
 *
 * The moving data are assumed to be in patient space, and are used to correct
 * the patient registration prMt. The moving data iself are also affected, in order
 * to keep the relation of moving relative to patient.
 *
 * \date 2015-09-16
 * \author Christian Askeland
 */
class RMPCWidget: public ICPRegistrationBaseWidget
{
	Q_OBJECT

public:
	RMPCWidget(RegServicesPtr services, QWidget *parent);
	virtual ~RMPCWidget() {}

protected:
	virtual void initializeRegistrator();
	virtual void inputChanged();
	virtual void applyRegistration(Transform3D delta);
	virtual void onShown();
	virtual void setup();

	virtual double getDefaultAutoLTS() const { return false; }

private:
	StringPropertyBasePtr mFixedImage;
	StringPropertyBasePtr mMovingImage;

	SpaceListenerPtr mSpaceListenerMoving;
	SpaceListenerPtr mSpaceListenerFixed;
};

} //namespace cx

#endif // CXRMPCWIDGET_H
