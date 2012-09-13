// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxStateService.h"

#include <iostream>
#include <QByteArray>
#include <QDir>
#include "sscEnumConverter.h"
#include "sscXmlOptionItem.h"
#include "sscMessageManager.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxWorkflowStateMachine.h"
#include "cxApplicationStateMachine.h"
#include "cxViewWrapper3D.h"
#include "cxDataLocations.h"

namespace cx
{
Desktop::Desktop()
{
}

Desktop::Desktop(QString layout, QByteArray mainwindowstate) :
				mLayoutUid(layout), mMainWindowState(mainwindowstate)
{
}

class ApplicationsParser
{
public:
	ApplicationsParser()
	{
		mXmlFile = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("applications");

		QString fullState =
						"AAAA/wAAAAD9AAAAAQAAAAAAAAGaAAACgPwCAAAACPsAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA/AAAAD8AAAKAAAACVQEAABv6AAAAAAEAAAAT+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAfQD////7AAAALgBDAG8AbgBzAG8AbABlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAABUAP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAR0A////+wAAADoAQwBhAG0AZQByAGEAQwBvAG4AdAByAG8AbABXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAbgD////7AAAALgBJAEcAVABMAGkAbgBrAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAEmAP////sAAAA4AFUAUwBBAGMAcQB1AHMAaQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAGAAP////sAAABCAFQAcgBhAGMAawBlAGQAQwBlAG4AdABlAHIAbABpAG4AZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAnAD////7AAAANABOAGEAdgBpAGcAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAACZAP////sAAAAyAEYAcgBhAG0AZQBUAHIAZQBlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAABmAP////sAAAA8AFQAbwBvAGwAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAABWwD////7AAAARgBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBIAGkAcwB0AG8AcgB5AFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAECAP////sAAABEAEMAYQBsAGkAYgByAGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAGaAP////sAAABIAFYAaQBzAHUAYQBsAGkAegBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAABOAD////7AAAARgBTAGUAZwBtAGUAbgB0AGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAE4AP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAS0A////+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAABNwD////7AAAAPABNAGUAcwBoAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAO0A////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAOwA////+wAAABQARABvAGMAawBXAGkAZABnAGUAdAEAAAAAAAABgQAAAQ8A/////AAAAygAAADLAAAAAAD////6AAAAAAEAAAAB+wAAAEAAVAByAGEAbgBzAGYAZQByAEYAdQBuAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAAAAAAAD7AAAAPgBTAGgAaQBmAHQAQwBvAHIAcgBlAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD7AAAAQgBJAG0AYQBnAGUAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAAEtAAAAAAAAAAA+wAAAEYAUABhAHQAaQBlAG4AdABSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD8AAACXQAAAUAAAAAAAP////oAAAAAAQAAAAH7AAAAFABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAAAAAAAAAAANgAAACgAAAAAQAAAAEAAAACAAAAAj8AAAABAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAIAAAAJAAAAHgBXAG8AcgBrAGYAbABvAHcAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAFgBEAGEAdABhAFQAbwBvAGwAQgBhAHIBAAAA5f////8AAAAAAAAAAAAAACIATgBhAHYAaQBnAGEAdABpAG8AbgBUAG8AbwBsAEIAYQByAQAAAYL/////AAAAAAAAAAAAAAAmAEMAYQBtAGUAcgBhADMARABWAGkAZQB3AFQAbwBvAGwAQgBhAHIBAAAB+/////8AAAAAAAAAAAAAABYAVABvAG8AbABUAG8AbwBsAEIAYQByAQAAAwT/////AAAAAAAAAAAAAAAiAFMAYwByAGUAZQBuAHMAaABvAHQAVABvAG8AbABCAGEAcgEAAANZ/////wAAAAAAAAAAAAAAHABEAGUAcwBrAHQAbwBwAFQAbwBvAGwAQgBhAHIBAAADiv////8AAAAAAAAAAAAAACwASQBuAHQAZQByAGEAYwB0AG8AcgBTAHQAeQBsAGUAVABvAG8AbABCAGEAcgEAAAPf/////wAAAAAAAAAAAAAAFgBIAGUAbABwAFQAbwBvAGwAQgBhAHIBAAAEfAAAAXgAAAAAAAAAAA==";

		//Lab application
		this->addDefaultDesktops(
						"PatientDataUid",
						"LAYOUT_3D_ACS",
						"AAAA/wAAAAD9AAAAAQAAAAAAAAFvAAADmvwCAAAACfsAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA/AAAAD8AAAI+AAACIAEAABv6AAAAAAEAAAAR+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAABPgD////7AAAAPABNAGUAcwBoAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAPcA////+wAAADoAUABvAGkAbgB0AFMAYQBtAHAAbABpAG4AZwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABHQD////7AAAAOgBDAGEAbQBlAHIAYQBDAG8AbgB0AHIAbwBsAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAABuAP////sAAAAuAEkARwBUAEwAaQBuAGsAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAATEA////+wAAADgAVQBTAEEAYwBxAHUAcwBpAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAY0A////+wAAAEIAVAByAGEAYwBrAGUAZABDAGUAbgB0AGUAcgBsAGkAbgBlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAClAP////sAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAJwA////+wAAADIARgByAGEAbQBlAFQAcgBlAGUAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAGYA////+wAAADwA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");
		this->addDefaultDesktops(
						"NavigationUid",
						"LAYOUT_3D_ACS",
						"AAAA/wAAAAD9AAAAAQAAAAAAAAGaAAACgPwCAAAACPsAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA/AAAAD8AAAKAAAAAAAD////6/////wEAAAAT+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAfwD////7AAAALgBDAG8AbgBzAG8AbABlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAABUAP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAR0A////+wAAADoAQwBhAG0AZQByAGEAQwBvAG4AdAByAG8AbABXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAbgD////7AAAALgBJAEcAVABMAGkAbgBrAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAExAP////sAAAA4AFUAUwBBAGMAcQB1AHMAaQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAGNAP////sAAABCAFQAcgBhAGMAawBlAGQAQwBlAG4AdABlAHIAbABpAG4AZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAApQD////7AAAANABOAGEAdgBpAGcAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAACcAP////sAAAAyAEYAcgBhAG0AZQBUAHIAZQBlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAABmAP////sAAAA8AFQAbwBvAGwAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABYAD////7AAAARgBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBIAGkAcwB0AG8AcgB5AFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAECAP////sAAABEAEMAYQBsAGkAYgByAGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAGkAP////sAAABIAFYAaQBzAHUAYQBsAGkAegBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABPwD////7AAAARgBTAGUAZwBtAGUAbgB0AGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAACPAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAS0A////+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABPgD////7AAAAPABNAGUAcwBoAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAPcA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOwA////+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAABgQAAAR8A/////AAAAygAAADLAAAAAAD////6AAAAAAEAAAAB+wAAAEAAVAByAGEAbgBzAGYAZQByAEYAdQBuAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAAAAAAAD7AAAAPgBTAGgAaQBmAHQAQwBvAHIAcgBlAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD7AAAAQgBJAG0AYQBnAGUAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAAEtAAAAAAAAAAA+wAAAEYAUABhAHQAaQBlAG4AdABSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD8AAACXQAAAUAAAAAAAP////oAAAAAAQAAAAH7AAAAFABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAAAAAAAAAAAeAAAADmgAAAAQAAAAEAAAACAAAAAj8AAAABAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAIAAAAJAAAAHgBXAG8AcgBrAGYAbABvAHcAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAFgBEAGEAdABhAFQAbwBvAGwAQgBhAHIAAAAA5f////8AAAAAAAAAAAAAACIATgBhAHYAaQBnAGEAdABpAG8AbgBUAG8AbwBsAEIAYQByAQAAAOX/////AAAAAAAAAAAAAAAmAEMAYQBtAGUAcgBhADMARABWAGkAZQB3AFQAbwBvAGwAQgBhAHIAAAAB+/////8AAAAAAAAAAAAAABYAVABvAG8AbABUAG8AbwBsAEIAYQByAQAAAV7/////AAAAAAAAAAAAAAAiAFMAYwByAGUAZQBuAHMAaABvAHQAVABvAG8AbABCAGEAcgEAAAGz/////wAAAAAAAAAAAAAAHABEAGUAcwBrAHQAbwBwAFQAbwBvAGwAQgBhAHIBAAAB5P////8AAAAAAAAAAAAAACwASQBuAHQAZQByAGEAYwB0AG8AcgBTAHQAeQBsAGUAVABvAG8AbABCAGEAcgAAAALW/////wAAAAAAAAAAAAAAFgBIAGUAbABwAFQAbwBvAGwAQgBhAHIBAAACOQAAAXgAAAAAAAAAAA==");
		this->addDefaultDesktops(
						"RegistrationUid",
						"LAYOUT_3D_ACS",
						"AAAA/wAAAAD9AAAAAQAAAAAAAAGaAAADmvwCAAAACPsAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA/AAAAD8AAAOaAAACLgEAABv6AAAAAAEAAAAT+wAAAEYAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAABLQD////7AAAARgBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBIAGkAcwB0AG8AcgB5AFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQBAAAAAP////8AAAECAP////sAAAAuAEMAbwBuAHMAbwBsAGUAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAA/////wAAAFQA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAfwD////7AAAAOgBQAG8AaQBuAHQAUwBhAG0AcABsAGkAbgBnAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAEdAP////sAAAA6AEMAYQBtAGUAcgBhAEMAbwBuAHQAcgBvAGwAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAG4A////+wAAAC4ASQBHAFQATABpAG4AawBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABMQD////7AAAAOABVAFMAQQBjAHEAdQBzAGkAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABjQD////7AAAAQgBUAHIAYQBjAGsAZQBkAEMAZQBuAHQAZQByAGwAaQBuAGUAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAKUA////+wAAADQATgBhAHYAaQBnAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAnAD////7AAAAMgBGAHIAYQBtAGUAVAByAGUAZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAZgD////7AAAAPABUAG8AbwBsAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAWAA////+wAAAEQAQwBhAGwAaQBiAHIAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAaQA////+wAAAEgAVgBpAHMAdQBhAGwAaQB6AGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAE/AP////sAAABGAFMAZQBnAG0AZQBuAHQAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAI8A////+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABPgD////7AAAAPABNAGUAcwBoAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAPcA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOwA////+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAABgQAAAR8A/////AAAAygAAADLAAAAAAD////6AAAAAAEAAAAB+wAAAEAAVAByAGEAbgBzAGYAZQByAEYAdQBuAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAAAAAAAD7AAAAPgBTAGgAaQBmAHQAQwBvAHIAcgBlAGMAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD7AAAAQgBJAG0AYQBnAGUAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAAEtAAAAAAAAAAA+wAAAEYAUABhAHQAaQBlAG4AdABSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD8AAACXQAAAUAAAAAAAP////oAAAAAAQAAAAH7AAAAFABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAAD/////AAAAAAAAAAAAAAXgAAADmgAAAAQAAAAEAAAACAAAAAj8AAAABAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAIAAAAJAAAAHgBXAG8AcgBrAGYAbABvAHcAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAFgBEAGEAdABhAFQAbwBvAGwAQgBhAHIAAAAA5f////8AAAAAAAAAAAAAACIATgBhAHYAaQBnAGEAdABpAG8AbgBUAG8AbwBsAEIAYQByAQAAAOX/////AAAAAAAAAAAAAAAmAEMAYQBtAGUAcgBhADMARABWAGkAZQB3AFQAbwBvAGwAQgBhAHIBAAABXv////8AAAAAAAAAAAAAABYAVABvAG8AbABUAG8AbwBsAEIAYQByAQAAAmf/////AAAAAAAAAAAAAAAiAFMAYwByAGUAZQBuAHMAaABvAHQAVABvAG8AbABCAGEAcgEAAAK8/////wAAAAAAAAAAAAAAHABEAGUAcwBrAHQAbwBwAFQAbwBvAGwAQgBhAHIBAAAC7f////8AAAAAAAAAAAAAACwASQBuAHQAZQByAGEAYwB0AG8AcgBTAHQAeQBsAGUAVABvAG8AbABCAGEAcgAAAANC/////wAAAAAAAAAAAAAAFgBIAGUAbABwAFQAbwBvAGwAQgBhAHIBAAADQgAAAXgAAAAAAAAAAA==");
		this->addDefaultDesktops(
						"PreOpPlanningUid",
						"LAYOUT_3D_ACS",
						"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");
		this->addDefaultDesktops(
						"IntraOpImagingUid",
						"LAYOUT_US_Acquisition",
						"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");
		this->addDefaultDesktops(
						"PostOpControllUid",
						"LAYOUT_3D_ACS",
						"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");
		this->addDefaultDesktops("DEFAULT", "LAYOUT_3D_ACS", fullState);
	}

