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
#include "cxCustusProtocol.h"

#include "cxIGTLinkConversion.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionSonixCXLegacy.h"
#include "cxImage.h"
#include "cxLogger.h"
#include "cxStreamedTimestampSynchronizer.h"


namespace cx
{

QString CustusProtocol::getName() const
{
    return "Custus";
}

bool CustusProtocol::doCRC() const
{
    //in the old version of the custusx openigtlink server
    //crc checking is disabled (for images)
    return false;
}

void CustusProtocol::translate(const igtl::ImageMessage::Pointer body)
{
    IGTLinkConversion converter;
	IGTLinkConversionSonixCXLegacy cxconverter;
	ImagePtr imagePtr = cxconverter.decode(body);
	imagePtr = cxconverter.decode(imagePtr);

    if (mStreamSynchronizer)
        mStreamSynchronizer->syncToCurrentTime(imagePtr);

    emit image(imagePtr);

    if(mUnsentUSStatusMessage)
    {
        ProbeDefinitionPtr definition = converter.decode(mUnsentUSStatusMessage, body, ProbeDefinitionPtr());
		definition = cxconverter.decode(definition);
        mUnsentUSStatusMessage = IGTLinkUSStatusMessage::New();
        emit probedefinition(definition->getUid(), definition);
    }
}

void CustusProtocol::translate(const cx::IGTLinkUSStatusMessage::Pointer body)
{
    mUnsentUSStatusMessage = body;
}

}
