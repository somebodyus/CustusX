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
#ifndef CXNETWORKDATATRANSFERWIDGET_H_
#define CXNETWORKDATATRANSFERWIDGET_H_

#include <QThread>
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"

class ctkPluginContext;

namespace cx {

class NetworkConnectionWidget;
class NetworkConnection;
typedef boost::shared_ptr<class NetworkConnectionHandle> NetworkConnectionHandlePtr;
typedef boost::shared_ptr<class NetworkDataTransfer> NetworkDataTransferPtr;
typedef boost::shared_ptr<class BoolProperty> BoolPropertyPtr;
typedef boost::shared_ptr<class StringProperty> StringPropertyPtr;
typedef boost::shared_ptr<class StringPropertySelectData> StringPropertySelectDataPtr;

/**
 * Widget for handling data transfer to/from an OpenIGTLink server.
 */
class NetworkDataTransferWidget : public BaseWidget
{
	Q_OBJECT
public:
	NetworkDataTransferWidget(NetworkDataTransferPtr backend, QWidget* parent=NULL);
	~NetworkDataTransferWidget();
private:
	NetworkDataTransferPtr mDataTransfer;
	NetworkConnectionWidget* mConnectionWidget;

	QVBoxLayout* createVBoxInGroupBox(QVBoxLayout* parent, QString header);
};

} // namespace cx

#endif // CXNETWORKDATATRANSFERWIDGET_H_
