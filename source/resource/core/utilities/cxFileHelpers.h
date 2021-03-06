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
#ifndef CXFILEHELPERS_H
#define CXFILEHELPERS_H

#include "cxResourceExport.h"

#include <QString>
#include <QDir>

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_utilities
* @{
*/

/** Remove a nonempty folder recursively
 *
 * http://stackoverflow.com/questions/11050977/removing-a-non-empty-folder-in-qt
 *
 * \ingroup cxResourceUtilities
 * \date jan 26, 2013
 * \author christiana
 */
cxResource_EXPORT bool removeNonemptyDirRecursively(const QString& dirName);

/** https://qt.gitorious.org/qt-creator/qt-creator/source/1a37da73abb60ad06b7e33983ca51b266be5910e:src/app/main.cpp#L13-189
 *  taken from utils/fileutils.cpp. We can not use utils here since that depends app_version.h.
 */
cxResource_EXPORT bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);

cxResource_EXPORT QFileInfoList getDirs(QString path);
cxResource_EXPORT QStringList getAbsolutePathToFiles(QString path, QStringList nameFilters, bool includeSubDirs = false);
cxResource_EXPORT QStringList getAbsolutePathToXmlFiles(QString path, bool includeSubDirs = false);

/**
 * @\}
 */

} // namespace cx


#endif // CXFILEHELPERS_H
