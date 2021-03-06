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
#ifndef CXMULTIFILEINPUTWIDGET_H_
#define CXMULTIFILEINPUTWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
class QLabel;
class QLineEdit;
class QTextEdit;
class QToolButton;
class QGridLayout;
class QAction;

namespace cx
{

/**
 *	Widget for displaying, editing and browsing for several files.
 *  \sa FileSelectWidget FileInputWidget
 *
 * \ingroup cx_resource_widgets
 * \date 2014-04-17
 * \author christiana
 */
class cxResourceWidgets_EXPORT MultiFileInputWidget : public QWidget
{
	Q_OBJECT
public:
	MultiFileInputWidget(QWidget* parent=0);

	void setDescription(QString text);
	void setFilenames(QStringList text);
//	void addFilename(QString text);
	void setHelp(QString text);
	void setBrowseHelp(QString text);
	QStringList getFilenames() const;
	QStringList getAbsoluteFilenames() const;
	void setBasePath(QString path);
	void setUseRelativePath(bool on);

signals:
	void fileChanged();

private slots:
	void browse();
	void updateColor();
	void evaluateTextChanges();
private:
	void updateHelpInternal();
	void widgetHasBeenChanged();
	QStringList convertToAbsoluteFilenames(QStringList text) const;
	QString convertToAbsoluteFilename(QString text) const;
	QString convertFilenamesToTextEditText(QStringList files) const;
	QString convertToPresentableFilename(QString text) const;

	QStringList mLastFilenames;
	QString mBasePath;
	bool mUseRelativePath;
	QString mBaseHelp;

	QLabel* mDescription;
	QTextEdit* mFilenameEdit;
	QToolButton* mBrowseButton;
	QGridLayout* mLayout;
	QAction* mBrowseAction;
};

} /* namespace cx */
#endif /* CXMULTIFILEINPUTWIDGET_H_ */
