/* -*- Mode:C++; c-file-style:"gnu"; -*- */
/*
 * nnn-2-app-test.cc
 *
 * Copyright (c) 2014 Waseda University, Sato Laboratory
 * Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 * Special thanks to University of Washington for initial templates
 *
 *  nnn-2-app-test.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  nnn-2-app-test.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-2-app-test.cc.  If not, see <http://www.gnu.org/licenses/>.
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
#include <ns3-dev/ns3/config.h>
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

char scenario[250] = "3N2Test";

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
	NS_LOG_INFO ("Node " << mtId << " was already connected to AP with SSID " << ssid);
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
	      // nodeFW->Enroll
	      Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Enroll, nodeFW);
	    }
	}
      else
	{
	  NS_LOG_INFO ("Node " << nodeNum << " has changed AP to " << nowAddr << " will reenroll");
	  last_seen_mac[nodeNum] = nowAddr;

	  // nodeFW->Reenroll ();
	  Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Reenroll, nodeFW );
	}
    }
  else
    {
      NS_LOG_INFO ("First time Node " << nodeNum << " sees an address, saving " << nowAddr);
      // Save the Address
      last_seen_mac[nodeNum] = nowAddr;

      // Now continue to do the enroll procedure
      //nodeFW->Enroll ();
      Simulator::Schedule (Seconds (0), &nnn::ForwardingStrategy::Enroll, nodeFW);
    }
}


int main (int argc, char *argv[])
{
  // These are our scenario arguments
  uint32_t aps = 2;                             // Number of wireless access nodes in a sector
  uint32_t mobile = 1;                          // Number of mobile terminals
  uint32_t servers = 1;                         // Number of servers in the network
  uint32_t xaxis = 100;                         // Size of the X axis
  uint32_t yaxis = 100;                         // Size of the Y axis
  double sec = 0.0;                             // Movement start
  bool traceFiles = false;                      // Tells to run the simulation with traceFiles
  bool useMobility = false;                     // Tells to run the simulation with mobility support
  bool producer = false;                        // Tells to run the simulation with the Provider moving
  char results[250] = "results";                // Directory to place results
  double endTime = 100;                         // Number of seconds to run the simulation
  double MBps = 0.15;                           // MB/s data rate desired for applications
  double retxtime = 0.05;                       // How frequent Interest retransmission timeouts should be checked (seconds)
  bool use3N = false;                           // Flags use of 3N based scenario
  bool useNDN = false;                          // Flags use of NDN based scenario

  // Variable for buffer
  char buffer[250];

  CommandLine cmd;
  cmd.AddValue ("mobile", "Number of mobile terminals in simulation", mobile);
  cmd.AddValue ("servers", "Number of servers in the simulation", servers);
  cmd.AddValue ("results", "Directory to place results", results);
  cmd.AddValue ("start", "Starting second", sec);
  cmd.AddValue ("trace", "Enable trace files", traceFiles);
  cmd.AddValue ("mobility", "Enable mobility support", useMobility);
  cmd.AddValue ("producer", "Makes the scenario with the Producer moving. By default it is the Consumer", producer);
  cmd.AddValue ("mbps", "Data transmission rate for App in MBps", MBps);
  cmd.AddValue ("retx", "How frequent Interest retransmission timeouts should be checked in seconds", retxtime);
  cmd.AddValue ("3n", "Uses 3N scenario", use3N);
  cmd.AddValue ("ndn", "Uses NDN scenario", useNDN);
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

  // What the NDN Data packet payload size is fixed to 1024 bytes
  uint32_t payLoadsize = 1024;

  // How many Interests/second a producer creates
  double intFreq = (MBps * 1000000) / payLoadsize;

  NS_LOG_INFO ("------Creating nodes------");

  NS_LOG_INFO ("Creating " << mobile << " mobile nodes");
  // Node definitions for mobile terminals
  NodeContainer MNContainer;
  MNContainer.Create(mobile);

  std::vector<uint32_t> mobileNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < mobile; i++)
    {
      mobileNodeIds.push_back(MNContainer.Get (i)->GetId ());
    }

  NS_LOG_INFO ("Creating " << aps << " AP nodes");
  // Wireless access Nodes
  NodeContainer APContainer;
  APContainer.Create (aps);

  std::vector<uint32_t> wirelessAPNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < aps; i++)
    {
      wirelessAPNodeIds.push_back(APContainer.Get (i)->GetId ());
    }

  NS_LOG_INFO ("Creating " << servers << " server nodes");
  NodeContainer ServerContainer;
  ServerContainer.Create (servers);

  std::vector<uint32_t> serverNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < servers; i++)
    {
      serverNodeIds.push_back(ServerContainer.Get (i)->GetId ());
    }

  // Container for all 3N capable nodes
  NodeContainer all3NNodes;
  all3NNodes.Add (MNContainer);
  all3NNodes.Add (APContainer);
  all3NNodes.Add (ServerContainer);

  // Make sure to seed our random
  gen.seed (std::time (0) + (long long)getpid () << 32);

  NS_LOG_INFO ("------Placing Server nodes-------");
  int added = 1;

  // It seems that if the server has no wireless cards, adding a MobilityHelper
  // crashes the flow of the simulator. I have no idea why this happens. Will have
  // to look more into this.
//  MobilityHelper ServerStations;
//
//  Ptr<ListPositionAllocator> ServerCenter = CreateObject<ListPositionAllocator> ();
//
//  for (int i = 0; i < servers; i++)
//    {
//      Vector pos (50 * added, 50 * added, 0.0);
//      ServerCenter->Add (pos);
//      added += 1;
//    }
//
//  ServerStations.SetPositionAllocator(ServerCenter);
//  ServerStations.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//  ServerStations.Install (ServerContainer);

  NS_LOG_INFO ("------Placing AP nodes-------");
  MobilityHelper APStations;

  Ptr<ListPositionAllocator> APpositions = CreateObject<ListPositionAllocator> ();

  added = 0;
  for (int i = 0; i < aps; i++)
    {
      Vector pos (0.0 + added, 0.0, 0.0);
      APpositions->Add (pos);
      added += 100;
    }

  APStations.SetPositionAllocator(APpositions);
  APStations.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  APStations.Install(APContainer);

  NS_LOG_INFO ("------Placing Mobile nodes-------");
  sprintf(buffer, "0|%d|0|%d", xaxis, yaxis);

  string bounds = string(buffer);

  MobilityHelper MNStations;

  MNStations.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Mode", StringValue ("Distance"),
                               "Distance", StringValue ("500"),
                               "Bounds", StringValue (bounds));

  MNStations.Install(MNContainer);

  NS_LOG_INFO ("------Creating Wireless cards------");

  // Use the Wifi Helper to define the wireless interfaces for APs
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
  wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

  // All interfaces are placed on the same channel. Makes AP changes easy.
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

  for (int i = 0; i < aps; i++)
    {
      // Temporary string containing our SSID
      std::string ssidtmp("ap-" + boost::lexical_cast<std::string>(i));

      // Push the newly created SSID into a vector
      ssidV.push_back (Ssid (ssidtmp));

      // Get the mobility model for wnode i
      Ptr<MobilityModel> tmp = (APContainer.Get (i))->GetObject<MobilityModel> ();

      // Store the information into our map
      apTerminalMobility[ssidtmp] = tmp;
    }

  NS_LOG_INFO ("------Assigning mobile terminal wireless cards------");

  NS_LOG_INFO ("Assigning AP wireless cards");
  std::vector<NetDeviceContainer> wifiAPNetDevices;
  for (int i = 0; i < aps; i++)
    {
      Ptr<Node> tmp = APContainer.Get (i);
      NS_LOG_INFO ("Installing wireless AP card on node " << tmp->GetId () << " with ssid " << ssidV[i]);
      wifiMacHelper.SetType ("ns3::ApWifiMac",
                             "Ssid", SsidValue (ssidV[i]),
                             "BeaconGeneration", BooleanValue (true),
                             "BeaconInterval", TimeValue (Seconds (0.102)));

      wifiAPNetDevices.push_back (wifi.Install (wifiPhyHelper, wifiMacHelper, tmp));
    }

  Ptr<Node> tmp = MNContainer.Get (0);
  NS_LOG_INFO ("Installing wireless station card on node " << tmp-> GetId () << " initial ssid " << ssidV[0]);
  // Create a Wifi for the mobile node
  wifiMacHelper.SetType("ns3::StaWifiMac",
                        "Ssid", SsidValue (ssidV[0]),
                        "ActiveProbing", BooleanValue (true));

  NetDeviceContainer wifiMTNetDevices = wifi.Install (wifiPhyHelper, wifiMacHelper, tmp);

  NS_LOG_INFO ("------Creating Point-to-Point connections ------");

  nnn::FlexPointToPointHelper p2p_100mbps5ms;
  //PointToPointHelper p2p_100mbps5ms;
  p2p_100mbps5ms.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  p2p_100mbps5ms.SetChannelAttribute ("Delay", StringValue ("1ms"));

  vector <NetDeviceContainer> ptpWLANCenterDevices;

  for (int i = 0; i < servers; i++)
    {
      NetDeviceContainer ptpWirelessCenterDevices;

      for (int j = 0; j < aps; j++)
	{
	  Ptr<Node> tmp1 = ServerContainer.Get (i);
	  Ptr<Node> tmp2 = APContainer.Get (j);
	  NS_LOG_INFO ("Installing PtoP link from " << tmp1->GetId () << " to " << tmp2->GetId ());
	  ptpWirelessCenterDevices.Add (p2p_100mbps5ms.Install (tmp1, tmp2));
	}

      ptpWLANCenterDevices.push_back (ptpWirelessCenterDevices);
    }

  char routeType[250];
  char moving[250];
  char mob[250];

  // Now install content stores and the rest on the middle node. Leave
  // out clients and the mobile node
  if (useNDN)
    {
      NS_LOG_INFO ("------Installing NDN stack on routers------");
      ndn::StackHelper ndnHelperRouters;

      // Decide what Forwarding strategy to use depending on user command line input

      sprintf(routeType, "%s", "smart");
      sprintf(mob, "%s", "no");
      NS_LOG_INFO ("NDN Utilizing SmartFlooding");
      ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::SmartFlooding::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
      ndnHelperRouters.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "1000000");
      ndnHelperRouters.SetDefaultRoutes (true);
      // Install on ICN capable routers
      ndnHelperRouters.Install (APContainer);
      ndnHelperRouters.Install (ServerContainer);

      NS_LOG_INFO ("------Installing NDN stack on user nodes ------");
      // Create a NDN stack for the clients and mobile node
      ndn::StackHelper ndnHelperUsers;
      // These nodes have only one interface, so BestRoute forwarding makes sense
      ndnHelperUsers.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
      // No Content Stores are installed on these machines
      ndnHelperUsers.SetContentStore ("ns3::ndn::cs::Nocache");
      ndnHelperUsers.SetDefaultRoutes (true);
      ndnHelperUsers.Install (MNContainer);

      NS_LOG_INFO ("------Installing NDN Producer Application------");

      NS_LOG_INFO ("Producer Payload size: " << payLoadsize);

      // Create the producer on the mobile node
      ndn::AppHelper producerHelper ("ns3::ndn::Producer");
      producerHelper.SetPrefix ("/waseda/sato");
      producerHelper.SetAttribute ("StopTime", TimeValue (Seconds(endTime-1)));
      // Payload size is in bytes
      producerHelper.SetAttribute ("PayloadSize", UintegerValue(payLoadsize));

      // Install producer
      if (producer)
	{
	  sprintf(moving, "%s", "prod");
	  producerHelper.Install (MNContainer);
	}
      else
	{
	  producerHelper.Install (ServerContainer);
	}

      NS_LOG_INFO ("------Installing NDN Consumer Application------");

      NS_LOG_INFO ("Consumer Interests/Second frequency: " << intFreq);

      // Create the consumer on the randomly selected node
      ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
      consumerHelper.SetPrefix ("/waseda/sato");
      consumerHelper.SetAttribute ("Frequency", DoubleValue (intFreq));
      consumerHelper.SetAttribute ("StartTime", TimeValue (Seconds(2)));
      consumerHelper.SetAttribute ("StopTime", TimeValue (Seconds(endTime-1)));
      consumerHelper.SetAttribute ("RetxTimer", TimeValue (Seconds(retxtime)));

      // Install consumer
      if (producer)
	{
	  consumerHelper.Install (ServerContainer);
	}
      else
	{
	  sprintf(moving, "%s", "con");
	  consumerHelper.Install (MNContainer);
	}
    }

  if (use3N)
    {
      sprintf(routeType, "%s", "3n");
      sprintf(buffer, "%dms", (int)(retxtime*1000));

      std::string retxch (buffer);
      NS_LOG_INFO ("Setting retransmission times to: " << retxch);

      // Now install content stores and the rest on the middle node. Leave
      // out clients and the mobile node
      NS_LOG_INFO ("------ Installing 3N stacks ------");

      NS_LOG_INFO ("------ Installing Server 3N stack ------");
      // Stack for nodes that use fixed connections
      nnn::NNNStackHelper ServerStack;
      // Set the Forwarding Strategy and have it have a 3N name lease time of 50 seconds
      ServerStack.SetForwardingStrategy ("ns3::nnn::ForwardingStrategy", "3NLeasetime", "120s", "RetxTimer", retxch);
      // Set the Content Store for the primary stack, Normal LRU ContentStore of 10000000 objects
      ServerStack.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "10000000");
      // Set the FIB default routes
      ServerStack.SetDefaultRoutes (true);
      // Install the stack
      ServerStack.Install (ServerContainer);

      // Create the initial 3N name
      Ptr<nnn::NNNAddress> firstName = Create <nnn::NNNAddress> ("a");
      // Get the ForwardingStrategy object from the node
      Ptr<nnn::ForwardingStrategy> fwServer = ServerContainer.Get (0)->GetObject<nnn::ForwardingStrategy> ();
      // Give a 3N name for the first AP - ensure it is longer than the actual simulation
      fwServer->SetNode3NName (firstName, Seconds (endTime + 5), true);

      ///////////////////////////////////////////////////////

      NS_LOG_INFO ("------ Installing AP 3N stack ------");
      // Stack for a Node that is given a node name
      nnn::NNNStackHelper APStack;
      // Set the Forwarding Strategy and have it have a 3N name lease time of 50 seconds
      APStack.SetForwardingStrategy ("ns3::nnn::ForwardingStrategy", "3NLeasetime", "80s", "RetxTimer", retxch);
      // Set the Content Store for the primary stack, Normal LRU ContentStore of 10000000 objects
      APStack.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", "10000000");
      // Set the FIB default routes
      APStack.SetDefaultRoutes (true);
      // Install the stack
      APStack.Install (APContainer);

      // Vector of all forwarding strategies for APs
      std::vector<Ptr<nnn::ForwardingStrategy> > fwAPs;

      // Force Enrollment for the APs
      for (int i = 0; i < aps; i++)
	{
	  fwAPs.push_back (APContainer.Get (i)->GetObject<nnn::ForwardingStrategy> ());
	}

      for (int i = 0; i < aps; i++)
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
      mobileStack.Install (MNContainer);

      // Create the applications
      NS_LOG_INFO ("------ Installing Producer Application------ ");
      // Create the producer on the AP node - same as in NDN
      nnn::AppHelper producerHelper ("ns3::nnn::Producer");
      producerHelper.SetPrefix ("/waseda/sato");
      producerHelper.SetAttribute ("PayloadSize", UintegerValue(payLoadsize));
      producerHelper.SetAttribute("StartTime", TimeValue (Seconds(2)));
      producerHelper.SetAttribute("StopTime", TimeValue (Seconds(endTime -1)));

      // Install producer
      if (producer)
	{
	  sprintf(moving, "%s", "prod");
	  if (useMobility)
	    {
	      sprintf(mob, "%s", "yes");
	      NS_LOG_INFO ("Provider is using mobility 3N DU support");
	      producerHelper.SetAttribute("UseDU", BooleanValue(true));
	    }
	  else
	    sprintf(mob, "%s", "no");

	  producerHelper.Install (MNContainer);
	}
      else
	{
	  producerHelper.Install (ServerContainer);
	}

      NS_LOG_INFO ("------ Installing Consumer Application------ ");
      // Create the consumer node on the mobile node - same as in NDN
      nnn::AppHelper consumerHelper ("ns3::nnn::ConsumerCbr");
      consumerHelper.SetPrefix ("/waseda/sato");
      consumerHelper.SetAttribute ("Frequency", DoubleValue (intFreq));
      consumerHelper.SetAttribute("StartTime", TimeValue (Seconds(2)));
      consumerHelper.SetAttribute("StopTime", TimeValue (Seconds(endTime-1)));
      consumerHelper.SetAttribute ("RetxTimer", TimeValue (Seconds(retxtime)));


      // Install consumer
      if (producer)
	{
	  consumerHelper.Install (ServerContainer);
	}
      else
	{
	  sprintf(moving, "%s", "con");
	  if (useMobility)
	    {
	      sprintf(mob, "%s", "yes");
	      NS_LOG_INFO ("Consumer is using mobility 3N SO support");
	      consumerHelper.SetAttribute("UseSO", BooleanValue(true));
	    }
	  else
	    sprintf(mob, "%s", "no");

	  consumerHelper.Install (MNContainer);
	}
    }

  NS_LOG_INFO ("------ Creating the AP Association Callbacks ------");
  char configbuf[250];

  // The AP association callbacks are only useful in 3N Mobility setting
  if (useMobility)
    {
      for (int i = 0; i < mobileNodeIds.size (); i++)
	{
	  NS_LOG_INFO ("Establishing callbacks for Node " << mobileNodeIds[i] << " wireless device 0");
	  sprintf (configbuf, "/NodeList/%d/DeviceList/%d/$ns3::WifiNetDevice/Mac/$ns3::StaWifiMac/Assoc", mobileNodeIds[i], 0);
	  Config::Connect(configbuf, MakeCallback (&ApAssociation));
	}
    }

  NS_LOG_INFO ("------ SSID distance event changes ------");
  for (int i = 0; i < mobileNodeIds.size (); i++)
    {
      for (int j = 50; j < endTime; j += 50)
	{
	  NS_LOG_INFO ("Scheduling SSID calculation for Node " << mobileNodeIds[i] << " wireless device 0 at " << j);
	  // Simulator::Schedule (Seconds(j), &SetSSIDviaDistance, mobileNodeIds[i], 0, apTerminalMobility);
	  Simulator::Schedule (Seconds(j), &SetForcedSSID, mobileNodeIds[i], 0, ssidV[1], useMobility);
	}
    }

  NS_LOG_INFO("Ending time " << endTime);

  // If the variable is set, print the trace files
  if (traceFiles) {
      // Filename
      char filename[250];

      // File ID
      char fileId[250];

      // Create the file identifier
      sprintf(fileId, "%s-%s-%s-%02d-%03d-%03d.txt", routeType, moving, mob, mobile, servers, aps);

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
	  NS_LOG_INFO ("Installing Application tracers");
	  // 3N App Tracer
	  sprintf (filename, "%s/%s-app-delays-%s", results, scenario, fileId);
	  nnn::AppDelayTracer::InstallAll (filename);

	  NS_LOG_INFO ("Installing Aggregate tracers");
	  // 3N Aggregate tracer
	  sprintf (filename, "%s/%s-aggregate-trace-%s", results, scenario, fileId);
	  nnn::L3AggregateTracer::InstallAll(filename, Seconds (1.0));
	}
  }

  NS_LOG_INFO ("------Scheduling events - SSID changes------");

  NS_LOG_INFO ("------Ready for execution!------");

  Simulator::Stop (Seconds (endTime));
  Simulator::Run ();
  Simulator::Destroy ();
}
