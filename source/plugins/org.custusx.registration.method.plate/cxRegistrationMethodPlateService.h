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

#ifndef CXREGISTRATIONMETHODPLATESERVICE_H_
#define CXREGISTRATIONMETHODPLATESERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_plate_Export.h"

namespace cx
{

/**
 * Registration method: Plate image to patient service implementation
 *
 * \ingroup org_custusx_registration_method_Plate
 *
 * \date 2014-10-09
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_plate_EXPORT RegistrationMethodPlateImageToPatientService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodPlateImageToPatientService(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, VisualizationServicePtr visualizationService) :
		RegistrationMethodService(registrationService),
		mPatientModelService(patientModelService),
		mVisualizationService(visualizationService) {}
	virtual ~RegistrationMethodPlateImageToPatientService() {}
	virtual QString getRegistrationType() {return QString("ImageToPatient");}
	virtual QString getRegistrationMethod() {return QString("Plate Landmark");}
	virtual QString getWidgetName() {return QString("PlateImage2PatientRegistrationWidget");}
	virtual QWidget* createWidget();

private:
	PatientModelServicePtr mPatientModelService;
	VisualizationServicePtr mVisualizationService;
};

} /* namespace cx */

#endif /* CXREGISTRATIONMETHODPLATESERVICE_H_ */
