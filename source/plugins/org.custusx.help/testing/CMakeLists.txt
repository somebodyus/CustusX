
if(BUILD_TESTING)

    set(CX_TEST_CATCH_ORG_CUSTUSX_HELP_MOC_SOURCE_FILES
	)
    set(CX_TEST_CATCH_ORG_CUSTUSX_HELP_SOURCE_FILES
		cxtestHelpPlugin.h
		cxtestHelpPlugin.cpp
        cxtestDummyHelp.h
        cxtestDummyHelp.cpp
    )

    qt5_wrap_cpp(CX_TEST_CATCH_ORG_CUSTUSX_HELP_MOC_SOURCE_FILES ${CX_TEST_CATCH_ORG_CUSTUSX_HELP_MOC_SOURCE_FILES})
    add_library(cxtest_org_custusx_help ${CX_TEST_CATCH_ORG_CUSTUSX_HELP_SOURCE_FILES} ${CX_TEST_CATCH_ORG_CUSTUSX_HELP_MOC_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_help)
    target_include_directories(cxtest_org_custusx_help
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
	target_link_libraries(cxtest_org_custusx_help
		PRIVATE
		org_custusx_help
		cxGui
#        cxResourceWidgets
		cxLogicManager
		cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_help)


endif(BUILD_TESTING)
