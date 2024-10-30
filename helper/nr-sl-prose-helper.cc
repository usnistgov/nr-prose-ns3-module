//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-prose-helper.h"

#include <ns3/config.h>
#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/nr-epc-ue-nas.h>
#include <ns3/nr-point-to-point-epc-helper.h>
#include <ns3/nr-sl-ue-prose.h>
#include <ns3/nr-sl-ue-rrc.h>
#include <ns3/nr-sl-ue-service.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlProseHelper");

NS_OBJECT_ENSURE_REGISTERED(NrSlProseHelper);

NrSlProseHelper::NrSlProseHelper(void)

{
    NS_LOG_FUNCTION(this);
    m_discoveryTrace = CreateObject<NrSlDiscoveryTrace>();
    m_relayTrace = CreateObject<NrSlRelayTrace>();
}

NrSlProseHelper::~NrSlProseHelper(void)
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlProseHelper::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::NrSlProseHelper")
                            .SetParent<Object>()
                            .SetGroupName("nr")
                            .AddConstructor<NrSlProseHelper>();
    return tid;
}

void
NrSlProseHelper::DoDispose(void)
{
    NS_LOG_FUNCTION(this);
    Object::DoDispose();
}

void
NrSlProseHelper::SetEpcHelper(const Ptr<NrPointToPointEpcHelper>& epcHelper)
{
    NS_LOG_FUNCTION(this);
    m_epcHelper = epcHelper;
}

void
NrSlProseHelper::PrepareUesForProse(NetDeviceContainer c)
{
    NS_LOG_FUNCTION(this);
    for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<NetDevice> netDev = *i;
        Ptr<NrUeNetDevice> nrUeDev = netDev->GetObject<NrUeNetDevice>();
        PrepareSingleUeForProse(nrUeDev);
    }
}

void
NrSlProseHelper::PrepareSingleUeForProse(Ptr<NrUeNetDevice> nrUeDev)
{
    NS_LOG_FUNCTION(this);

    // Create ProSe layer
    Ptr<NrSlUeProse> nrSlUeProse = CreateObject<NrSlUeProse>();

    // Connect ProSe layer SAPs
    Ptr<NrUeRrc> nrUeRrc = nrUeDev->GetRrc();
    nrSlUeProse->SetNrSlUeSvcRrcSapProvider(nrUeRrc->GetNrSlUeSvcRrcSapProvider());
    nrUeRrc->SetNrSlUeSvcRrcSapUser(nrSlUeProse->GetNrSlUeSvcRrcSapUser());

    Ptr<NrEpcUeNas> epcUeNas = nrUeDev->GetNas();
    nrSlUeProse->SetNrSlUeSvcNasSapProvider(epcUeNas->GetNrSlUeSvcNasSapProvider());
    epcUeNas->SetNrSlUeSvcNasSapUser(nrSlUeProse->GetNrSlUeSvcNasSapUser());

    // Keep the ProSe layer accessible in the net device
    nrUeDev->AggregateObject(nrSlUeProse);
}

void
NrSlProseHelper::PrepareUesForUnicast(NetDeviceContainer c)
{
    NS_LOG_FUNCTION(this);
    for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<NetDevice> netDev = *i;
        Ptr<NrUeNetDevice> nrUeDev = netDev->GetObject<NrUeNetDevice>();
        PrepareSingleUeForUnicast(nrUeDev);
    }
}

void
NrSlProseHelper::PrepareSingleUeForUnicast(Ptr<NrUeNetDevice> nrUeDev)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrSlUeProse> nrSlUeProse = nrUeDev->GetObject<NrSlUeProse>();
    nrSlUeProse->ConfigureUnicast();
}

