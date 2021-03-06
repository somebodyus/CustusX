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
#ifndef CXOPENIGTLINKRTSOURCE_H_
#define CXOPENIGTLINKRTSOURCE_H_

#include "org_custusx_core_video_Export.h"

#include <vector>
#include <map>
#include <boost/array.hpp>
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;
typedef boost::shared_ptr<QThread> QThreadPtr;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_video
 * @{
 */

typedef boost::shared_ptr<class ProbeDefinition> ProbeDefinitionPtr;
typedef boost::shared_ptr<class StreamerService> StreamerServicePtr;
typedef boost::shared_ptr<class ImageReceiverThread> ImageReceiverThreadPtr;
typedef boost::shared_ptr<class BasicVideoSource> BasicVideoSourcePtr;
typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;

/** \brief Represent one video grabber connection.
 *
 * Connect to a video/scanner interface and receive
 * all video and probe data from it. Convert to video
 * streams, set all data in probe if available.
 * Video Streams are also available directly from this
 * object.
 *
 * Refactored from old class OpenIGTLinkRTSource.
 *
 *  \ingroup org_custusx_core_video
 *  \date Oct 31, 2010
 *  \date Feb 26, 2013
 *  \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT VideoConnection : public QObject
{
	Q_OBJECT

public:
	explicit VideoConnection(VideoServiceBackendPtr backend);
	virtual ~VideoConnection();
	virtual bool isConnected() const;
	void disconnectServer();
    void runDirectLinkClient(StreamerServicePtr service);
	std::vector<VideoSourcePtr> getVideoSources();

signals:
	bool connected(bool);
	void fps(QString source, int fps);
	void videoSourcesChanged();

private slots:
	void onConnected();
	void onDisconnected();
	void imageReceivedSlot();
	void statusReceivedSlot();
	void fpsSlot(QString, double fps);
	void connectVideoToProbe();
	void useUnusedProbeDefinitionSlot();///< If no probe is available the ProbeDefinition is saved and this slot is called when a probe becomes available

private:
	void waitForClientFinished();
	StreamerServicePtr getStreamerInterface();
	void updateImage(ImagePtr message); // called by receiving thread when new data arrives.
	void stopClient(); ///< Get rid of the mClient thread.
	void resetProbe();///< Tell probe it is no longer connected to a digital interface
	void updateStatus(ProbeDefinitionPtr message);
	void startAllSources();
	void stopAllSources();
	void removeSourceFromProbe(ToolPtr tool);

	QPointer<ImageReceiverThread> mClient;
	QPointer<QThread> mThread;

	double mFPS;
	std::vector<ProbeDefinitionPtr> mUnusedProbeDefinitionVector;
	std::vector<BasicVideoSourcePtr> mSources;
	VideoServiceBackendPtr mBackend;
	StreamerServicePtr mStreamerInterface;
};
typedef boost::shared_ptr<VideoConnection> VideoConnectionPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXOPENIGTLINKRTSOURCE_H_ */
