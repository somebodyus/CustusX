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

#include "catch.hpp"
#include "cxImageAlgorithms.h"

#include <vtkImageData.h>
#include "cxImage.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxDataLocations.h"
#include "cxtestVisServices.h"
#include "cxPatientModelService.h"

using namespace cx;

TEST_CASE("ImageAlgorithms: resample() works", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	QString fname0 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/mra.mhd";
	QString fname1 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/US_01_20110222T110117_1.mhd";

	pasm->importData(fname0, fname0);
	pasm->importData(fname1, fname1);
	cx::ImagePtr image = pasm->getData<cx::Image>(fname0);
	cx::ImagePtr referenceImage = pasm->getData<cx::Image>(fname1);
	//	std::cout << "referenceImage base: " << referenceImage->getBaseVtkImageData() << std::endl;
	REQUIRE(image);
	REQUIRE(referenceImage);

	cx::Transform3D refMi = referenceImage->get_rMd().inv() * image->get_rMd();

	cx::ImagePtr oriented = resampleImage(pasm, image, refMi);
	REQUIRE(oriented);
	int inMin = image->getBaseVtkImageData()->GetScalarRange()[0];
	int inMax = image->getBaseVtkImageData()->GetScalarRange()[1];
	int outMin = oriented->getBaseVtkImageData()->GetScalarRange()[0];
	int outMax = oriented->getBaseVtkImageData()->GetScalarRange()[1];
	CHECK(inMin == outMin);
	//    std::cout << "inMax: " << inMax << " outMax: " << outMax << std::endl;
	CHECK(inMax == outMax);
	CHECK(image->getBaseVtkImageData() != oriented->getBaseVtkImageData());
//	CHECK(image->getTransferFunctions3D()->getVtkImageData() == image->getBaseVtkImageData());
	//  std::cout << "image:    " << image->getBaseVtkImageData() << " oriented:    " << oriented->getBaseVtkImageData() << std::endl;
	//  std::cout << "image tf: " << image->getTransferFunctions3D()->getVtkImageData() << " oriented tf: " << oriented->getTransferFunctions3D()->getVtkImageData() << std::endl;
	//Make sure the image and tf points to the same vtkImageData
//	CHECK(oriented->getTransferFunctions3D()->getVtkImageData() == oriented->getBaseVtkImageData());
//	CHECK(oriented->getLookupTable2D()->getVtkImageData() == oriented->getBaseVtkImageData());

	cx::Transform3D orient_M_ref = oriented->get_rMd().inv() * referenceImage->get_rMd();
	cx::DoubleBoundingBox3D bb_crop = cx::transform(orient_M_ref, referenceImage->boundingBox());

	// increase bb size by margin
	double margin = 5.0;
	bb_crop[0] -= margin;
	bb_crop[1] += margin;
	bb_crop[2] -= margin;
	bb_crop[3] += margin;
	bb_crop[4] -= margin;
	bb_crop[5] += margin;

	oriented->setCroppingBox(bb_crop);

	cx::ImagePtr cropped = cropImage(pasm, oriented);
	REQUIRE(cropped);
	int cropMin = cropped->getBaseVtkImageData()->GetScalarRange()[0];
	int cropMax = cropped->getBaseVtkImageData()->GetScalarRange()[1];
	CHECK(cropMin == inMin);
	CHECK(cropMax >  inMin);
	CHECK(cropMax <= inMax);
	CHECK(oriented->getBaseVtkImageData() != cropped->getBaseVtkImageData());
//	CHECK(cropped->getTransferFunctions3D()->getVtkImageData() == cropped->getBaseVtkImageData());
//	CHECK(cropped->getLookupTable2D()->getVtkImageData() == cropped->getBaseVtkImageData());

	QString uid = image->getUid() + "_resample%1";
	QString name = image->getName() + " resample%1";

	cx::ImagePtr resampled = cx::resampleImage(pasm, cropped, cx::Vector3D(referenceImage->getBaseVtkImageData()->GetSpacing()), uid, name);
	REQUIRE(resampled);
	outMin = resampled->getBaseVtkImageData()->GetScalarRange()[0];
	outMax = resampled->getBaseVtkImageData()->GetScalarRange()[1];
	//  std::cout << "outMin: " << outMin << " cropMin: " << cropMin << std::endl;
	//  CHECK(outMin == cropMin); //What happens when min == 1 instead of 0? //Looks ok in ImageTFData::fixTransferFunctions()
	CHECK(outMin >=  cropMin);
	CHECK(outMax >  cropMin);
	CHECK(outMax <= cropMax);
	CHECK(outMax <= cropMax);
	CHECK(cropped->getBaseVtkImageData() != resampled->getBaseVtkImageData());
//	CHECK(resampled->getTransferFunctions3D()->getVtkImageData() == resampled->getBaseVtkImageData());
//	CHECK(resampled->getLookupTable2D()->getVtkImageData() == resampled->getBaseVtkImageData());

}

