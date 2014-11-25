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

#include "cxFastPatientRegistrationWidget.h"

#include "cxReporter.h"
#include "cxRegistrationService.h"

namespace cx
{
FastPatientRegistrationWidget::FastPatientRegistrationWidget(RegServices services, QWidget* parent) :
		LandmarkPatientRegistrationWidget(services, parent, "FastPatientRegistrationWidget", "Fast Patient Registration")
{
}

FastPatientRegistrationWidget::~FastPatientRegistrationWidget()
{}

QString FastPatientRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Fast translation registration. </h3>"
      "<p>Select landmarks on the patient that corresponds to one or more of the points sampled in image registration. "
      "Points are used to determine the translation of the patient registration.</p>"
      "<p><i>Point on the patient using a tool and click the Sample button.</i></p>"
      "</html>";
}

void FastPatientRegistrationWidget::performRegistration()
{
	mServices.registrationService->doFastRegistration_Translation();
	this->updateAvarageAccuracyLabel();
}

}//namespace cx