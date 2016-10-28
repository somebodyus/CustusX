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
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include "cxSharedOpenGLContext.h"
#include "vtkForwardDeclarations.h"
#include "cxImage.h"

namespace cxtest {

cx::SharedOpenGLContextPtr initSharedOpenGLContext(vtkRenderWindowPtr renderWindow)
{
	vtkOpenGLRenderWindowPtr opengl_renderwindow = vtkOpenGLRenderWindow::SafeDownCast(renderWindow.Get());
	REQUIRE(opengl_renderwindow);
	REQUIRE(cx::SharedOpenGLContext::isValid(opengl_renderwindow));
//	opengl_renderwindow.Get()->DebugOn();
	opengl_renderwindow->DebugOn();
	cx::SharedOpenGLContextPtr sharedOpenGLContext = cx::SharedOpenGLContextPtr(new cx::SharedOpenGLContext(opengl_renderwindow));
	return sharedOpenGLContext;
}

cx::ImagePtr createDummyImage(int number = 0)
{
	vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(100, 255);
	QString uid = QString("dummyImageUid%1").arg(number);
	QString name = QString("dummyImageName%1").arg(number);
	cx::ImagePtr image = cx::ImagePtr(new cx::Image(uid, dummyImageData, name));
	return image;
}

TEST_CASE("SharedOpenGLContext init", "[opengl][resource][visualization][unit]")
{
	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	REQUIRE(initSharedOpenGLContext(renderWindow));
}

//TEST_CASE("SharedOpenGLContext render", "[opengl][resource][visualization][unit]")
//{
//	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
//	REQUIRE(initSharedOpenGLContext(renderWindow));
//	renderWindow->Render();
//}

TEST_CASE("SharedOpenGLContext upload texture", "[opengl][resource][visualization][unit]")
{
	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	cx::SharedOpenGLContextPtr sharedOpenGLContext = initSharedOpenGLContext(renderWindow);

	cx::ImagePtr image0 = createDummyImage(0);
	cx::ImagePtr image1 = createDummyImage(1);

	renderWindow->Render();//Call render to create the openGL context
	//Crash in vtkOpenGLRenderWindow::OpenGLInitContext() on glewInit(), if we haven't called render
	REQUIRE(sharedOpenGLContext->upload3DTexture(image0));
	REQUIRE(sharedOpenGLContext->upload3DTexture(image1));

//	for (unsigned i = 0; i < 10; ++i)
//		renderWindow->Render();
}

}//cxtest