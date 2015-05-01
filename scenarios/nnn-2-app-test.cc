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

char scenario[250] = "3N2Test";

NS_LOG_COMPONENT_DEFINE (scenario);

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
void SetSSIDviaDistance(uint32_t mtId, Ptr<MobilityModel> node, std::map<std::string, Ptr<MobilityModel> > aps)
{
  char configbuf[250];
  char buffer[250];

  // This causes the device in mtId to change the SSID, forcing AP change
  sprintf(configbuf, "/NodeList/%d/DeviceList/0/$ns3::WifiNetDevice/Mac/Ssid", mtId);

  std::map<double, std::string> SsidDistance;

  // Iterate through the map of seen Ssids
  for (std::map<std::string, Ptr<MobilityModel> >::iterator ii=aps.begin(); ii!=aps.end(); ++ii)
    {
      // Calculate the distance from the AP to the node and save into the map
      SsidDistance[node->GetDistanceFrom((*ii).second)] = (*ii).first;
    }

  double distance = SsidDistance.begin()->first;
  std::string ssid(SsidDistance.begin()->second);

  sprintf(buffer, "Change to SSID %s at distance of %f", ssid.c_str(), distance);

  NS_LOG_INFO(buffer);

  // Because the map sorts by std:less, the first position has the lowest distance
  Config::Set(configbuf, SsidValue(ssid));

  // Empty the maps
  SsidDistance.clear();
}

