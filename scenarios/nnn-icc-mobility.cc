/* -*- Mode:C++; c-file-style:"gnu"; -*- */
/*
 * nnn-icc-mobility.cc
 *
 * Copyright (c) 2015 Waseda University, Sato Laboratory
 * Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-icc-mobility.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  nnn-icc.mobility.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-icn-mobility.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

// Standard C++ modules
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <iostream>
#include <map>
#include <string>
#include <sys/time.h>
#include <vector>

// Random modules
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>

// ns3 modules
#include <ns3-dev/ns3/applications-module.h>
#include <ns3-dev/ns3/bridge-helper.h>
#include <ns3-dev/ns3/csma-module.h>
#include <ns3-dev/ns3/core-module.h>
#include <ns3-dev/ns3/mobility-module.h>
#include <ns3-dev/ns3/network-module.h>
#include <ns3-dev/ns3/point-to-point-module.h>
#include <ns3-dev/ns3/wifi-module.h>

// ndnSIM modules
#include <ns3-dev/ns3/ndnSIM-module.h>
#include <ns3-dev/ns3/ndnSIM/utils/tracers/ipv4-rate-l3-tracer.h>
#include <ns3-dev/ns3/ndnSIM/utils/tracers/ipv4-seqs-app-tracer.h>

// nnnSIM modules
#include "nnnSIM/nnnSIM-module.h"

// 3N Tracer
#include "nnnSIM/utils/tracers/nnn-l3-rate-tracer.h"
#include "nnnSIM/utils/tracers/nnn-l3-aggregate-tracer.h"
#include "nnnSIM/utils/tracers/nnn-app-delay-tracer.h"

using namespace ns3;
using namespace boost;
using namespace std;

namespace br = boost::random;

typedef struct timeval TIMER_TYPE;
#define TIMER_NOW(_t) gettimeofday (&_t,NULL);
#define TIMER_SECONDS(_t) ((double)(_t).tv_sec + (_t).tv_usec*1e-6)
#define TIMER_DIFF(_t1, _t2) (TIMER_SECONDS (_t1)-TIMER_SECONDS (_t2))

char scenario[250] = "MobilityICC";

NS_LOG_COMPONENT_DEFINE (scenario);

// Global information to use in callbacks
std::map<int, Address> last_seen_mac;
std::map<int, Address>::iterator last_seen_mac_it;
std::map<int, std::string> last_seen_ssid;
std::map<int, std::string>::iterator last_seen_ssid_it;

// Number generator
br::mt19937_64 gen;

// Obtains a random number from a uniform distribution between min and max.
// Must seed number generator to ensure randomness at runtime.
int obtain_Num(int min, int max)
{
  br::uniform_int_distribution<> dist(min, max);
  return dist(gen);
}

// Obtain a random double from a uniform distribution between min and max.
// Must seed number generator to ensure randomness at runtime.
double obtain_Num(double min, double max)
{
  br::uniform_real_distribution<> dist(min, max);
  return dist(gen);
}

// Function to change the SSID of a Node, depending on distance
void SetSSIDviaDistance(uint32_t mtId, uint32_t netId, std::map<std::string, Ptr<MobilityModel> > aps, bool use3N)
{
  NS_LOG_FUNCTION (mtId << netId);

  char configbuf[250];
  char buffer[250];

  // Obtain the global list of Nodes in the simulation
  NodeContainer global = NodeContainer::GetGlobal ();

  Ptr<MobilityModel> nodeMM = global.Get (mtId)->GetObject<MobilityModel> ();
  Ptr<nnn::ForwardingStrategy> nodeFW;

  if (use3N)
    nodeFW = global.Get (mtId)->GetObject<nnn::ForwardingStrategy> ();

  // This causes the device in mtId to change the SSID, forcing AP change
  sprintf(configbuf, "/NodeList/%d/DeviceList/%d/$ns3::WifiNetDevice/Mac/Ssid", mtId, netId);

  std::map<double, std::string> SsidDistance;

  // Iterate through the map of seen Ssids
  for (std::map<std::string, Ptr<MobilityModel> >::iterator ii=aps.begin(); ii!=aps.end(); ++ii)
    {
      // Calculate the distance from the AP to the node and save into the map
      SsidDistance[nodeMM->GetDistanceFrom((*ii).second)] = (*ii).first;
    }

  double distance = SsidDistance.begin()->first;
  std::string ssid(SsidDistance.begin()->second);

  sprintf(buffer, "Change to SSID %s at distance of %f", ssid.c_str(), distance);

  NS_LOG_INFO(buffer);

  // Search nodeNum in global map
  last_seen_ssid_it = last_seen_ssid.find (mtId);

  // Check to know what action the ForwardingStrategy for the node should do
  if (last_seen_ssid_it != last_seen_ssid.end ())
    {
      // We know that we have connected somewhere before
      if (last_seen_ssid[mtId] == ssid)
	{
	  NS_LOG_INFO ("Node " << mtId << " was already connected to AP with SSID " << ssid << ", no action taken");
	  SsidDistance.clear ();
	  return;
	}
      else
	{
	  NS_LOG_INFO ("Node " << mtId << " has changed AP to new " << ssid << " will disenroll");
	  last_seen_ssid[mtId] = ssid;

	  if (use3N)
	    {
	      // Force a disenroll procedure
	      Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Disenroll, nodeFW);
	    }
	}
    }
  else
    {
      NS_LOG_INFO ("First time Node " << mtId << " sees an address, saving " << ssid);
      // Save the Address
      last_seen_ssid[mtId] = ssid;

      if (use3N)
	{
	  // Now continue to do the enroll procedure
	  Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Enroll, nodeFW);
	}
    }

  // Because the map sorts by std:less, the first position has the lowest distance
  Config::Set (configbuf, SsidValue (ssid));

  // Empty the maps
  SsidDistance.clear ();
}

// Function to force a change to a particular Ssid
void SetForcedSSID (uint32_t mtId, uint32_t netId, Ssid ssid, bool use3N)
{
  NS_LOG_FUNCTION (mtId << netId);

  char configbuf[250];

  // Obtain the global list of Nodes in the simulation
  NodeContainer global = NodeContainer::GetGlobal ();

  if (use3N)
    {
      // Get the ForwardingStrategy for the node in question
      Ptr<nnn::ForwardingStrategy> nodeFW = global.Get (mtId)->GetObject<nnn::ForwardingStrategy> ();

      // Since we know we are changing SSID, we don't do any checks
      // Force a disenroll procedure
      Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Disenroll, nodeFW);
    }

  // This causes the device in mtId to change the SSID, forcing AP change
  sprintf(configbuf, "/NodeList/%d/DeviceList/%d/$ns3::WifiNetDevice/Mac/Ssid", mtId, netId);

  // Start the SSID change
  Config::Set (configbuf, SsidValue (ssid));
}

void ApAssociation (std::string context, const Mac48Address mac)
{
  NS_LOG_FUNCTION (context << mac);

  // Temporary container for context string
  std::vector<std::string> context_parts;

  // Use boost library to split the string
  boost::split(context_parts, context, boost::is_any_of ("//"));

  // Get the Node ID
  int nodeNum = std::atoi (context_parts[2].c_str());

  // Obtain the global list of Nodes in the simulation
  NodeContainer global = NodeContainer::GetGlobal ();

  // Get the node that fired the callback
  Ptr<Node> moving = global.Get (nodeNum);

  Ptr<nnn::ForwardingStrategy> nodeFW = moving->GetObject<nnn::ForwardingStrategy> ();

  // Convert MAC to Address
  Address nowAddr = mac.operator ns3::Address();

  // Search nodeNum in global map
  last_seen_mac_it = last_seen_mac.find(nodeNum);

  if (last_seen_mac_it != last_seen_mac.end ())
    {
      // We know that we have connected somewhere before
      if (last_seen_mac[nodeNum] == nowAddr)
	{
	  NS_LOG_INFO ("Node " << nodeNum << " was already connected to AP with " << nowAddr);
	  if (!nodeFW->Has3NName())
	    {
	      NS_LOG_INFO ("Node " << nodeNum << " still doesn't have a 3N name, reattempting");
	      Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Enroll, nodeFW);
	    }
	}
      else
	{
	  NS_LOG_INFO ("Node " << nodeNum << " has changed AP to " << nowAddr << " will reenroll");
	  last_seen_mac[nodeNum] = nowAddr;

	  Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Reenroll, nodeFW );
	}
    }
  else
    {
      NS_LOG_INFO ("First time Node " << nodeNum << " sees an address, saving " << nowAddr);
      // Save the Address
      last_seen_mac[nodeNum] = nowAddr;

      // Now continue to do the enroll procedure
      Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Enroll, nodeFW);
    }
}

int main (int argc, char *argv[])
{
  // These are our scenario arguments
  uint32_t sectors = 2;                         // Number of wireless sectors
  uint32_t mobile = 1;                          // Number of mobile terminals
  uint32_t servers = 1;                         // Number of servers in the network
  uint32_t aps = 2;                             // Number of APs in a sector
  uint32_t wnodes = 4;                          // Number of APs in the network
  uint32_t xaxis = 100;                         // Size of the X axis
  uint32_t yaxis = 100;                         // Size of the Y axis
  double sec = 0.0;                             // Movement start
  double speed = 1.4;                           // Speed in X axis
  bool traceFiles = false;                      // Tells to run the simulation with traceFiles
  bool smart = false;                           // Tells to run the simulation with SmartFlooding
  bool bestr = false;                           // Tells to run the simulation with BestRoute
  char results[250] = "results";                // Directory to place results
  double MBps = 0.15;                           // MB/s data rate desired for applications
  // 0.15 MB/s equals to 1.2 Mb/s which is the bitrate for
  // 480p livestream video according to Adobe
  int contentSize = -1;                         // Size of content to be retrieved
  int maxSeq = -1;                              // Maximum number of Data packets to request
  double retxtime = 0.05;                       // How frequent Interest retransmission timeouts should be checked (seconds)
  int csSize = 10000000;                        // How big the Content Store should be
  bool use3N = false;                           // Flags use of 3N based scenario
  bool useNDN = false;                          // Flags use of NDN based scenario
  bool producer = false;                        // Tells to run the simulation with the Provider moving
  double initialWait = 10;                      // How much time we should wait to start the simulation (Mostly to deal with 3N naming)

  // Variable for buffer
  char buffer[250];

  CommandLine cmd;
  cmd.AddValue ("results", "Directory to place results", results);
  cmd.AddValue ("trace", "Enable trace files", traceFiles);
  cmd.AddValue ("smart", "Enable SmartFlooding forwarding", smart);
  cmd.AddValue ("bestr", "Enable BestRoute forwarding", bestr);
  cmd.AddValue ("csSize", "Number of Interests a Content Store can maintain", csSize);
  cmd.AddValue ("mbps", "Data transmission rate for App in MBps", MBps);
  cmd.AddValue ("size", "Content size in MB (-1 is for no limit)", contentSize);
  cmd.AddValue ("retx", "How frequent Interest retransmission timeouts should be checked in seconds", retxtime);
  cmd.AddValue ("speed", "How fast in m/s the mobile node should go in X axis", speed);
  cmd.AddValue ("3n", "Uses 3N scenario", use3N);
  cmd.AddValue ("ndn", "Uses NDN scenario", useNDN);
  cmd.AddValue ("producer", "Makes the scenario with the Producer moving. By default it is the Consumer", producer);
  cmd.Parse (argc,argv);

  if (! (use3N || useNDN))
    {
      cerr << "ERROR: Must choose a network type to test!" << endl;
      return 1;
    }

  if (use3N && useNDN)
    {
      cerr << "ERROR: Cannot run 3N and NDN scenarios at the same time! Pick one!" << endl;
      return 1;
    }

  double initialPos = -speed*initialWait;                            // Where to put the initial node
  double appEndTime = initialWait + (400 / speed);               // At what moment to end the applications
  double endTime = initialWait + (400 / speed) + initialWait;    // Number of seconds to run the simulation

  // What the NDN Data packet payload size is fixed to 1024 bytes
  uint32_t payLoadsize = 1024;

  // Give the content size, find out how many sequence numbers are necessary
  if (contentSize > 0)
    {
      maxSeq = 1 + (((contentSize*1000000) - 1) / payLoadsize);
    }

  // How many Interests/second a producer creates
  double intFreq = (MBps * 1000000) / payLoadsize;

  NS_LOG_INFO ("------Creating nodes------");
  // Node definitions for mobile terminals (consumers)
  NodeContainer mobileTerminalContainer;
  mobileTerminalContainer.Create(mobile);

  std::vector<uint32_t> mobileNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < mobile; i++)
    {
      mobileNodeIds.push_back(mobileTerminalContainer.Get (i)->GetId ());
    }

  // Central Nodes
  NodeContainer centralContainer;
  centralContainer.Create (sectors);

  // Wireless access Nodes
  NodeContainer wirelessContainer;
  wirelessContainer.Create (wnodes);

  // Separate the wireless nodes into sector specific containers
  std::vector<NodeContainer> sectorNodes;

  for (int i = 0; i < sectors; i++)
    {
      NodeContainer wireless;
      for (int j = i*aps; j < aps + i*aps; j++)
	{
	  wireless.Add(wirelessContainer.Get (j));
	}
      sectorNodes.push_back(wireless);
    }

  // Container for all NDN capable nodes not in first level
  NodeContainer lowerNdnNodes;
  lowerNdnNodes.Add (centralContainer);
  lowerNdnNodes.Add (wirelessContainer);

  // Container for server (producer) nodes
  NodeContainer serverNodes;
  serverNodes.Create (servers);

  // Container for all NDN capable nodes
  NodeContainer allNdnNodes;
  allNdnNodes.Add (serverNodes);
  allNdnNodes.Add (centralContainer);
  allNdnNodes.Add (wirelessContainer);

  std::vector<uint32_t> serverNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < servers; i++)
    {
      serverNodeIds.push_back(serverNodes.Get (i)->GetId ());
    }

  // Container for all nodes without NDN specific capabilities
  NodeContainer allUserNodes;
  allUserNodes.Add (mobileTerminalContainer);

  // Make sure to seed our random
  gen.seed (std::time (0) + (long long)getpid () << 32);

  NS_LOG_INFO ("------Placing Central nodes-------");
  MobilityHelper centralStations;

  Ptr<ListPositionAllocator> initialCenter = CreateObject<ListPositionAllocator> ();

  int add = 0;
  for (int i = 0; i < sectors; i++)
    {
      Vector pos (100 + add, 100, 0.0);
      initialCenter->Add (pos);
      add += 200;
    }

  centralStations.SetPositionAllocator(initialCenter);
  centralStations.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  centralStations.Install(centralContainer);

  NS_LOG_INFO ("------Placing wireless access nodes------");
  MobilityHelper wirelessStations;

  Ptr<ListPositionAllocator> initialWireless = CreateObject<ListPositionAllocator> ();

  add = 0;
  for (int i = 0; i < wnodes; i++)
    {
      Vector pos (50 + add, 150, 0.0);
      initialWireless->Add (pos);
      add += 100;
    }

  wirelessStations.SetPositionAllocator(initialWireless);
  wirelessStations.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  wirelessStations.Install(wirelessContainer);

  NS_LOG_INFO ("------Placing mobile node and determining direction and speed------");
  MobilityHelper mobileStations;

  Ptr<ListPositionAllocator> initialMobile = CreateObject<ListPositionAllocator> ();

  add = 0;
  for (int i = 0; i < mobile; i++)
    {
      Vector pos (initialPos, 200 + add, 0.0);
      initialMobile->Add (pos);
      add += 10;
    }

  mobileStations.SetPositionAllocator(initialMobile);
  mobileStations.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobileStations.Install(mobileTerminalContainer);

  // Set the constant speed for X axis
  mobileTerminalContainer.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (speed, 0, 0));

  // Connect Wireless Nodes to central nodes
  // Because the simulation is using Wifi, PtP connections are 100Mbps
  // with 5ms delay
  NS_LOG_INFO("------Connecting Central nodes to wireless access nodes------");

  vector <NetDeviceContainer> ptpWLANCenterDevices;

  // For 3N scenarios, we have to use similar P2P devices of type FlexPointToPoint
  nnn::FlexPointToPointHelper p2p_100mbps5ms;
  p2p_100mbps5ms.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  p2p_100mbps5ms.SetChannelAttribute ("Delay", StringValue ("1ms"));

  for (int i = 0; i < sectors; i++)
    {
      NetDeviceContainer ptpWirelessCenterDevices;

      for (int j = 0; j < aps; j++)
	{
	  ptpWirelessCenterDevices.Add (p2p_100mbps5ms.Install (centralContainer.Get (i), sectorNodes[i].Get (j)));
	}

      ptpWLANCenterDevices.push_back (ptpWirelessCenterDevices);
    }

  // Connect the server to the central sector nodes
  NetDeviceContainer ptpServerlowerNdnDevices;
  for (int i = 0; i < serverNodes.GetN (); i++)
    {
      for (int j = 0; j < sectors; j++)
	{
	  ptpServerlowerNdnDevices.Add (p2p_100mbps5ms.Install (serverNodes.Get(i), centralContainer.Get (j)));
	}
    }

  NS_LOG_INFO ("------Creating Wireless cards------");

  // Use the Wifi Helper to define the wireless interfaces for APs
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
  wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

  // All interfaces are placed on the same channel. Makes AP changes easy. Might
  // have to be reconsidered for multiple mobile nodes
  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel (wifiChannel.Create ());
  wifiPhyHelper.Set("TxPowerStart", DoubleValue(16.0206));
  wifiPhyHelper.Set("TxPowerEnd", DoubleValue(16.0206));

  // Add a simple no QoS based card to the Wifi interfaces
  NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default ();

  // Create SSIDs for all the APs
  std::vector<Ssid> ssidV;

  NS_LOG_INFO ("------Creating ssids for wireless cards------");

  // We store the Wifi AP mobility models in a map, ordered by the ssid string. Will be easier to manage when
  // calling the modified StaMApWifiMac
  std::map<std::string, Ptr<MobilityModel> > apTerminalMobility;

  for (int i = 0; i < wnodes; i++)
    {
      // Temporary string containing our SSID
      std::string ssidtmp("ap-" + boost::lexical_cast<std::string>(i));

      // Push the newly created SSID into a vector
      ssidV.push_back (Ssid (ssidtmp));

      // Get the mobility model for wnode i
      Ptr<MobilityModel> tmp = (wirelessContainer.Get (i))->GetObject<MobilityModel> ();

      // Store the information into our map
      apTerminalMobility[ssidtmp] = tmp;
    }

  NS_LOG_INFO ("------Assigning mobile terminal wireless cards------");

  NS_LOG_INFO ("Assigning AP wireless cards");
  std::vector<NetDeviceContainer> wifiAPNetDevices;
  for (int i = 0; i < wnodes; i++)
    {
      wifiMacHelper.SetType ("ns3::ApWifiMac",
			     "Ssid", SsidValue (ssidV[i]),
			     "BeaconGeneration", BooleanValue (true),
			     "BeaconInterval", TimeValue (Seconds (0.102)));

      wifiAPNetDevices.push_back (wifi.Install (wifiPhyHelper, wifiMacHelper, wirelessContainer.Get (i)));
    }

  // Create a Wifi station with a modified Station MAC.
  wifiMacHelper.SetType("ns3::StaWifiMac",
			"Ssid", SsidValue (ssidV[0]),
			"ActiveProbing", BooleanValue (true));

  NetDeviceContainer wifiMTNetDevices = wifi.Install (wifiPhyHelper, wifiMacHelper, mobileTerminalContainer);

  char routeType[250];
  char moving[250];
  char speedchar[250];

  sprintf(buffer, "%d", csSize);

  sprintf (speedchar, "%.2f", speed);

  std::string speedstr (speedchar);

  std::replace(speedstr.begin (), speedstr.end (), '.', '-');

  std::string cs (buffer);

  // Now install content stores and the rest on the middle node. Leave
  // out clients and the mobile node
  if (useNDN)
    {
      NS_LOG_INFO ("------Installing NDN stack on routers------");
      ndn::StackHelper ndnHelperRouters;

      // Decide what Forwarding strategy to use depending on user command line input
      if (smart) {
	  sprintf(routeType, "%s", "smart");
	  NS_LOG_INFO ("NDN Utilizing SmartFlooding");
	  ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::SmartFlooding::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
      } else if (bestr) {
	  sprintf(routeType, "%s", "bestr");
	  NS_LOG_INFO ("NDN Utilizing BestRoute");
	  ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::BestRoute::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
      } else {
	  sprintf(routeType, "%s", "flood");
	  NS_LOG_INFO ("NDN Utilizing Flooding");
	  ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::Flooding::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
      }

      // Set the Content Stores
      NS_LOG_INFO ("Setting Content Store size: " << buffer);

      ndnHelperRouters.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", cs);
      ndnHelperRouters.SetDefaultRoutes (true);
      // Install on ICN capable routers
      ndnHelperRouters.Install (allNdnNodes);

      NS_LOG_INFO ("------Installing NDN stack on user nodes ------");
      // Create a NDN stack for the clients and mobile node
      ndn::StackHelper ndnHelperUsers;
      // These nodes have only one interface, so BestRoute forwarding makes sense
      ndnHelperUsers.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
      // No Content Stores are installed on these machines
      ndnHelperUsers.SetContentStore ("ns3::ndn::cs::Nocache");
      ndnHelperUsers.SetDefaultRoutes (true);
      ndnHelperUsers.Install (mobileTerminalContainer);

      NS_LOG_INFO ("------Installing NDN Producer Application------");

      NS_LOG_INFO ("Producer Payload size: " << payLoadsize);

      // Create the producer on the mobile node
      ndn::AppHelper producerHelper ("ns3::ndn::Producer");
      producerHelper.SetPrefix ("/waseda/sato");
      producerHelper.SetAttribute ("StopTime", TimeValue (Seconds(appEndTime)));
      // Payload size is in bytes
      producerHelper.SetAttribute ("PayloadSize", UintegerValue(payLoadsize));

      if (producer)
	{
	  producerHelper.Install (mobileTerminalContainer);
	}
      else
	{
	  producerHelper.Install (serverNodes);
	}

      NS_LOG_INFO ("------Installing NDN Consumer Application------");

      NS_LOG_INFO ("Consumer Interests/Second frequency: " << intFreq);

      // Create the consumer on the randomly selected node
      ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
      consumerHelper.SetPrefix ("/waseda/sato");
      consumerHelper.SetAttribute ("Frequency", DoubleValue (intFreq));
      consumerHelper.SetAttribute ("StartTime", TimeValue (Seconds(initialWait)));
      consumerHelper.SetAttribute ("StopTime", TimeValue (Seconds(appEndTime)));
      consumerHelper.SetAttribute ("RetxTimer", TimeValue (Seconds(retxtime)));
      if (maxSeq > 0)
	consumerHelper.SetAttribute ("MaxSeq", IntegerValue(maxSeq));

      if (producer)
	{
	  sprintf (moving, "%s", "prod");
	  consumerHelper.Install (serverNodes);
	}
      else
	{
	  sprintf (moving, "%s", "con");
	  consumerHelper.Install (mobileTerminalContainer);
	}
    }

  if (use3N)
    {
      sprintf(routeType, "%s", "3n");
      NS_LOG_INFO ("------ Installing Server 3N stack ------");
      // Stack for nodes that use fixed connections
      nnn::NNNStackHelper ServerStack;

      sprintf(buffer, "%ds", (int)endTime+35);

      std::string timeStr (buffer);

      sprintf(buffer, "%dms", (int)(retxtime*1000));

      std::string retxch (buffer);

      NS_LOG_INFO ("Setting lease time to: " << timeStr);
      NS_LOG_INFO ("Setting retransmission times to: " << retxch);

      // Set the Forwarding Strategy and have it have a 3N name lease time of 300 seconds
      ServerStack.SetForwardingStrategy ("ns3::nnn::ForwardingStrategy", "3NLeasetime", timeStr, "RetxTimer", retxch);

      NS_LOG_INFO ("Setting Content Store size: " << cs);

      // Set the Content Store for the primary stack, Normal LRU ContentStore
      ServerStack.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", cs);
      // Set the FIB default routes
      ServerStack.SetDefaultRoutes (true);
      // Install the stack
      ServerStack.Install (serverNodes);

      // Create the initial 3N name
      Ptr<nnn::NNNAddress> firstName = Create <nnn::NNNAddress> ("a");
      // Get the ForwardingStrategy object from the node
      Ptr<nnn::ForwardingStrategy> fwServer = serverNodes.Get (0)->GetObject<nnn::ForwardingStrategy> ();
      // Give a 3N name for the first AP - ensure it is longer than the actual simulation
      fwServer->SetNode3NName (firstName, Seconds (endTime + 5), true);

      ///////////////////////////////////////////////////////

      NS_LOG_INFO ("------ Installing Router 3N stack ------");
      // Stack for a Node that is given a node name
      nnn::NNNStackHelper APStack;

      // Set the Forwarding Strategy and have it have a 3N name lease time of 300 seconds
      APStack.SetForwardingStrategy ("ns3::nnn::ForwardingStrategy", "3NLeasetime", timeStr, "RetxTimer", retxch);
      // Set the Content Store for the primary stack, Normal LRU ContentStore

      APStack.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", cs);
      // Set the FIB default routes
      APStack.SetDefaultRoutes (true);
      // Install the stack
      APStack.Install (lowerNdnNodes);

      // Vector of all forwarding strategies for APs
      std::vector<Ptr<nnn::ForwardingStrategy> > fwAPs;

      // Force Enrollment for the APs
      for (int i = 0; i < lowerNdnNodes.GetN (); i++)
	{
	  fwAPs.push_back (lowerNdnNodes.Get (i)->GetObject<nnn::ForwardingStrategy> ());
	}

      for (int i = 0; i < lowerNdnNodes.GetN () ; i++)
	{
	  Simulator::Schedule(Seconds (0), &nnn::ForwardingStrategy::Enroll, fwAPs[i]);
	}

      ///////////////////////////////////////////////////////

      NS_LOG_INFO ("------ Installing mobile 3N stack ------");
      // Stack for nodes that are mobile;
      nnn::NNNStackHelper mobileStack;
      // No Content Store for mobile stack
      mobileStack.SetContentStore ("ns3::ndn::cs::Nocache");
      // Do not produce 3N names for these nodes
      mobileStack.SetForwardingStrategy ("ns3::nnn::ForwardingStrategy", "Produce3Nnames", "false", "RetxTimer", retxch);
      // Set the FIB default routes
      mobileStack.SetDefaultRoutes (true);
      // Install the stack
      mobileStack.Install (mobileTerminalContainer);

      // Create the applications
      NS_LOG_INFO ("------ Installing 3N Producer Application------ ");
      NS_LOG_INFO ("Producer Payload size: " << payLoadsize);

      // Create the producer on the AP node - same as in NDN
      nnn::AppHelper producerHelper ("ns3::nnn::Producer");
      producerHelper.SetPrefix ("/waseda/sato");
      // Payload size is in bytes
      producerHelper.SetAttribute ("PayloadSize", UintegerValue(payLoadsize));
      producerHelper.SetAttribute ("StopTime", TimeValue (Seconds(appEndTime)));

      // Install producer on AP
      if (producer)
	{
	  producerHelper.SetAttribute ("IsMobile", BooleanValue (true));
	  producerHelper.Install (mobileTerminalContainer);
	}
      else
	{
	  producerHelper.Install (serverNodes);
	}

      NS_LOG_INFO ("------ Installing 3N Consumer Application------ ");
      NS_LOG_INFO ("Consumer Interests/Second frequency: " << intFreq);

      // Create the consumer node on the mobile node - same as in NDN
      nnn::AppHelper consumerHelper ("ns3::nnn::ConsumerCbr");
      consumerHelper.SetPrefix ("/waseda/sato");
      consumerHelper.SetAttribute ("Frequency", DoubleValue (intFreq));
      consumerHelper.SetAttribute ("StartTime", TimeValue (Seconds(initialWait)));
      consumerHelper.SetAttribute ("StopTime", TimeValue (Seconds(appEndTime)));
      consumerHelper.SetAttribute ("RetxTimer", TimeValue (Seconds(retxtime)));

      if (producer)
	{
	  sprintf (moving, "%s", "prod");
	  consumerHelper.Install (serverNodes);
	}
      else
	{
	  sprintf (moving, "%s", "con");
	  consumerHelper.SetAttribute ("IsMobile", BooleanValue (true));
	  consumerHelper.Install (mobileTerminalContainer);
	}
    }

  NS_LOG_INFO("Ending time! " <<  endTime);

  // If the variable is set, print the trace files
  if (traceFiles) {
      // Filename
      char filename[250];

      // File ID
      char fileId[250];

      // Create the file identifier
      sprintf(fileId, "%s-%s-%s-%02d-%03d-%03d.txt", routeType, moving, speedstr.c_str(), mobile, servers, wnodes);

      sprintf(filename, "%s/%s-clients-%s", results, scenario, fileId);

      std::ofstream clientFile;

      clientFile.open (filename);
      for (int i = 0; i < mobileNodeIds.size(); i++)
	{
	  clientFile << mobileNodeIds[i] << std::endl;
	}

      clientFile.close();

      // Print server nodes to file
      sprintf(filename, "%s/%s-servers-%s", results, scenario, fileId);

      std::ofstream serverFile;

      serverFile.open (filename);
      for (int i = 0; i < serverNodeIds.size(); i++)
	{
	  serverFile << serverNodeIds[i] << std::endl;
	}

      serverFile.close();

      NS_LOG_INFO ("Installing tracers");
      if (useNDN)
	{
	  NS_LOG_INFO ("Installing NDN tracers");
	  // NDN Aggregate tracer
	  sprintf (filename, "%s/%s-aggregate-trace-%s", results, scenario, fileId);
	  ndn::L3AggregateTracer::InstallAll(filename, Seconds (1.0));

	  // NDN L3 tracer
	  sprintf (filename, "%s/%s-rate-trace-%s", results, scenario, fileId);
	  ndn::L3RateTracer::InstallAll (filename, Seconds (1.0));

	  // NDN App Tracer
	  sprintf (filename, "%s/%s-app-delays-%s", results, scenario, fileId);
	  ndn::AppDelayTracer::InstallAll (filename);

	  // L2 Drop rate tracer
	  sprintf (filename, "%s/%s-drop-trace-%s", results, scenario, fileId);
	  L2RateTracer::InstallAll (filename, Seconds (0.5));

	  // Content Store tracer
	  sprintf (filename, "%s/%s-cs-trace-%s", results, scenario, fileId);
	  ndn::CsTracer::InstallAll (filename, Seconds (1));
	}

      if (use3N)
	{
	  NS_LOG_INFO ("Installing 3N tracers");
	  // 3N App Tracer
	  sprintf (filename, "%s/%s-app-delays-%s", results, scenario, fileId);
	  nnn::AppDelayTracer::InstallAll (filename);

	  // 3N Aggregate tracer
	  sprintf (filename, "%s/%s-aggregate-trace-%s", results, scenario, fileId);
	  nnn::L3AggregateTracer::InstallAll(filename, Seconds (1.0));

	  // 3N L3 tracer
	  sprintf (filename, "%s/%s-rate-trace-%s", results, scenario, fileId);
	  nnn::L3RateTracer::InstallAll (filename, Seconds (1.0));

	  // Content Store tracer
	  sprintf (filename, "%s/%s-cs-trace-%s", results, scenario, fileId);
	  ndn::CsTracer::InstallAll (filename, Seconds (1));
	}
  }

  // The AP association callbacks are only useful in 3N setting
  if (use3N)
    {
      NS_LOG_INFO ("------ Creating the AP Association Callbacks ------");
      char configbuf[250];

      for (int i = 0; i < mobileNodeIds.size (); i++)
	{
	  NS_LOG_INFO ("Establishing callbacks for Node " << mobileNodeIds[i] << " wireless device 0");
	  sprintf (configbuf, "/NodeList/%d/DeviceList/%d/$ns3::WifiNetDevice/Mac/$ns3::StaWifiMac/Assoc", mobileNodeIds[i], 0);
	  Config::Connect(configbuf, MakeCallback (&ApAssociation));
	}
    }

  NS_LOG_INFO ("------Scheduling events - SSID changes------");

  // Schedule AP Changes
  double apsec = 0.0;
  // How often should the AP check it's distance
  double checkTime = 100.0 / speed;
  double j = apsec;

  while ( j < endTime)
    {
      for (int i = 0; i < mobile; i++)
	{
	  NS_LOG_INFO ("Scheduling SSID change for Node" << mobileNodeIds[i] << " at " << j);
	  Simulator::Schedule (Seconds(j), &SetSSIDviaDistance, mobileNodeIds[i], 0, apTerminalMobility, use3N);
	}

      j += checkTime;
    }

  NS_LOG_INFO ("------Ready for execution!------");

  Simulator::Stop (Seconds (endTime));
  Simulator::Run ();
  Simulator::Destroy ();
}
