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

#include "cxPreferenceTab.h"

#include <cmath>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include "cxDoubleWidgets.h"
#include "cxHelperWidgets.h"
#include "cxSettings.h"
#include "sscConfig.h"
#include "cxImage.h"

namespace cx
{
//==============================================================================
// PreferencesTab
//------------------------------------------------------------------------------

PreferenceTab::PreferenceTab(QWidget *parent) :
    QWidget(parent)
{
	this->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text
  mTopLayout = new QVBoxLayout;

  QVBoxLayout* vtopLayout = new QVBoxLayout;
  vtopLayout->addLayout(mTopLayout);
  vtopLayout->setMargin(0);
  vtopLayout->addStretch();
  this->setLayout(vtopLayout);
}

//==============================================================================
// PerformanceTab
//------------------------------------------------------------------------------
PerformanceTab::PerformanceTab(QWidget *parent) :
		PreferenceTab(parent)
{
	this->setObjectName("preferences_performance_widget");
	mRenderingIntervalSpinBox = NULL;
	mRenderingRateLabel = NULL;
	mSmartRenderCheckBox = NULL;
	mGPU2DRenderCheckBox = NULL;
    mOptimizedViewsCheckBox = NULL;
	mShadingCheckBox = NULL;
	mMainLayout = NULL;
}

void PerformanceTab::init()
{
  int renderingInterval = settings()->value("renderingInterval").toInt();

  QLabel* renderingIntervalLabel = new QLabel(tr("Rendering interval"));

  mRenderingIntervalSpinBox = new QSpinBox;
  mRenderingIntervalSpinBox->setSuffix("ms");
  mRenderingIntervalSpinBox->setMinimum(4);
  mRenderingIntervalSpinBox->setMaximum(1000);
  mRenderingIntervalSpinBox->setValue(renderingInterval);
  connect(mRenderingIntervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(renderingIntervalSlot(int)));

  mRenderingRateLabel = new QLabel("");
  this->renderingIntervalSlot(renderingInterval);

  double Mb = pow(10.0,6);
  bool ok = true;
  double maxRenderSize = settings()->value("View3D/maxRenderSize").toDouble(&ok);
  if (!ok)
    maxRenderSize = 10 * Mb;
  mMaxRenderSize = DoubleProperty::initialize("MaxRenderSize", "Max Render Size (Mb)", "Maximum size of volumes used in volume rendering. Applies to new volumes.", maxRenderSize, DoubleRange(1*Mb,300*Mb,1*Mb), 0, QDomNode());
  mMaxRenderSize->setInternal2Display(1.0/Mb);

  double stillUpdateRate = settings()->value("stillUpdateRate").value<double>();
	mStillUpdateRate = DoubleProperty::initialize("StillUpdateRate", "Still Update Rate",
																											"<p>Still Update Rate in vtkRenderWindow. "
																											"Increasing the value may improve rendering speed "
																											"at the cost of render quality.</p> "
																											"Generally raycast rendering requires this to be low (0.001), "
																											"while texture based rendering requires it to be high (5-10)."
																											"<p>Restart needed.</p>",
																											stillUpdateRate, DoubleRange(0.0001, 20, 0.0001), 4, QDomNode());

  mSmartRenderCheckBox = new QCheckBox("Smart Render");
  mSmartRenderCheckBox->setChecked(settings()->value("smartRender", true).toBool());
  mSmartRenderCheckBox->setToolTip("Render only when scene has changed, plus once per second.");

  m3DVisualizer = StringProperty::initialize("ImageRender3DVisualizer",
	  "3D Renderer",
	  "Select 3D visualization method for images",
	  settings()->value("View3D/ImageRender3DVisualizer").toString(),
	  this->getAvailableVisualizers(),
	  QDomNode());
  m3DVisualizer->setDisplayNames(this->getAvailableVisualizerDisplayNames());

  bool useGPU2DRender = settings()->value("useGPU2DRendering").toBool();
  mGPU2DRenderCheckBox = new QCheckBox("2D Overlay");
  mGPU2DRenderCheckBox->setChecked(useGPU2DRender);
  mGPU2DRenderCheckBox->setToolTip("<p>Use a GPU-based 2D renderer instead of "
								   "the software-based one, if available.</p>"
								   "<p>This enables multiple volume rendering in 2D.<p>");

  bool optimizedViews = settings()->value("optimizedViews").toBool();
  mOptimizedViewsCheckBox = new QCheckBox("Optimized Views");
  mOptimizedViewsCheckBox->setChecked(optimizedViews);
  mOptimizedViewsCheckBox->setToolTip("<p>Merge all non-3D views into a single vtkRenderWindow</p>"
									  "<p>This speeds up render on some platforms, still experimental.<p>");

  bool useGPU3DDepthPeeling = settings()->value("View3D/depthPeeling").toBool();
  mGPU3DDepthPeelingCheckBox = new QCheckBox("Use GPU 3D depth peeling");
  mGPU3DDepthPeelingCheckBox->setChecked(useGPU3DDepthPeeling);
  mGPU3DDepthPeelingCheckBox->setToolTip("Use a GPU-based 3D depth peeling to correctly visualize translucent surfaces.");

  //Layout
  mMainLayout = new QGridLayout;
  mMainLayout->addWidget(renderingIntervalLabel, 0, 0);
  new SpinBoxGroupWidget(this, mMaxRenderSize, mMainLayout, 1);
  mMainLayout->addWidget(mRenderingIntervalSpinBox, 0, 1);
  mMainLayout->addWidget(mRenderingRateLabel, 0, 2);
  mMainLayout->addWidget(mSmartRenderCheckBox, 2, 0);
  mMainLayout->addWidget(mGPU2DRenderCheckBox, 5, 0);
  mMainLayout->addWidget(mOptimizedViewsCheckBox, 6, 0);
  mMainLayout->addWidget(mGPU3DDepthPeelingCheckBox, 7, 0);
  new SpinBoxGroupWidget(this, mStillUpdateRate, mMainLayout, 8);
  mMainLayout->addWidget(sscCreateDataWidget(this, m3DVisualizer), 9, 0, 1, 2);

  mMainLayout->setColumnStretch(0, 2);
  mMainLayout->setColumnStretch(1, 2);
  mMainLayout->setColumnStretch(2, 1);

  mTopLayout->addLayout(mMainLayout);
}

static QStringList getAvailableVisualizers();
static std::map<QString, QString> getAvailableVisualizerDisplayNames();

void PerformanceTab::renderingIntervalSlot(int interval)
{
  mRenderingRateLabel->setText(QString("%1 fps").arg(1000.0/interval, 0, 'f', 1));
}

QStringList PerformanceTab::getAvailableVisualizers()
{
	QStringList retval;
	retval << "vtkVolumeTextureMapper3D";
	retval << "vtkGPUVolumeRayCastMapper";
#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
	retval << "vtkOpenGLGPUMultiVolumeRayCastMapper";
#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME

	return retval;
}

std::map<QString, QString> PerformanceTab::getAvailableVisualizerDisplayNames()
{
	std::map<QString, QString> names;
	names["vtkVolumeTextureMapper3D"] = "Texture (single volume)";
	names["vtkGPUVolumeRayCastMapper"] = "Raycast GPU (single volume)";
	names["vtkOpenGLGPUMultiVolumeRayCastMapper"] = "Mehdi Raycast GPU (multi volume)";
	return names;
}

void PerformanceTab::saveParametersSlot()
{
  settings()->setValue("renderingInterval", mRenderingIntervalSpinBox->value());
  settings()->setValue("useGPU2DRendering", mGPU2DRenderCheckBox->isChecked());
  settings()->setValue("optimizedViews", mOptimizedViewsCheckBox->isChecked());

  settings()->setValue("View3D/maxRenderSize",     mMaxRenderSize->getValue());
  settings()->setValue("smartRender",       mSmartRenderCheckBox->isChecked());
  settings()->setValue("stillUpdateRate",   mStillUpdateRate->getValue());
  settings()->setValue("View3D/depthPeeling", mGPU3DDepthPeelingCheckBox->isChecked());
  settings()->setValue("View3D/ImageRender3DVisualizer",   m3DVisualizer->getValue());
}

} /* namespace cx */