void
NrSlProseHelper::EstablishRealDirectLink(Time time,
                                         Ptr<NetDevice> initUe,
                                         Ipv4Address initUeIp,
                                         struct SidelinkInfo& initSlInfo,
                                         Ptr<NetDevice> trgtUe,
                                         Ipv4Address trgtUeIp,
                                         struct SidelinkInfo& trgtSlInfo)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrUeNetDevice> initUeNetDev = initUe->GetObject<NrUeNetDevice>();
    Ptr<NrUeNetDevice> trgtUeNetDev = trgtUe->GetObject<NrUeNetDevice>();
    Ptr<NrSlUeProse> initUeProse = initUeNetDev->GetObject<NrSlUeProse>();
    Ptr<NrSlUeProse> trgtUeProse = trgtUeNetDev->GetObject<NrSlUeProse>();
    Ptr<NrUeRrc> initUeRrc = initUeNetDev->GetRrc();
    Ptr<NrUeRrc> trgtUeRrc = trgtUeNetDev->GetRrc();

    initUeProse->SetImsi(initUeRrc->GetImsi());
    trgtUeProse->SetImsi(trgtUeRrc->GetImsi());

    uint32_t initUeL2Id = initUeRrc->GetSourceL2Id();
    uint32_t trgtUeL2Id = trgtUeRrc->GetSourceL2Id();

    initUeProse->SetL2Id(initUeL2Id);
    trgtUeProse->SetL2Id(trgtUeL2Id);

    NS_LOG_INFO("initUeL2Id " << initUeL2Id << " trgtUeL2Id " << trgtUeL2Id);

    initSlInfo.m_dstL2Id = trgtUeL2Id;
    initSlInfo.m_srcL2Id = initUeL2Id;
    trgtSlInfo.m_dstL2Id = initUeL2Id;
    trgtSlInfo.m_srcL2Id = trgtUeL2Id;

    // Initiating UE
    Simulator::Schedule(time,
                        &NrSlUeProse::AddDirectLinkConnection,
                        initUeProse,
                        initUeL2Id,
                        initUeIp,
                        trgtUeL2Id,
                        true,
                        0,
                        initSlInfo);

    // Target UE
    Simulator::Schedule(time,
                        &NrSlUeProse::AddDirectLinkConnection,
                        trgtUeProse,
                        trgtUeL2Id,
                        trgtUeIp,
                        initUeL2Id,
                        false,
                        0,
                        trgtSlInfo);
}

void
NrSlProseHelper::EstablishL3UeToNetworkRelayConnection(Time t,
                                                       Ptr<NetDevice> remoteUe,
                                                       Ipv4Address remoteUeIp,
                                                       struct SidelinkInfo& remoteUeSlInfo,
                                                       Ptr<NetDevice> relayUe,
                                                       Ipv4Address relayUeIp,
                                                       struct SidelinkInfo& relayUeSlInfo,
                                                       uint32_t relayServiceCode)
{
    NS_LOG_FUNCTION(this);

    if (relayServiceCode <= 0)
    {
        NS_FATAL_ERROR(
            "Please provide a relay service code greater than zero for U2N relay connection.");
    }
    Ptr<NrUeNetDevice> remoteUeNetDev = remoteUe->GetObject<NrUeNetDevice>();
    Ptr<NrUeNetDevice> relayUeNetDev = relayUe->GetObject<NrUeNetDevice>();
    Ptr<NrSlUeProse> remoteUeProse = remoteUeNetDev->GetObject<NrSlUeProse>();
    Ptr<NrSlUeProse> relayUeProse = relayUeNetDev->GetObject<NrSlUeProse>();
    Ptr<NrUeRrc> remoteUeRrc = remoteUeNetDev->GetRrc();
    Ptr<NrUeRrc> relayUeRrc = relayUeNetDev->GetRrc();

    remoteUeProse->SetImsi(remoteUeRrc->GetImsi());
    relayUeProse->SetImsi(relayUeRrc->GetImsi());

    uint32_t remoteUeL2Id = remoteUeRrc->GetSourceL2Id();
    uint32_t relayUeL2Id = relayUeRrc->GetSourceL2Id();

    remoteUeProse->SetL2Id(remoteUeL2Id);
    relayUeProse->SetL2Id(relayUeL2Id);

    NS_LOG_DEBUG("remote UE L2Id: " << remoteUeL2Id << " relay UE L2Id: " << relayUeL2Id);

    remoteUeSlInfo.m_dstL2Id = relayUeL2Id;
    remoteUeSlInfo.m_srcL2Id = remoteUeL2Id;
    relayUeSlInfo.m_dstL2Id = remoteUeL2Id;
    relayUeSlInfo.m_srcL2Id = relayUeL2Id;

    // Remote UE (Initiating UE)
    Simulator::Schedule(t,
                        &NrSlUeProse::AddDirectLinkConnection,
                        remoteUeProse,
                        remoteUeL2Id,
                        remoteUeIp,
                        relayUeL2Id,
                        true,
                        relayServiceCode,
                        remoteUeSlInfo);

    // Relay UE (Target UE)
    Simulator::Schedule(t,
                        &NrSlUeProse::AddDirectLinkConnection,
                        relayUeProse,
                        relayUeL2Id,
                        relayUeIp,
                        remoteUeL2Id,
                        false,
                        relayServiceCode,
                        relayUeSlInfo);
}

