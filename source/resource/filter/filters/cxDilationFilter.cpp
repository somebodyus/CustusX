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

#include "cxDilationFilter.h"

#include "cxDoubleProperty.h"
#include "cxColorProperty.h"
#include "cxBoolProperty.h"
#include "cxStringProperty.h"
#include "cxSelectDataStringProperty.h"

#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include "cxAlgorithmHelpers.h"
#include <vtkImageCast.h>
#include "cxUtilHelpers.h"
#include "cxContourFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"


namespace cx {

DilationFilter::DilationFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString DilationFilter::getName() const
{
	return "Dilation";
}

QString DilationFilter::getType() const
{
	return "DilationFilter";
}

QString DilationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Dilation Filter.</h3>"
	        "<p>This filter dilates a binary volume with a given radius in mm.<p>"
	        "<p>The dilation is performed using a ball structuring element<p>"
	        "</html>";
}

DoublePropertyPtr DilationFilter::getDilationRadiusOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Dilation radius (mm)", "",
    "Set dilation radius in mm", 1, DoubleRange(1, 20, 1), 0,
                    root);
	retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	return retval;
}

BoolPropertyPtr DilationFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolPropertyPtr retval = BoolProperty::initialize("Generate Surface", "",
	                                                                        "Generate a surface of the output volume", true,
	                                                                            root);
	return retval;
}

ColorPropertyPtr DilationFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}


void DilationFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getDilationRadiusOption(mOptions));
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(mOptions));
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void DilationFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select segmentation input for dilation");
	mInputTypes.push_back(temp);
}

void DilationFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Dilated segmentation image");
	mOutputTypes.push_back(temp);

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from dilated segmentation image.");
	mOutputTypes.push_back(temp);
}

bool DilationFilter::preProcess() {

    return FilterImpl::preProcess();
}

bool DilationFilter::execute() {
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	double radius = this->getDilationRadiusOption(mCopiedOptions)->getValue();

	// Convert radius in mm to radius in voxels for the structuring element
	Eigen::Array3d spacing = input->getSpacing();
	itk::Size<3> radiusInVoxels;
	radiusInVoxels[0] = radius/spacing(0);
	radiusInVoxels[1] = radius/spacing(1);
	radiusInVoxels[2] = radius/spacing(2);

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	// Create structuring element
	typedef itk::BinaryBallStructuringElement<unsigned char,3> StructuringElementType;
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radiusInVoxels);
    structuringElement.CreateStructuringElement();

	// Dilation
	typedef itk::BinaryDilateImageFilter<itkImageType, itkImageType, StructuringElementType> dilateFilterType;
	dilateFilterType::Pointer dilationFilter = dilateFilterType::New();
	dilationFilter->SetInput(itkImage);
	dilationFilter->SetKernel(structuringElement);
	dilationFilter->SetDilateValue(1);
	dilationFilter->Update();
	itkImage = dilationFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());

	vtkImageCastPtr imageCast = vtkImageCastPtr::New();
	imageCast->SetInputData(rawResult);
	imageCast->SetOutputScalarTypeToUnsignedChar();
	imageCast->Update();
	rawResult = imageCast->GetOutput();

	// TODO: possible memory problem here - check debug mem system of itk/vtk

	mRawResult =  rawResult;

	BoolPropertyPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);
	if (generateSurface->getValue())
	{
        double threshold = 1;/// because the segmented image is 0..1
        mRawContour = ContourFilter::execute(mRawResult, threshold);
	}

    return true;
}

bool DilationFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ImagePtr output = createDerivedImage(mServices->patient(),
										 uid, name,
										 mRawResult, input);
	mRawResult = NULL;
	if (!output)
		return false;

	output->resetTransferFunctions();
	mServices->patient()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	if (mRawContour!=NULL)
	{
		ColorPropertyPtr colorOption = this->getColorOption(mOptions);
		MeshPtr contour = ContourFilter::postProcess(mServices->patient(), mRawContour, output, colorOption->getValue());
		mOutputTypes[1]->setValue(contour->getUid());
		mRawContour = vtkPolyDataPtr();
	}

    return true;
}



} // namespace cx
