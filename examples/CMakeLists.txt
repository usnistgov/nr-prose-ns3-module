set(nr-prose-examples_examples
    nr-prose-unicast-single-link
    nr-prose-discovery
    nr-prose-discovery-l3-relay
    nr-prose-discovery-l3-relay-selection
)
set(nr-prose-examples_flowmon_examples
    nr-prose-unicast-multi-link
    nr-prose-unicast-l3-relay
    nr-prose-network-coex
    nr-prose-l3-relay
)
foreach(
  example
  ${nr-prose-examples_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${libnr}
                      ${libnr-prose}
  )
endforeach()
foreach(
  example
  ${nr-prose-examples_flowmon_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK ${libnr}
                      ${libnr-prose}
                      ${libflow-monitor}
  )
endforeach()

set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${libnr} ${libnr-prose} ${libflow-monitor})
if(${NS3_NETSIMULYZER})
  set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${PROSE_L3_RELAY_ON_OFF_LIBRARIES} ${libnetsimulyzer})
endif()

build_lib_example(
  NAME nr-prose-l3-relay-on-off
  SOURCE_FILES nr-prose-l3-relay-on-off.cc
  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
)

# Also requires psc and sip modules; uncomment the below if they are present
#build_lib_example(
#  NAME nr-prose-l3-relay-mcptt
#  SOURCE_FILES nr-prose-l3-relay-mcptt.cc
#  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
#                    ${libnr-prose}
#                    ${libpsc}
#                    ${libsip}
#)