void
NrSlProseHelper::ConfigureL3UeToNetworkRelay(const NetDeviceContainer relayUeDevices,
                                             const std::set<uint32_t> relayServiceCodes,
                                             NrEpsBearer bearer,
                                             Ptr<NrEpcTft> tft)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_epcHelper, "dedicated EPS bearers cannot be set up when the EPC is not used");

    for (NetDeviceContainer::Iterator devIt = relayUeDevices.Begin(); devIt != relayUeDevices.End();
         ++devIt)
    {
        uint64_t imsi = (*devIt)->GetObject<NrUeNetDevice>()->GetImsi();
        Ptr<NrSlUeProse> prose = (*devIt)->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();

        // Set the relay service codes of the services the relay UE provides and the associated
        // configuration
        for (auto it = relayServiceCodes.begin(); it != relayServiceCodes.end(); ++it)
        {
            // Activate Eps dedicated bearer for relaying
            uint8_t relayDrbId = m_epcHelper->ActivateEpsBearer(*devIt, imsi, tft, bearer);
            NrSlUeProse::NrSlL3U2nServiceConfiguration config;
            config.relayDrbId = relayDrbId;
            prose->AddL3U2nRelayServiceConfiguration(*it, config);
        }
        // Set EPC Helper pointer on the ProSe layer, which is used to configure
        // data path in the EpcPgwApplication when a remote UE successfully connects to this relay
        // UE)
        prose->SetEpcHelper(m_epcHelper);
    }
}

void
NrSlProseHelper::StartDiscoveryApp(Ptr<NetDevice> ueDevice,
                                   uint32_t appCode,
                                   uint32_t dstL2Id,
                                   NrSlUeProse::DiscoveryRole role)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrSlUeProse> ueProse = ueDevice->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();
    ueProse->SetL2Id(ueDevice->GetObject<NrUeNetDevice>()->GetRrc()->GetSourceL2Id());
    ueProse->SetImsi(ueDevice->GetObject<NrUeNetDevice>()->GetRrc()->GetImsi());
    ueProse->AddDiscoveryApp(appCode, dstL2Id, role);
}

void
NrSlProseHelper::StopDiscoveryApp(Ptr<NetDevice> ueDevice,
                                  uint32_t appCode,
                                  NrSlUeProse::DiscoveryRole role)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrSlUeProse> ueProse = ueDevice->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();
    ueProse->RemoveDiscoveryApp(appCode, role);
}

void
NrSlProseHelper::StartDiscovery(Ptr<NetDevice> ueDevice,
                                std::list<uint32_t> appCodes,
                                std::list<uint32_t> dstL2Ids,
                                NrSlUeProse::DiscoveryRole role)
{
    NS_LOG_FUNCTION(this);

    std::list<uint32_t>::iterator dst = dstL2Ids.begin();
    for (std::list<uint32_t>::iterator it = appCodes.begin(); it != appCodes.end(); ++it)
    {
        StartDiscoveryApp(ueDevice, *it, *dst, role);
        ++dst;
    }
}

void
NrSlProseHelper::StopDiscovery(Ptr<NetDevice> ueDevice,
                               std::list<uint32_t> appCodes,
                               NrSlUeProse::DiscoveryRole role)
{
    NS_LOG_FUNCTION(this);

    std::map<uint32_t, NrSlUeProse::DiscoveryInfo>::iterator itInfo;
    for (std::list<uint32_t>::iterator it = appCodes.begin(); it != appCodes.end(); ++it)
    {
        StopDiscoveryApp(ueDevice, *it, role);
    }
}

void
NrSlProseHelper::StartRelayDiscovery(Ptr<NetDevice> ueDevice,
                                     uint32_t relayCode,
                                     uint32_t dstL2Id,
                                     NrSlUeProse::DiscoveryModel model,
                                     NrSlUeProse::DiscoveryRole role)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrSlUeProse> ueProse = ueDevice->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();
    Ptr<NrUeRrc> ueRrc = ueDevice->GetObject<NrUeNetDevice>()->GetRrc();
    uint32_t srcL2Id = ueRrc->GetSourceL2Id();
    ueProse->SetL2Id(srcL2Id);
    ueProse->SetImsi(ueRrc->GetImsi());
    ueProse->AddRelayDiscovery(relayCode, dstL2Id, model, role);
    ueProse->SetNetDevice(ueDevice);
}

void
NrSlProseHelper::StopRelayDiscovery(Ptr<NetDevice> ueDevice,
                                    uint32_t relayCode,
                                    NrSlUeProse::DiscoveryRole role)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrSlUeProse> ueProse = ueDevice->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();
    ueProse->RemoveRelayDiscovery(relayCode, role);
}

void
NrSlProseHelper::EnableDiscoveryTraces(void)
{
    NS_LOG_FUNCTION_NOARGS();
    Config::Connect(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/$ns3::NrSlUeProse/DiscoveryTrace",
        MakeBoundCallback(&NrSlDiscoveryTrace::DiscoveryTraceCallback, m_discoveryTrace));
}

