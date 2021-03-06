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

#ifndef CXTOOLMANAGERWIDGET_H_
#define CXTOOLMANAGERWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxLegacySingletons.h"

class QPushButton;

namespace cx
{

/**
 * \class ToolManagerWidget
 *
 * \brief Designed as a debugging widget for the cxToolManager
 * \ingroup cx_gui
 *
 * \date May 25, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT ToolManagerWidget : public BaseWidget
{
  Q_OBJECT

public:
  ToolManagerWidget(QWidget* parent = NULL);
  virtual ~ToolManagerWidget();

private slots:
  void configureClickedSlot(bool);
  void deconfigureClickedSlot(bool);
  void initializeClickedSlot(bool);
  void uninitializeClickedSlot(bool);
  void startTrackingClickedSlot(bool);
  void stopTrackingClickedSlot(bool);

  void updateButtonStatusSlot(); ///< makes sure that the buttons represent the status of the toolmanager

private:
  QPushButton* mConfigureButton;
  QPushButton* mDeConfigureButton;
  QPushButton* mInitializeButton;
  QPushButton* mUnInitializeButton;
  QPushButton* mStartTrackingButton;
  QPushButton* mStopTrackingButton;
};

}

#endif /* CXTOOLMANAGERWIDGET_H_ */
