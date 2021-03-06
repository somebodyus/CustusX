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
#ifndef CXSMOOTHINGIMAGEFILTER_H
#define CXSMOOTHINGIMAGEFILTER_H

#include "cxFilterImpl.h"

namespace cx
{

/** Filter for smoothing a volume.
 *
 *
 * \ingroup cx_resource_filter
 * \date Nov 26, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT SmoothingImageFilter : public FilterImpl
{
	Q_OBJECT

public:
	SmoothingImageFilter(VisServicesPtr services);
	virtual ~SmoothingImageFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoublePropertyPtr getSigma(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	vtkImageDataPtr mRawResult;
};
typedef boost::shared_ptr<class SmoothingImageFilter> SmoothingImageFilterPtr;


} // namespace cx



#endif // CXSMOOTHINGIMAGEFILTER_H