	~ApplicationsParser() {}

	void addDefaultDesktops(QString workflowStateUid, QString layoutUid, QString mainwindowstate)
	{
		mWorkflowDefaultDesktops[workflowStateUid] = Desktop(layoutUid,
						QByteArray::fromBase64(mainwindowstate.toAscii()));
	}

	Desktop getDefaultDesktop(QString applicationName, QString workflowName)
	{
		//TODO use applicationName!!!
		if (!mWorkflowDefaultDesktops.count(workflowName))
			return mWorkflowDefaultDesktops["DEFAULT"];
		return mWorkflowDefaultDesktops[workflowName];
	}

	Desktop getDesktop(QString applicationName, QString workflowName)
	{
		Desktop retval;
		QDomElement workflowElement =
						mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).getElement();
		QDomElement desktopElement;
		if (workflowElement.namedItem("custom").isNull())
		{
			return this->getDefaultDesktop(applicationName, workflowName);
		}
		else
		{
			desktopElement = workflowElement.namedItem("custom").toElement();
		}
		retval.mMainWindowState = QByteArray::fromBase64(desktopElement.attribute("mainwindowstate").toAscii());
		retval.mLayoutUid = desktopElement.attribute("layoutuid");

		return retval;
	}

	void setDesktop(QString applicationName, QString workflowName, Desktop desktop)
	{
		QDomElement desktopElement =
						mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).descend("custom").getElement();
		desktopElement.setAttribute("mainwindowstate", QString(desktop.mMainWindowState.toBase64()));
		desktopElement.setAttribute("layoutuid", desktop.mLayoutUid);
		mXmlFile.save();
	}

	void resetDesktop(QString applicationName, QString workflowName)
	{
		QDomElement workflowElement =
						mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).getElement();
		workflowElement.removeChild(workflowElement.namedItem("custom"));
		mXmlFile.save();
	}

