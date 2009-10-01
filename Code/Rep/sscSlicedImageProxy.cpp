#include "sscSlicedImageProxy.h"

#include <boost/lexical_cast.hpp>

#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>

#include <vtkImageAlgorithm.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

#include "sscView.h"
#include "sscDataManager.h"
#include "sscSliceProxy.h"
#include "sscImageLUT2D.h"

namespace ssc
{


SlicedImageProxy::SlicedImageProxy()
{
	mMatrixAxes = vtkMatrix4x4Ptr::New();

	mReslicer = vtkImageReslicePtr::New();
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->SetOutputDimensionality( 2);
	mReslicer->SetResliceAxes(mMatrixAxes);
	//mReslicer->SetAutoCropOutput(false); //faster update rate
	mReslicer->AutoCropOutputOn(); // fix used in 2.0.9, but slower update rate
	
	mWindowLevel = vtkImageMapToColorsPtr::New();
	mWindowLevel->SetInputConnection( mReslicer->GetOutputPort() );
	mWindowLevel->SetOutputFormatToRGBA();
	
 	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.
}

SlicedImageProxy::~SlicedImageProxy()
{
}

void SlicedImageProxy::setSliceProxy(SliceProxyPtr slicer)
{
	if (mSlicer)
	{
		disconnect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}	
	mSlicer = slicer;
	if (mSlicer)
	{
		connect(mSlicer.get(), SIGNAL(transformChanged(Transform3D)), this, SLOT(sliceTransformChangedSlot(Transform3D)));
	}
}

typedef vtkSmartPointer<vtkImageExtractComponents> vtkImageExtractComponentsPtr;
typedef vtkSmartPointer<vtkImageAppendComponents > vtkImageAppendComponentsPtr;

void SlicedImageProxy::setImage(ImagePtr image)
{
	mImage = image;
	if (mImage)
	{
		mReslicer->SetInput( mImage->getBaseVtkImageData() );
		mWindowLevel->SetLookupTable(image->getLookupTable2D()->getOutputLookupTable());
		mWindowLevel->Update();
		
		if (mImage->getBaseVtkImageData()->GetNumberOfScalarComponents()==3) // color
		{
			// split the image into the components, apply the lut, then merge.

			vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
			
			for (int i=0; i<3; ++i)
			{			
				
				vtkImageMapToColorsPtr windowLevel = vtkImageMapToColorsPtr::New();
				windowLevel->SetInput(mReslicer->GetOutput());
				windowLevel->SetActiveComponent(i);
				windowLevel->SetLookupTable(image->getLookupTable2D()->getOutputLookupTable());
//				if (i=2) //TODO the only thing missing here is the alpha channel. Should be able to pass that on from the last pipe.
//				{
//					windowLevel->SetOutputFormatToLuminanceAlpha();	
//				}
//				else
				{
					windowLevel->SetOutputFormatToLuminance();					
				}

				merger->SetInput(i, windowLevel->GetOutput());
			}
			
			mRedirecter->SetInput(merger->GetOutput());
		 	//mRedirecter->SetInput(mReslicer->GetOutput());			
		}
		else // grayscale
		{
		 	mRedirecter->SetInput(mWindowLevel->GetOutput());						
		}
	}
}

ImagePtr SlicedImageProxy::getImage()const
{
	return mImage;
}

//deliver the sliced image..
vtkImageDataPtr SlicedImageProxy::getOutput()
{
	return mRedirecter->GetOutput();
}

void SlicedImageProxy::update()
{
	if (!mSlicer || !mImage)
		return;

	Transform3D rMs = mSlicer->get_sMr().inv();
	Transform3D iMr = mImage->get_rMd().inv();
	Transform3D M = iMr*rMs;

	mMatrixAxes->DeepCopy(M.matrix());
}

void SlicedImageProxy::sliceTransformChangedSlot(Transform3D sMr)
{
	update();
}

void SlicedImageProxy::printSelf(std::ostream & os, Indent indent)
{
	//os << indent << "PlaneType: " << mType << std::endl;
	os << indent << "mImage: " << (mImage ? mImage->getUid() : "NULL") << std::endl;
	os << indent << "mSlicer: " << (mSlicer ? mSlicer.get() : 0) << std::endl;
	if (mSlicer)
	{
		mSlicer->printSelf(os, indent.stepDown());
	}
	os << indent << "mReslicer->GetOutput(): " << mReslicer->GetOutput() << std::endl;
	os << indent << "mReslicer->GetInput() : " << mReslicer->GetInput() << std::endl;
	Transform3D test(mReslicer->GetResliceAxes());
	os << indent << "resliceaxes: " << std::endl;
	test.put(os, indent.getIndent()+3);
	os << std::endl;
	//os << indent << "rMs_debug: " << std::endl;
	//rMs_debug.put(os, indent.getIndent()+3);

}


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------




