//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_PROSE_HELPER_H
#define NR_SL_PROSE_HELPER_H

#include "nr-sl-discovery-trace.h"
#include "nr-sl-relay-trace.h"

#include <ns3/lte-rrc-sap.h>
#include <ns3/net-device-container.h>
#include <ns3/nr-sl-helper.h>
#include <ns3/nr-sl-ue-prose-relay-selection-algorithm.h>
#include <ns3/nr-sl-ue-prose.h>
#include <ns3/object.h>

namespace ns3
{

class NrEpcTft;
class NrEpsBearer;
class NrPointToPointEpcHelper;
class NrUeNetDevice;

class NrSlProseHelper : public Object
{
    /**
     * \brief Class to help in the configuration of the Proximity Service (ProSe)
     *        functionalities
     */
  public:
    /**
     * \brief Constructor
     */
    NrSlProseHelper(void);
    /**
     * \brief Destructor
     */
    virtual ~NrSlProseHelper(void);
    /**
     * \brief GetTypeId, inherited from Object
     *
     * \returns The TypeId
     */
    static TypeId GetTypeId(void);
    /**
     * \brief Set EPC helper
     *
     * \param epcHelper Ptr of type NrPointToPointEpcHelper
     */
    void SetEpcHelper(const Ptr<NrPointToPointEpcHelper>& epcHelper);
    /**
     * \brief Prepare UE for ProSe
     *
     * This method installs the ProSe layer in the UE(s) in the container
     *
     * \param c The NetDeviceContainer
     */
    void PrepareUesForProse(NetDeviceContainer c);
    /**
     * \brief Prepare UE for Unicast ProSe Direct Communication
     *
     * This method configures the UE(s) in the container to be able to do unicast
     * ProSe direct communication
     *
     * \param c The \c NetDeviceContainer
     */
    void PrepareUesForUnicast(NetDeviceContainer c);

    /**
     * \brief Establish a 5G ProSe direct link between the two UEs using the real
     *        protocol
     *
     * This method schedules the creation of the direct link instances in both UEs
     * participating in the direct link. Then, the ProSe layer configures the direct
     * link instances and starts the establishment procedure in the initiating UE.
     * A real protocol means that PC5-S messages used for establishing and
     * maintaining the direct link connection go through the protocol stack, and
     * are transmitted in SL-SRBs and sent over the SL
     *
     * \param time The time at which the direct link instances should be created
     * \param initUe The UE initiating the establishment procedure
     * \param initUeIp The IP address used by the initiating UE
     * \param initSlInfo the traffic profile parameters to be used for the sidelink data radio
     * bearer on the initiating UE \param trgtUE The peer UE \param trgtUeIp The IP address used by
     * the target UE \param trgtSlInfo the traffic profile parameters to be used for the sidelink
     * data radio bearer on the target UE
     */
    void EstablishRealDirectLink(Time time,
                                 Ptr<NetDevice> initUe,
                                 Ipv4Address initUeIp,
                                 struct SidelinkInfo& initSlInfo,
                                 Ptr<NetDevice> trgtUe,
                                 Ipv4Address trgtUeIp,
                                 struct SidelinkInfo& trgtSlInfo);

    /**
     * \brief Establish a 5G ProSe L3 UE-to-Network (U2N) relay connection between
     *        two UEs (a remote UE and a relay UE)
     *
     * This method schedules the creation of the corresponding direct link instances
     * in both UEs participating in the U2N relay connection (Remote UE is the
     * initiating UE of the direct link and relay UE is the target UE). Then, the
     * ProSe layer configures the direct link instances and starts the establishment
     * procedure in the remote UE.
     *
     * \param t The time at which the L3 U2N connection procedure should start
     * \param remoteUe The remote UE of the connection
     * \param remoteUeIp The IPv4 address used by the remote UE
     * \param remoteUeSlInfo the traffic profile parameters to be used for the sidelink data radio
     * bearer on the remote UE \param relayUE The relay UE \param relayUeIp The IPv4 address used by
     * the relay UE \param relayUeSlInfo the traffic profile parameters to be used for the sidelink
     * data radio bearer on the relay UE \param relayServiceCode the relay service code associated
     * to this direct link
     */
    void EstablishL3UeToNetworkRelayConnection(Time t,
                                               Ptr<NetDevice> remoteUe,
                                               Ipv4Address remoteUeIp,
                                               struct SidelinkInfo& remoteUeSlInfo,
                                               Ptr<NetDevice> relayUe,
                                               Ipv4Address relayUeIp,
                                               struct SidelinkInfo& relayUeSlInfo,
                                               uint32_t relayServiceCode);

    /**
     * \brief Install configuration on the UEs that will act as L3 UE-to-Network (U2N)
     *        relay UEs
     *
     *  This method activates the EPS bearer to be used for relaying traffic on
     *  each relay UE device, and internally sets the pointer to the EpcHelper in
     *  the ProSe layer. The EpcHelper will be used by the ProSe layer to configure
     *  the data path in the EpcPgwApplication when a remote UE successfully connects
     *  to the relay UE
     *
     * \param ueDevices the devices in which the L3 U2N relay configuration will be installed
     * \param relayServiceCodes the relay service codes to which the configuration will be
     * associated \param bearer the EPS bearer to be used for relaying traffic \param tft the
     * traffic flow template to be used for relaying traffic
     */
    void ConfigureL3UeToNetworkRelay(const NetDeviceContainer ueDevices,
                                     const std::set<uint32_t> relayServiceCodes,
                                     NrEpsBearer bearer,
                                     Ptr<NrEpcTft> tft);