private:
	ssc::XmlOptionFile mXmlFile;
	std::map<QString, Desktop> mWorkflowDefaultDesktops;
};

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

StateService *StateService::mTheInstance = NULL;
StateService* stateService()
{
	return StateService::getInstance();
}
StateService* StateService::getInstance()
{
	if (mTheInstance == NULL)
	{
		mTheInstance = new StateService();
		mTheInstance->initialize();
	}
	return mTheInstance;
}

void StateService::destroyInstance()
{
	delete mTheInstance;
	mTheInstance = NULL;
}

StateService::StateService()
{
}

StateService::~StateService()
{
}

QString StateService::getVersionName()
{
	QString versionName;
#ifdef VERSION_NUMBER_VERBOSE
	versionName = QString("%1").arg(VERSION_NUMBER_VERBOSE);
#else
#endif
	return versionName;
}

WorkflowStateMachinePtr StateService::getWorkflow()
{
	return mWorkflowStateMachine;
}

ApplicationStateMachinePtr StateService::getApplication()
{
	return mApplicationStateMachine;
}

template<class T>
void StateService::fillDefault(QString name, T value)
{
	if (!settings()->contains(name))
		settings()->setValue(name, value);
}

/** Look for a grabber server with a given name in path.
 *  If found, return filename with args relative to bundle dir.
 *
 */
