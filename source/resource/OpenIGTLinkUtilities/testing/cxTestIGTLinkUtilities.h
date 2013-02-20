#ifndef CXTESTACQUISITION_H_
#define CXTESTACQUISITION_H_

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "vtkImageData.h"
#include "sscImage.h"

/**Unit tests that test IGTLink utilities
 */
class TestIGTLinkUtilities : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testDecodeEncodeImage();
	void testDecodeEncodeProbeData();
public:
	CPPUNIT_TEST_SUITE( TestIGTLinkUtilities );
		CPPUNIT_TEST( testConstructor );
		CPPUNIT_TEST( testDecodeEncodeImage );
		CPPUNIT_TEST( testDecodeEncodeProbeData );
	CPPUNIT_TEST_SUITE_END();
private:
	int getValue(ssc::ImagePtr data, int x, int y, int z);
	void setValue(vtkImageDataPtr data, int x, int y, int z, unsigned char val);
};
CPPUNIT_TEST_SUITE_REGISTRATION( TestIGTLinkUtilities );


#endif /*CXTESTACQUISITION_H_*/