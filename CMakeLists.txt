set(source_files
    helper/nr-sl-discovery-trace.cc
    helper/nr-sl-prose-helper.cc
    helper/nr-sl-relay-trace.cc
    model/nr-sl-discovery-header.cc
    model/nr-sl-pc5-signalling-header.cc
    model/nr-sl-ue-prose.cc
    model/nr-sl-ue-prose-direct-link.cc
    model/nr-sl-ue-prose-relay-selection-algorithm.cc
    model/nr-sl-ue-service.cc
)

set(header_files
    helper/nr-sl-discovery-trace.h
    helper/nr-sl-prose-helper.h
    helper/nr-sl-relay-trace.h
    model/nr-sl-discovery-header.h
    model/nr-sl-pc5-signalling-header.h
    model/nr-sl-ue-prose-direct-link.h
    model/nr-sl-ue-prose.h
    model/nr-sl-ue-service.h
    model/nr-sl-ue-prose-relay-selection-algorithm.h
)


set(test_sources
)

build_lib(
  LIBNAME nr-prose
  SOURCE_FILES ${source_files}
  HEADER_FILES ${header_files}
  LIBRARIES_TO_LINK
    ${liblte}
    ${libinternet-apps}
    ${libnr}
  TEST_SOURCES ${test_sources}
)
