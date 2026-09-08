function(opencagd_configure_output_directories)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" PARENT_SCOPE)

    foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${cfg}" cfg_upper)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg_upper} "${CMAKE_BINARY_DIR}/lib" PARENT_SCOPE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg_upper} "${CMAKE_BINARY_DIR}/lib" PARENT_SCOPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg_upper} "${CMAKE_BINARY_DIR}/bin" PARENT_SCOPE)
    endforeach()
endfunction()

function(opencagd_apply_library_defaults target export_define export_name)
    add_library(OpenCAGD::${export_name} ALIAS ${target})
    set_target_properties(${target} PROPERTIES EXPORT_NAME ${export_name})
    target_include_directories(${target}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_compile_features(${target} PUBLIC cxx_std_14)
    target_compile_definitions(${target} PRIVATE ${export_define})
    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${target} PUBLIC OPENCAGD_STATIC_DEFINE)
    endif()
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        set_target_properties(${target} PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
    endif()
    install(TARGETS ${target} EXPORT OpenCAGDTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endfunction()

function(opencagd_set_runtime_output target category module)
    set(_output "${CMAKE_BINARY_DIR}/${category}/${module}")
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${_output}")
    foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${cfg}" cfg_upper)
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${cfg_upper} "${_output}")
    endforeach()
endfunction()

function(opencagd_add_example target source module)
    add_executable(${target} ${source})
    target_compile_features(${target} PRIVATE cxx_std_14)
    opencagd_set_runtime_output(${target} examples ${module})
endfunction()

function(opencagd_add_gtest target source module)
    add_executable(${target} ${source})
    target_compile_features(${target} PRIVATE cxx_std_14)
    target_link_libraries(${target} PRIVATE GTest::gtest_main)
    opencagd_set_runtime_output(${target} tests ${module})
    gtest_discover_tests(${target}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests/${module}"
        DISCOVERY_TIMEOUT 30)
endfunction()
