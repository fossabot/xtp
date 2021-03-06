if(NOT VOTCA_SPHINX_DIR)
  set(VOTCA_SPHINX_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()
if(CMAKE_VERSION VERSION_LESS 3.12)
  find_package(PythonInterp 3) # Deprecated since version 3.12
  set_package_properties(PythonInterp PROPERTIES TYPE OPTIONAL PURPOSE "Used to build documentation files")
  if(PYTHONINTERP_FOUND)
    set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
  endif()
else()
  find_package(Python 3 COMPONENTS Interpreter)
  set_package_properties(Python PROPERTIES TYPE OPTIONAL PURPOSE "Used to build documentation files")
endif()

if(Python_EXECUTABLE)
  file(GLOB XMLS ${CMAKE_CURRENT_SOURCE_DIR}/../xtp/xml/*.xml)

  set(XML_RST_FILES)
  foreach(_XML ${XMLS})
    get_filename_component(_FILE "${_XML}" NAME_WE)
    add_custom_command(OUTPUT ${VOTCA_SPHINX_DIR}/${_FILE}.rst COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/extract_metadata.py -i ${_XML} -o ${VOTCA_SPHINX_DIR}/${_FILE}.rst
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/extract_metadata.py ${_XML})
    list(APPEND XML_RST_FILES ${VOTCA_SPHINX_DIR}/${_FILE}.rst)
  endforeach()
  add_custom_command(OUTPUT ${VOTCA_SPHINX_DIR}/qmpackage_defaults.rst COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/extract_metadata.py -m qm -i ${CMAKE_CURRENT_SOURCE_DIR}/../xtp/data/qmpackage_defaults.xml -o ${VOTCA_SPHINX_DIR}/qmpackage_defaults.rst
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/extract_metadata.py ${CMAKE_CURRENT_SOURCE_DIR}/../xtp/data/qmpackage_defaults.xml)
  list(APPEND XML_RST_FILES ${VOTCA_SPHINX_DIR}/qmpackage_defaults.rst)
  add_custom_target(doc-xtp DEPENDS ${XML_RST_FILES})
endif()