    /**
     * Starts discovery process for given applications depending on the interest (monitoring or
     * announcing) \param ueDevice the targeted device \param appCode application code to be added
     * \param dstL2Id destination layer 2 ID to be set for this appCode
     * \param role UE role (discovered or discoveree)
     */
    void StartDiscoveryApp(Ptr<NetDevice> ueDevice,
                           uint32_t appCode,
                           uint32_t dstL2Id,
                           NrSlUeProse::DiscoveryRole role);

    /**
     * Stops discovery process for given applications
     * \param ueDevice the targeted device
     * \param appCode application code to be removed
     * \param role UE role (discovered or discoveree)
     */
    void StopDiscoveryApp(Ptr<NetDevice> ueDevice,
                          uint32_t appCode,
                          NrSlUeProse::DiscoveryRole role);

    /**
     * Starts discovery process for given applications depending on the interest (monitoring or
     * announcing) \param ueDevice the targeted device \param appCodes application code to be added
     * \param dstL2Ids destination layer 2 IDs to be set for each appCode
     * \param role UE role (discovered or discoveree)
     */
    void StartDiscovery(Ptr<NetDevice> ueDevice,
                        std::list<uint32_t> appCodes,
                        std::list<uint32_t> dstL2Ids,
                        NrSlUeProse::DiscoveryRole role);

    /**
     * Stops discovery process for given applications
     * \param ueDevice the targeted device
     * \param appCodes application codes to be removed
     * \param role UE role (discovered or discoveree)
     */
    void StopDiscovery(Ptr<NetDevice> ueDevice,
                       std::list<uint32_t> appCodes,
                       NrSlUeProse::DiscoveryRole role);

    /**
     * Starts relay discovery process depending on the interest (relay or remote)
     * \param ueDevice the targeted device
     * \param relayCode relay code
     * \param dstL2Ids destination layer 2 ID
     * \param model UE model (A or B)
     * \param role UE role (relay or remote)
     */
    void StartRelayDiscovery(Ptr<NetDevice> ueDevice,
                             uint32_t relayCode,
                             uint32_t dstL2Id,
                             NrSlUeProse::DiscoveryModel model,
                             NrSlUeProse::DiscoveryRole role);

    /**
     * Stops relay discovery process for given code
     * \param ueDevice the targeted device
     * \param relayCode relay code to be removed
     * \param role UE role (relay or remote)
     */
    void StopRelayDiscovery(Ptr<NetDevice> ueDevice,
                            uint32_t relayCode,
                            NrSlUeProse::DiscoveryRole role);

    /**
     * Enable trace sinks for ProSe discovery
     */
    void EnableDiscoveryTraces(void);

    /**
     * Enable trace sinks for ProSe relay selection
     */
    void EnableRelayTraces(void);

    /**
     * Start Relay discovery and link establishment betwwen relay and remote
     *
     * \param remoteDevices Net Devices of remote UEs
     * \param remoteTime when to start the discovery for remote UEs
     * \param relayDevices Net Devices of relay UEs
     * \param relayTime when to start the discovery for relay UEs
     * \param relayCodes relay codes to be announced
     * \param dstL2Ids destination layer 2 IDs to be associated with the relays
     * \param discoveryModel the discovery model considered: Model A or Model B
     * \param selectionAlgorithm the relay (re)selection algorithm considered
     * \param tft the traffic flow template to be used for relaying traffic
     * \param bearer EPS beraer to use for relaying traffic
     */
    void StartRemoteRelayConnection(const NetDeviceContainer remoteDevices,
                                    const std::vector<Time> remoteTime,
                                    const NetDeviceContainer relayDevices,
                                    const std::vector<Time> relayTime,
                                    const std::vector<uint32_t> relayCodes,
                                    const std::vector<uint32_t> dstL2Ids,
                                    NrSlUeProse::DiscoveryModel discoveryModel,
                                    Ptr<NrSlUeProseRelaySelectionAlgorithm> selectionAlgorithm,
                                    Ptr<NrEpcTft> tft,
                                    NrEpsBearer bearer);

    /**
     * \brief Install NR Sidelink relay discovery/(re)selection configuration for both remote and
     * relay UEs
     *
     * \param relays the relay UEs net device container
     * \param remotes the remote UEs net device container
     * \param discConfig the structure defined as NrRrcSap::SlDiscConfigCommon
     */
    void InstallNrSlDiscoveryConfiguration(NetDeviceContainer relays,
                                           NetDeviceContainer remotes,
                                           const NrRrcSap::SlDiscConfigCommon discConfig);

  protected:
    /**
     * \brief \c DoDispose method inherited from \c Object
     */
    virtual void DoDispose(void) override;

  private:
    /**
     * \brief Prepare Single UE for ProSe
     *
     *  Install ProSe layer in the device and connect corresponding SAPs
     *
     * \param nrUeDev The Ptr to NR UE NetDevice
     */
    void PrepareSingleUeForProse(Ptr<NrUeNetDevice> nrUeDev);
    /**
     * \brief  Prepare UE for Unicast ProSe Direct Communication
     *
     * \param nrUeDev The Ptr to NR UE NetDevice
     */
    void PrepareSingleUeForUnicast(Ptr<NrUeNetDevice> nrUeDev);

    Ptr<NrPointToPointEpcHelper> m_epcHelper; //!< pointer to the EPC helper

    Ptr<NrSlDiscoveryTrace> m_discoveryTrace; //!< Container of discovery traces.

    Ptr<NrSlRelayTrace> m_relayTrace; //!< Container of relay traces
};

} // namespace ns3

#endif /* NR_SL_PROSE_HELPER_H */
