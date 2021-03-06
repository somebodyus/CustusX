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

#ifndef CXFASTPATIENTREGISTRATIONWIDGET_H_
#define CXFASTPATIENTREGISTRATIONWIDGET_H_

#include "cxPatientLandMarksWidget.h"
#include "org_custusx_registration_method_landmark_Export.h"
#include <QCheckBox>

class QTableWidgetItem;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
 * @{
 */

/**
 * \class FastPatientRegistrationWidget
 *
 * \brief Widget for performing a fast and aprox landmark based patient registration,
 * using only the matrix translation part.
 *
 * \date 27. sep. 2010
 * \\author Janne Beate Bakeng
 */
class org_custusx_registration_method_landmark_EXPORT FastPatientRegistrationWidget : public PatientLandMarksWidget
{
	Q_OBJECT
public:
  FastPatientRegistrationWidget(RegServicesPtr services, QWidget* parent);
  virtual ~FastPatientRegistrationWidget();

protected:
  virtual void showEvent(QShowEvent *event);
  virtual void hideEvent(QHideEvent *event);

  QCheckBox* mMouseClickSample;

protected slots:
  void mouseClickSampleStateChanged();
  virtual void pointSampled(Vector3D p_r);
private:
  virtual void performRegistration();
  QTableWidgetItem * getLandmarkTableItem();
};

/**
 * @}
 */
}
#endif /* CXFASTPATIENTREGISTRATIONWIDGET_H_ */
