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

#ifndef CXUSRECONSTRUCTIONSERVICEPROXY_H
#define CXUSRECONSTRUCTIONSERVICEPROXY_H

#include "org_custusx_usreconstruction_Export.h"

#include "cxUsReconstructionService.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/**
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionServiceProxy : public UsReconstructionService
{

	Q_OBJECT

public:
	UsReconstructionServiceProxy(ctkPluginContext *pluginContext);

	virtual void selectData(QString filename, QString calFilesPath = "");
	virtual void selectData(USReconstructInputData data);

	virtual QString getSelectedFilename() const;
	virtual USReconstructInputData getSelectedFileData();
//	virtual ReconstructParamsPtr getParams();
	virtual PropertyPtr getParam(QString uid);
	virtual std::vector<PropertyPtr> getAlgoOptions();
	virtual XmlOptionFile getSettings();
	virtual OutputVolumeParams getOutputVolumeParams() const;

	virtual void setOutputVolumeParams(const OutputVolumeParams& par);
//	virtual void setOutputRelativePath(QString path);
//	virtual void setOutputBasePath(QString path);
	virtual void startReconstruction();
	virtual std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction();
	virtual ReconstructionMethodService* createAlgorithm();

	virtual ReconstructCore::InputParams createCoreParameters();

	virtual bool isNull();

public slots:
	virtual void newDataOnDisk(QString mhdFilename);
private:
	void initServiceListener();
	void onServiceAdded(UsReconstructionService* service);
	void onServiceRemoved(UsReconstructionService *service);

	ctkPluginContext *mPluginContext;
	UsReconstructionServicePtr mUsReconstructionService;
	boost::shared_ptr<ServiceTrackerListener<UsReconstructionService> > mServiceListener;
};

} //cx

#endif // CXUSRECONSTRUCTIONSERVICEPROXY_H
