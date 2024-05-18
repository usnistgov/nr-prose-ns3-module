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
set(nr-prose-examples_source_files
    nr-v2x-examples/ue-mac-pscch-tx-output-stats.cc
    nr-v2x-examples/ue-mac-pssch-tx-output-stats.cc
    nr-v2x-examples/ue-phy-pscch-rx-output-stats.cc
    nr-v2x-examples/ue-phy-pssch-rx-output-stats.cc
    nr-v2x-examples/ue-rlc-rx-output-stats.cc
    nr-v2x-examples/ue-to-ue-pkt-txrx-output-stats.cc
    nr-v2x-examples/v2x-kpi.cc
)
foreach(
  example
  ${nr-prose-examples_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
                 ${nr-prose-examples_source_files}
    LIBRARIES_TO_LINK ${libnr}
  )
endforeach()
foreach(
  example
  ${nr-prose-examples_flowmon_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
                 ${nr-prose-examples_source_files}
    LIBRARIES_TO_LINK ${libnr}
                      ${libflow-monitor}
  )
endforeach()

set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${libnr} ${libflow-monitor})
if(${NS3_NETSIMULYZER})
  set(PROSE_L3_RELAY_ON_OFF_LIBRARIES ${PROSE_L3_RELAY_ON_OFF_LIBRARIES} ${libnetsimulyzer})
endif()

build_lib_example(
  NAME nr-prose-l3-relay-on-off
  SOURCE_FILES nr-prose-l3-relay-on-off.cc
               ${nr-prose-examples_source_files}
  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
)

# To be enabled when psc and sip modules are added
#build_lib_example(
#  NAME nr-prose-l3-relay-mcptt
#  SOURCE_FILES nr-prose-l3-relay-mcptt.cc
#               ${nr-prose-examples_source_files}
#  LIBRARIES_TO_LINK ${PROSE_L3_RELAY_ON_OFF_LIBRARIES}
#                    ${libpsc}
#                    ${libsip}
