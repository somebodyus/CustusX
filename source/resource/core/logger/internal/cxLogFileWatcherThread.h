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

#ifndef CXLOGFILEWATCHERTHREAD_H
#define CXLOGFILEWATCHERTHREAD_H

#include "cxResourceExport.h"

//#define SSC_PRINT_CALLER_INFO

#include <QMetaType>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "cxDefinitions.h"
#include "cxAudio.h"
#include <sstream>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxLogMessage.h"
#include <QList>
#include <QThread>
#include <QFileSystemWatcher>
#include "cxLogThread.h"

class QString;
class QDomNode;
class QDomDocument;
class QFile;
class QTextStream;

/**
 * \file
 * \addtogroup cx_resource_core_logger
 * @{
 */

namespace cx
{
class LogFile;

/**\brief Thread for log handling. Used inside LogFileWatcher.
 *
 * \addtogroup cx_resource_core_logger
 */
class LogFileWatcherThread : public LogThread
{
	Q_OBJECT

public:
	LogFileWatcherThread(QObject* parent = NULL);
	virtual ~LogFileWatcherThread();

private slots:
	void onDirectoryChanged(const QString& path);
	void onFileChanged(const QString& path);
private:
	virtual void executeSetLoggingFolder(QString absoluteLoggingFolderPath);

	std::vector<Message> readMessages(const QString& path);

	QFileSystemWatcher mWatcher;
	QString mLogPath;
	QStringList mInitializedFiles;
	std::map<QString, LogFile> mFiles;
};

} //namespace cx

/**
 * @}
 */

#endif // CXLOGFILEWATCHERTHREAD_H
