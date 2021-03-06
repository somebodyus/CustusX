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

#ifndef CXREGISTRATIONMETHODSERVICE_H
#define CXREGISTRATIONMETHODSERVICE_H

#include <QObject>

#include <boost/shared_ptr.hpp>
#include "cxRegServices.h"
#include "org_custusx_registration_Export.h"

#define RegistrationMethodService_iid "cx::RegistrationMethodService"

namespace cx
{


/** \brief Registration Method services
 *
 * This class defines the common interface towards the registration methods plugins.
 *
 *  \ingroup cx_resource_core_registration
 *  \date 2014-09-02
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationMethodService : public QObject
{
	Q_OBJECT
public:
	RegistrationMethodService(RegServicesPtr services) :
	mServices(services) {}
    virtual ~RegistrationMethodService() {}

	virtual QWidget* createWidget() = 0;
	virtual QString getWidgetName() = 0;
	virtual QString getRegistrationType() = 0;
	virtual QString getRegistrationMethod() = 0;

protected:
	RegServicesPtr mServices;
};

} //namespace cx
Q_DECLARE_INTERFACE(cx::RegistrationMethodService, RegistrationMethodService_iid)


#endif // CXREGISTRATIONMETHODSERVICE_H