QString StateService::checkGrabberServerExist(QString path, QString filename, QString args)
{
//	std::cout << "checking [" << path << "] [" << filename << "] [" << args << "]" << std::endl;
	path = QDir::cleanPath(path);
//	std::cout << "  cleaned [" << path << "]" << std::endl;
	if (QDir(path).exists(filename))
	{
//		std::cout << "  found [" << filename << "]" << std::endl;
		return QDir(DataLocations::getBundlePath()).relativeFilePath(path + "/" + filename) + " " + args;
	}

	return "";
}

/**Return the location of external video grabber application that
 * can be used as a local server controlled by CustusX.
 *
 */
QString StateService::getDefaultGrabberServer()
{
#ifdef __APPLE__
	QString filename = "GrabberServer.app";
	QString postfix = " --auto";
	QString result;
	result = this->checkGrabberServerExist(DataLocations::getBundlePath(), filename, postfix);
	if (!result.isEmpty())
	return result;
	result = this->checkGrabberServerExist(DataLocations::getRootConfigPath() + "/../install/Apple", filename, postfix);
	if (!result.isEmpty())
	return result;
	return "";
#elif WIN32
	return ""; // TODO fill in something here...
#else
	QString result;
	result = this->checkGrabberServerExist(DataLocations::getBundlePath() + "/..", "runOpenIGTLinkServer.sh", "");
	if (!result.isEmpty())
		return result;
	result = this->checkGrabberServerExist(DataLocations::getBundlePath() + "/../../../modules/OpenIGTLinkServer",
					"OpenIGTLinkServer", "");
	if (!result.isEmpty())
		return result;
	return "";
#endif
}