int main (int argc, char *argv[])
{
  // These are our scenario arguments
  uint32_t sectors = 1;                         // Number of wireless sectors
  uint32_t aps = 1;                             // Number of wireless access nodes in a sector
  uint32_t mobile = 1;                          // Number of mobile terminals
  uint32_t servers = 1;                         // Number of servers in the network
  uint32_t wnodes = aps * sectors;              // Number of nodes in the network
  uint32_t xaxis = 100;                         // Size of the X axis
  uint32_t yaxis = 100;                         // Size of the Y axis
  double sec = 0.0;                             // Movement start
  bool traceFiles = false;                      // Tells to run the simulation with traceFiles
  bool useMobility = false;                     // Tells to run the simulation with mobility support
  bool useCSMA = false;                         // Tells to run the simulation using CSMA
  bool useP2P  = false;                         // Tells to run the simulation using P2P links
  bool useWIFI = false;                         // Tells to run the simulation using WIFI links
  char results[250] = "results";                // Directory to place results
  double endTime = 100;                         // Number of seconds to run the simulation

  // Variable for buffer
  char buffer[250];

  CommandLine cmd;
  cmd.AddValue ("mobile", "Number of mobile terminals in simulation", mobile);
  cmd.AddValue ("servers", "Number of servers in the simulation", servers);
  cmd.AddValue ("results", "Directory to place results", results);
  cmd.AddValue ("start", "Starting second", sec);
  cmd.AddValue ("trace", "Enable trace files", traceFiles);
  cmd.AddValue ("mobility", "Enable mobility support", useMobility);
  cmd.AddValue ("csma", "Enable CSMA links", useCSMA);
  cmd.AddValue ("p2p", "Enable P2P links", useP2P);
  cmd.AddValue ("wifi", "Enable WIFI links", useWIFI);
  cmd.Parse (argc,argv);


  if (!(useCSMA || useP2P || useWIFI))
    {
      NS_LOG_ERROR ("Must use P2P, CSMA or WIFI links! If all are flagged, they will be used respectively");
      return 1;
    }

  NS_LOG_INFO ("------Creating nodes------");
  NS_LOG_INFO ("Creating " << mobile << " nodes");
  // Node definitions for mobile terminals
  NodeContainer MNContainer;
  MNContainer.Create(mobile);

  std::vector<uint32_t> mobileNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < mobile; i++)
    {
      mobileNodeIds.push_back(MNContainer.Get (i)->GetId ());
    }

  NS_LOG_INFO ("Creating " << wnodes << " AP nodes");
  // Wireless access Nodes
  NodeContainer APContainer;
  APContainer.Create (wnodes);

  // Container for all 3N capable nodes
  NodeContainer all3NNodes;
  all3NNodes.Add (MNContainer);
  all3NNodes.Add (APContainer);

  std::vector<uint32_t> wirelessAPNodeIds;

  // Save all the mobile Node IDs
  for (int i = 0; i < aps; i++)
    {
      wirelessAPNodeIds.push_back(APContainer.Get (i)->GetId ());
    }

  // Make sure to seed our random
  gen.seed (std::time (0) + (long long)getpid () << 32);

  NS_LOG_INFO ("------Placing Wireless nodes-------");
  MobilityHelper centralStations;

  Ptr<ListPositionAllocator> initialCenter = CreateObject<ListPositionAllocator> ();

  int added = 0;

  for (int i = 0; i < aps; i++)
    {
      Vector pos (0.0 + added, 0.0, 0.0);
      initialCenter->Add (pos);
      added += 100;
    }

  centralStations.SetPositionAllocator(initialCenter);
  centralStations.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  centralStations.Install(APContainer);

  NS_LOG_INFO ("------Placing Mobile nodes-------");
  sprintf(buffer, "0|%d|0|%d", xaxis, yaxis);

  string bounds = string(buffer);

  MobilityHelper mobileStations;

  mobileStations.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                   "Mode", StringValue ("Distance"),
                                   "Distance", StringValue ("500"),
                                   "Bounds", StringValue (bounds));

  mobileStations.Install(MNContainer);

  if (useP2P)
    {
      NS_LOG_INFO ("------ Creating P2P links ------");
      NS_LOG_INFO("------ Connecting P2P links ------");
      nnn::FlexPointToPointHelper p2p_100mbps5ms;
      p2p_100mbps5ms.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
      p2p_100mbps5ms.SetChannelAttribute ("Delay", StringValue ("1ms"));

      vector <NetDeviceContainer> ptpWLANCenterDevices;

      for (int i = 0; i < sectors; i++)
	{
	  NetDeviceContainer ptpWirelessCenterDevices;

	  for (int j = 0; j < mobile; j++)
	    {
	      ptpWirelessCenterDevices.Add (p2p_100mbps5ms.Install (APContainer.Get (i), MNContainer.Get (j) ));
	    }

	  ptpWLANCenterDevices.push_back (ptpWirelessCenterDevices);
	}
    }

  if (useCSMA)
    {
      NS_LOG_INFO ("------ Creating CSMA links ------");
      CsmaHelper csma;
      csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
      csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (4)));

      NetDeviceContainer csmaDevices;

      csmaDevices = csma.Install(all3NNodes);
    }

  if (useWIFI)
    {
      NS_LOG_INFO ("------ Creating WIFI Links ------");
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

      for (int i = 0; i < wnodes; i++)
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
      for (int i = 0; i < wnodes; i++)
	{
	  wifiMacHelper.SetType ("ns3::ApWifiMac",
	                         "Ssid", SsidValue (ssidV[i]),
	                         "BeaconGeneration", BooleanValue (true),
	                         "BeaconInterval", TimeValue (Seconds (0.102)));

	  wifiAPNetDevices.push_back (wifi.Install (wifiPhyHelper, wifiMacHelper, APContainer.Get (i)));
	}

      // Create a Wifi for the mobile node
      wifiMacHelper.SetType("ns3::StaWifiMac",
                            "Ssid", SsidValue (ssidV[0]),
                            "ActiveProbing", BooleanValue (true));

      NetDeviceContainer wifiMTNetDevices = wifi.Install (wifiPhyHelper, wifiMacHelper, MNContainer);

      // Using the same calculation from the Yans-wifi-Channel, we obtain the Mobility Models for the
      // mobile node as well as all the Wifi capable nodes
      Ptr<MobilityModel> mobileTerminalMobility = (MNContainer.Get (0))->GetObject<MobilityModel> ();

      std::vector<Ptr<MobilityModel> > mobileTerminalsMobility;

      // Get the list of mobile node mobility models
      for (int i = 0; i < mobile; i++)
	{
	  mobileTerminalsMobility.push_back((MNContainer.Get (i))->GetObject<MobilityModel> ());
	}
    }

  char routeType[250];

  // Now install content stores and the rest on the middle node. Leave
  // out clients and the mobile node
  NS_LOG_INFO ("------ Installing 3N stack ------");

  NS_LOG_INFO ("------ Installing primary 3N stack ------");
  // Stack for a Node that is given a node name
  nnn::NNNStackHelper primaryStack;
  // Set the Forwarding Strategy and have it have a 3N name lease time of 50 seconds
  primaryStack.SetForwardingStrategy("ns3::nnn::ForwardingStrategy", "3NLeasetime", "50s");
  // Set the Content Store for the primary stack, Normal LRU ContentStore of 10000000 objects
  primaryStack.SetContentStore("ns3::ndn::cs::Freshness::Lru", "MaxSize", "10000000");
  // Set the FIB default routes
  primaryStack.SetDefaultRoutes (true);
  // Install the stack
  primaryStack.Install(APContainer);

  // Create the initial 3N name
  Ptr<nnn::NNNAddress> firstName = Create <nnn::NNNAddress> ("a");
  // Get the ForwardingStrategy object from the node
  Ptr<nnn::ForwardingStrategy> fwAP = APContainer.Get (0)->GetObject<nnn::ForwardingStrategy> ();
  // Give a 3N name for the first AP - ensure it is longer than the actual simulation
  fwAP->SetNode3NName(firstName, Seconds (endTime + 5), true);

  ///////////////////////////////////////////////////////
  // Stack for nodes that use fixed connections
  nnn::NNNStackHelper fixedStack;
  ///////////////////////////////////////////////////////

  NS_LOG_INFO ("------ Installing mobile 3N stack ------");
  // Stack for nodes that are mobile;
  nnn::NNNStackHelper mobileStack;
  // No Content Store for mobile stack
  mobileStack.SetContentStore ("ns3::ndn::cs::Nocache");
  // Do not produce 3N names for these nodes
  mobileStack.SetForwardingStrategy("ns3::nnn::ForwardingStrategy", "Produce3Nnames", "false");
  // Set the FIB default routes
  mobileStack.SetDefaultRoutes (true);
  // Install the stack
  mobileStack.Install(MNContainer);

  // Get the ForwardingStrategy object from the mobile nodes
    Ptr<nnn::ForwardingStrategy> fwMN = MNContainer.Get (0)->GetObject<nnn::ForwardingStrategy> ();
  // Force the enroll procedure of the mobile node
  Simulator::Schedule (Seconds(3), &nnn::ForwardingStrategy::Enroll, fwMN);

  // Create the applications
  NS_LOG_INFO ("------ Installing Producer Application------ ");
  // Create the producer on the AP node - same as in NDN
  nnn::AppHelper producerHelper ("ns3::nnn::Producer");
  producerHelper.SetPrefix ("/waseda/sato");
  producerHelper.SetAttribute("StopTime", TimeValue (Seconds(endTime -1)));
  // Install producer on AP
  producerHelper.Install (APContainer);

  NS_LOG_INFO ("------ Installing Consumer Application------ ");
  // Create the consumer node on the mobile node - same as in NDN
  nnn::AppHelper consumerHelper ("ns3::nnn::ConsumerCbr");
  consumerHelper.SetPrefix ("/waseda/sato");
  consumerHelper.SetAttribute ("Frequency", DoubleValue (0.05));
  consumerHelper.SetAttribute("StartTime", TimeValue (Seconds(4)));
  consumerHelper.SetAttribute("StopTime", TimeValue (Seconds(endTime-1)));
  if (useMobility)
    {
      NS_LOG_INFO ("Consumer is using mobility 3N SO support");
      consumerHelper.SetAttribute("IsMobile", BooleanValue(true));
    }
  consumerHelper.Install (MNContainer);

  NS_LOG_INFO("Ending time " << endTime);

  // If the variable is set, print the trace files
  if (traceFiles) {
      // Filename
      char filename[250];

      // File ID
      char fileId[250];

      // Create the file identifier
      sprintf(fileId, "%02d-%03d-%03d.txt", mobile, servers, wnodes);

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
      for (int i = 0; i < wirelessAPNodeIds.size(); i++)
	{
	  serverFile << wirelessAPNodeIds[i] << std::endl;
	}

      serverFile.close();

      NS_LOG_INFO ("Installing Application tracers");
      // 3N App Tracer
      sprintf (filename, "%s/%s-app-delays-%s", results, scenario, fileId);
      nnn::AppDelayTracer::InstallAll (filename);

      // 3N L3 tracer
      sprintf (filename, "%s/%s-rate-trace-%s", results, scenario, fileId);
      nnn::L3RateTracer::InstallAll (filename, Seconds (1.0));

      NS_LOG_INFO ("Installing Aggregate tracers");
      // 3N Aggregate tracer
      sprintf (filename, "%s/%s-aggregate-trace-%s", results, scenario, fileId);
      nnn::L3AggregateTracer::InstallAll(filename, Seconds (1.0));
  }

  NS_LOG_INFO ("------Scheduling events - SSID changes------");

  NS_LOG_INFO ("------Ready for execution!------");

  Simulator::Stop (Seconds (endTime));
  Simulator::Run ();
  Simulator::Destroy ();
}
