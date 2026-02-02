file(GLOB HEADER_FILES *.h *.txx)
file(GLOB SOURCES *.cxx)

if(NOT CAFAnaCore_USE_Ifdhc)
  list(FILTER HEADER_FILES EXCLUDE REGEX ".*SAM.*")
  list(FILTER SOURCES EXCLUDE REGEX ".*SAM.*")
  list(FILTER HEADER_FILES EXCLUDE REGEX ".*Wildcard.*")
  list(FILTER SOURCES EXCLUDE REGEX ".*Wildcard.*")
endif()

cet_make_library(LIBRARY_NAME CAFAnaCore
  SOURCE
    ${SOURCES}
    ${HEADER_FILES}

  LIBRARIES
    CAFAnaCore_options
)

install_headers()
install_source()