/**Enter all default Settings here.
 *
 */
void StateService::fillDefaultSettings()
{
	this->fillDefault("Automation/autoStartTracking", true);
	this->fillDefault("Automation/autoStartStreaming", true);
	this->fillDefault("Automation/autoReconstruct", true);
	this->fillDefault("Automation/autoSelectDominantTool", true);
	this->fillDefault("renderingInterval", 33);
	this->fillDefault("backgroundColor", QColor("black"));
	this->fillDefault("globalPatientDataFolder", QDir::homePath() + "/Patients");
	this->fillDefault("globalApplicationName", enum2string(ssc::mdLABORATORY));
	this->fillDefault("globalPatientNumber", 1);
	this->fillDefault("Ultrasound/acquisitionName", "US-Acq");
	this->fillDefault("Ultrasound/8bitAcquisitionData", true);
	this->fillDefault("View3D/sphereRadius", 1.0);
	this->fillDefault("View3D/labelSize", 2.5);
	this->fillDefault("View3D/showOrientationAnnotation", true);

	this->fillDefault("IGTLink/localServer", this->getDefaultGrabberServer());

	this->fillDefault("showSectorInRTView", true);
//  this->fillDefault("autoLandmarkRegistration", true);
	this->fillDefault("View3D/stereoType", stINTERLACED);
	this->fillDefault("View3D/eyeAngle", 4.0);
	this->fillDefault("View/showDataText", true);

	this->fillDefault("View3D/annotationModelSize", 0.2);
	this->fillDefault("View3D/annotationModel", "woman.stl");
	this->fillDefault("View3D/depthPeeling", false);

#ifdef __APPLE__
	this->fillDefault("useGPUVolumeRayCastMapper", false);
	this->fillDefault("useGPU2DRendering", false);
	this->fillDefault("stillUpdateRate", 8.0);
#elif WIN32
	this->fillDefault("useGPUVolumeRayCastMapper", true);
	this->fillDefault("useGPU2DRendering", false);
	this->fillDefault("stillUpdateRate", 0.001);
#else
	this->fillDefault("useGPUVolumeRayCastMapper", true);
	this->fillDefault("useGPU2DRendering", true);
	this->fillDefault("stillUpdateRate", 0.001);
#endif

	this->fillDefault("smartRender", true);

	this->fillDefault("IGSTKDebugLogging", false);
	this->fillDefault("giveManualToolPhysicalProperties", false);

	this->fillDefault("applyTransferFunctionPresetsToAll", true);
}

void StateService::initialize()
{
	this->fillDefaultSettings();

	mApplicationStateMachine.reset(new ApplicationStateMachine());
	mApplicationStateMachine->start();

	mWorkflowStateMachine.reset(new WorkflowStateMachine());
	mWorkflowStateMachine->start();
}

Desktop StateService::getActiveDesktop()
{
	ApplicationsParser parser;
	return parser.getDesktop(mApplicationStateMachine->getActiveUidState(), mWorkflowStateMachine->getActiveUidState());
}

void StateService::saveDesktop(Desktop desktop)
{
	ApplicationsParser parser;
	parser.setDesktop(mApplicationStateMachine->getActiveUidState(), mWorkflowStateMachine->getActiveUidState(),
					desktop);
}

void StateService::resetDesktop()
{
	ApplicationsParser parser;
	parser.resetDesktop(mApplicationStateMachine->getActiveUidState(), mWorkflowStateMachine->getActiveUidState());
}

} //namespace cx