void
NrSlProseHelper::StartRemoteRelayConnection(
    const NetDeviceContainer remoteDevices,
    const std::vector<Time> remoteTime,
    const NetDeviceContainer relayDevices,
    const std::vector<Time> relayTime,
    const std::vector<uint32_t> relayCodes,
    const std::vector<uint32_t> dstL2Ids,
    NrSlUeProse::DiscoveryModel discoveryModel,
    Ptr<NrSlUeProseRelaySelectionAlgorithm> selectionAlgorithm,
    Ptr<NrEpcTft> tft,
    NrEpsBearer bearer)
{
    NS_LOG_FUNCTION(this);

    // Start Discovery for relays/remotes
    for (uint32_t i = 0; i < relayDevices.GetN(); ++i)
    {
        Simulator::Schedule(relayTime[i],
                            &NrSlProseHelper::StartRelayDiscovery,
                            this,
                            relayDevices.Get(i),
                            relayCodes[i],
                            dstL2Ids[i],
                            discoveryModel,
                            NrSlUeProse::RelayUE);
    }

    for (uint32_t j = 0; j < remoteDevices.GetN(); ++j)
    {
        for (uint32_t k = 0; k < relayDevices.GetN(); ++k)
        {
            Simulator::Schedule(remoteTime[j],
                                &NrSlProseHelper::StartRelayDiscovery,
                                this,
                                remoteDevices.Get(j),
                                relayCodes[k],
                                dstL2Ids[k],
                                discoveryModel,
                                NrSlUeProse::RemoteUE);
        }
    }

    // Apply the configuration on the devices acting as relay UEs
    std::set<uint32_t> relayCodesSet;
    std::copy(relayCodes.begin(),
              relayCodes.end(),
              std::inserter(relayCodesSet, relayCodesSet.end()));
    ConfigureL3UeToNetworkRelay(relayDevices, relayCodesSet, bearer, tft);

    // Define relay selection algorithm and enable RSRP measurements for remote UEs
    for (uint32_t i = 0; i < remoteDevices.GetN(); ++i)
    {
        Ptr<NrSlUeProse> remoteProse =
            remoteDevices.Get(i)->GetObject<NrUeNetDevice>()->GetObject<NrSlUeProse>();
        remoteProse->SetRelaySelectionAlgorithm(selectionAlgorithm);
        Ptr<NrUeRrc> remoteRrc = remoteDevices.Get(i)->GetObject<NrUeNetDevice>()->GetRrc();
        remoteRrc->EnableUeSlRsrpMeasurements();
    }
}

void
NrSlProseHelper::EnableRelayTraces(void)
{
    std::cout << "EnableRelayTraces" << std::endl;
    NS_LOG_FUNCTION(this);
    // Relay discovery traces
    Config::Connect(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/$ns3::NrSlUeProse/RelayDiscoveryTrace",
        MakeBoundCallback(&NrSlRelayTrace::RelayDiscoveryTraceCallback, m_relayTrace));

    // Relay direct link communication establishment traces
    Config::Connect(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/$ns3::NrSlUeProse/RelaySelectionTrace",
        MakeBoundCallback(&NrSlRelayTrace::RelaySelectionTraceCallback, m_relayTrace));

    // Relay-Remote RSRP measurement
    Config::Connect("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/$ns3::NrSlUeProse/RelayRsrpTrace",
                    MakeBoundCallback(&NrSlRelayTrace::RelayRsrpTraceCallback, m_relayTrace));
}

void
NrSlProseHelper::InstallNrSlDiscoveryConfiguration(NetDeviceContainer relays,
                                                   NetDeviceContainer remotes,
                                                   const NrRrcSap::SlDiscConfigCommon discConfig)
{
    NS_LOG_FUNCTION(this);

    for (auto i = relays.Begin(); i != relays.End(); ++i)
    {
        Ptr<NetDevice> netRelayDev = *i;
        Ptr<NrUeNetDevice> nrRelayDev = netRelayDev->GetObject<NrUeNetDevice>();
        Ptr<NrUeRrc> nrRelayRrc = nrRelayDev->GetRrc();
        Ptr<NrSlUeRrc> nrSlRelayRrc = nrRelayRrc->GetObject<NrSlUeRrc>();
        nrSlRelayRrc->SetNrSlDiscoveryRelayConfiguration(discConfig.slRelayUeConfigCommon);
    }

    for (auto j = remotes.Begin(); j != remotes.End(); ++j)
    {
        Ptr<NetDevice> netRemoteDev = *j;
        Ptr<NrUeNetDevice> nrRemoteDev = netRemoteDev->GetObject<NrUeNetDevice>();
        Ptr<NrUeRrc> nrRemoteRrc = nrRemoteDev->GetRrc();
        Ptr<NrSlUeRrc> nrSlRemoteRrc = nrRemoteRrc->GetObject<NrSlUeRrc>();
        nrSlRemoteRrc->SetNrSlDiscoveryRemoteConfiguration(discConfig.slRemoteUeConfigCommon);
    }
}

} // namespace ns3
