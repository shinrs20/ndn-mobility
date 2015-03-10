/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template by University of California written by
 *   Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *
 *  nnn-forwarding-strategy.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-forwarding-strategy.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-forwarding-strategy.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

// nnnSIM - 3N data
#include "nnn-forwarding-strategy.h"

#include "../pit/nnn-pit.h"
#include "../pit/nnn-pit-entry.h"
#include "../fib/nnn-fib.h"
#include "../nnn-face.h"
#include "../nnn-naming.h"
#include "../nnn-packets.h"
#include "../nnst/nnn-nnst.h"
#include "../nnst/nnn-nnst-entry.h"
#include "../nnst/nnn-nnst-entry-facemetric.h"
#include "../nnpt/nnn-nnpt.h"
#include "../../helper/nnn-names-container.h"
#include "../buffers/nnn-pdu-buffer.h"
#include "../addr-aggr/nnn-addr-aggregator.h"
#include "../../helper/nnn-header-helper.h"

// ndnSIM - NDN data
#include <ns3-dev/ns3/name.h>
#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-content-store.h>
#include <ns3-dev/ns3/ndn-wire.h>
#include <ns3-dev/ns3/ndn-header-helper.h>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/boolean.h>
#include <ns3-dev/ns3/integer.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/ptr.h>

#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/string.h>

namespace ll = boost::lambda;

// Max label, which is a really the maximum unsigned int
#define MAX3NLABEL 4294967295

namespace ns3 {
  namespace nnn {

    NS_OBJECT_ENSURE_REGISTERED (ForwardingStrategy);

    NS_LOG_COMPONENT_DEFINE (ForwardingStrategy::GetLogName ().c_str ());

    std::string
    ForwardingStrategy::GetLogName ()
    {
      return "nnn.fw";
    }

    TypeId ForwardingStrategy::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::ForwardingStrategy")
	  .SetGroupName ("nnn")
	  .SetParent<Object> ()

	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutENs",  "OutENs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outENs))
	  .AddTraceSource ("InENs",   "InENs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inENs))
	  .AddTraceSource ("DropENs", "DropENs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropENs))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutAENs",  "OutAENs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outAENs))
	  .AddTraceSource ("InAENs",   "InAENs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inAENs))
	  .AddTraceSource ("DropAENs", "DropAENs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropAENs))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutDENs",  "OutDENs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outDENs))
	  .AddTraceSource ("InDENs",   "InDENs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inDENs))
	  .AddTraceSource ("DropDENs", "DropDENs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropDENs))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutRENs",  "OutRENs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outRENs))
	  .AddTraceSource ("InRENs",   "InRENs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inRENs))
	  .AddTraceSource ("DropRENs", "DropRENs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropRENs))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutINFs",  "OutINFs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outINFs))
	  .AddTraceSource ("InINFs",   "InINFs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inINFs))
	  .AddTraceSource ("DropINFs", "DropINFs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropINFs))

	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutNULLps",  "OutNULLps",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outNULLps))
	  .AddTraceSource ("InNULLps",   "InNULLps",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inNULLps))
	  .AddTraceSource ("DropNULLps", "DropNULLps", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropNULLps))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutSOs",  "OutSOs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outSOs))
	  .AddTraceSource ("InSOs",   "InSOs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inSOs))
	  .AddTraceSource ("DropSOs", "DropSOs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropSOs))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutDOs",  "OutDOs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outDOs))
	  .AddTraceSource ("InDOs",   "InDOs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inDOs))
	  .AddTraceSource ("DropDOs", "DropDOs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropDOs))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutDUs",  "OutDUs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outDUs))
	  .AddTraceSource ("InDUs",   "InDUs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inDUs))
	  .AddTraceSource ("DropDUs", "DropDUs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropDUs))

	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutInterests",  "OutInterests",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outInterests))
	  .AddTraceSource ("InInterests",   "InInterests",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inInterests))
	  .AddTraceSource ("DropInterests", "DropInterests", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropInterests))

	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("OutData",  "OutData",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outData))
	  .AddTraceSource ("InData",   "InData",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inData))
	  .AddTraceSource ("DropData", "DropData", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropData))

	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////

	  .AddTraceSource ("SatisfiedInterets", "SatisfiedInterests", MakeTraceSourceAccessor (&ForwardingStrategy::m_satisfiedInterests))
	  .AddTraceSource ("TimedOutInterests", "TimedOutInterests", MakeTraceSourceAccessor (&ForwardingStrategy::m_timedOutInterests))

	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////

	  .AddAttribute ("CacheUnsolicitedDataFromApps", "Cache unsolicited Data that has been pushed from applications",
	                 BooleanValue (true),
	                 MakeBooleanAccessor (&ForwardingStrategy::m_cacheUnsolicitedDataFromApps),
	                 MakeBooleanChecker ())

	  .AddAttribute ("CacheUnsolicitedData", "Cache overheard Data that have not been requested",
	                 BooleanValue (false),
	                 MakeBooleanAccessor (&ForwardingStrategy::m_cacheUnsolicitedData),
	                 MakeBooleanChecker ())

	  .AddAttribute ("DetectRetransmissions", "If non-duplicate Interest is received on the same face more than once, it is considered a retransmission",
	                 BooleanValue (true),
	                 MakeBooleanAccessor (&ForwardingStrategy::m_detectRetransmissions),
	                 MakeBooleanChecker ())

	  .AddAttribute ("Produce3Nnames", "Produce 3N names from delegated name space",
	                 BooleanValue (true),
	                 MakeBooleanAccessor (&ForwardingStrategy::m_produce3Nnames),
	                 MakeBooleanChecker ())

	  .AddAttribute ("3NLeasetime", "Lease time for a 3N name (Only in use if Produce3Nnames is used)",
	                 StringValue ("300s"),
	                 MakeTimeAccessor (&ForwardingStrategy::m_3n_lease_time),
	                 MakeTimeChecker ())

	  .AddAttribute ("3NLifetime", "LifeTime for 3N PDUs",
	                 StringValue ("2s"),
	                 MakeTimeAccessor (&ForwardingStrategy::m_3n_lifetime),
	                 MakeTimeChecker ())

	  .AddAttribute ("StandardMetric", "Standard Metric in NNST for new entries (Only in use if Produce3NNames is used)",
	                 IntegerValue (6),
	                 MakeIntegerAccessor (&ForwardingStrategy::m_standardMetric),
	                 MakeIntegerChecker<int32_t>())

	  // Required for testing at this moment
	  .AddConstructor <ForwardingStrategy> ()
	  ;
      return tid;
    }

    ForwardingStrategy::ForwardingStrategy ()
    {
    }

    ForwardingStrategy::~ForwardingStrategy ()
    {
    }

    uint64_t
    ForwardingStrategy::obtain_Num(uint64_t min, uint64_t max)
    {
      NS_LOG_FUNCTION (this);
      // Make sure to seed our random
      Time now = Simulator::Now();

      gen.seed((long long)(now.GetMilliSeconds()) + (long long)getpid() << 32);

      boost::random::uniform_int_distribution<> dist(min, max);

      return dist(gen);
    }

    void
    ForwardingStrategy::SetNode3NName (Ptr<const NNNAddress> name, Time lease)
    {
      NS_LOG_FUNCTION (this);
      NS_LOG_INFO("Adding 3N name " << *name << " to node");
      m_node_names->addEntry(name, lease);
    }

    const NNNAddress&
    ForwardingStrategy::GetNode3NName ()
    {
      NS_LOG_FUNCTION (this);
      return *m_node_names->findNewestName();
    }

    Ptr<const NNNAddress>
    ForwardingStrategy::GetNode3NNamePtr ()
    {
      NS_LOG_FUNCTION (this);
      return m_node_names->findNewestName();
    }

    Ptr<NNNAddress>
    ForwardingStrategy::produce3NName ()
    {
      NS_LOG_FUNCTION (this);
      bool produced = false;

      Ptr<NNNAddress> ret;

      if (Has3NName ())
	{
	  // Get this nodes currently functioning 3N name and copy it to a new variable
	  NNNAddress base = GetNode3NName ();

	  while (!produced)
	    {
	      // Create a random numerical component
	      name::Component tmp = tmp.fromNumber(obtain_Num(1, MAX3NLABEL));

	      // Append the randomly created number to our 3N name
	      NNNAddress tmp3 = base.append(tmp);

	      ret = Create<NNNAddress> (tmp3.toDotHex());

	      // Check if the random has by unfortunate circumstances created a name
	      // that has already been leased
	      if (! m_leased_names->foundName(ret))
		{
		  produced = true;
		}
	    }

	  NS_LOG_INFO("Produce 3N name " << *ret);
	}
      return ret;
    }

    bool
    ForwardingStrategy::Has3NName ()
    {
      return (!m_node_names->isEmpty());
    }

    bool
    ForwardingStrategy::GoesBy3NName (Ptr<NNNAddress> addr)
    {
      return m_node_names->foundName (addr);
    }

    void
    ForwardingStrategy::flushBuffer(Ptr<NNNAddress> oldName, Ptr<NNNAddress> newName)
    {
      NS_LOG_FUNCTION (this << *oldName << " to " << *newName);
      if (m_node_pdu_buffer->DestinationExists(oldName))
	{
	  // The actual queue
	  std::queue<Ptr<Packet> > addrQueue = m_node_pdu_buffer->PopQueue(oldName);

	  // Dummy Pointers to the PDU types
	  Ptr<DO> do_o_orig;
	  Ptr<SO> so_o_orig;
	  Ptr<DU> du_o_orig;

	  std::pair<Ptr<Face>, Address> closestSector;

	  Ptr<Face> outFace;
	  Address destAddr;

	  while (!addrQueue.empty())
	    {
	      // Get the PDU at the front of the queue
	      Ptr<Packet> queuePDU = addrQueue.front();

	      switch(HeaderHelper::GetNNNHeaderType(queuePDU))
	      {
		case SO_NNN:
		  // Convert the Packet back to a SO for manipulation
		  so_o_orig = wire::nnnSIM::SO::FromWire(queuePDU);

		  // Renew the SO lifetime
		  so_o_orig->SetLifetime(Seconds(2));
		  // Change the SO 3N name to the new name
		  so_o_orig->SetName(newName);

		  //
		  // I will have to define a particular routing method for SO PDUs
		  // in this area.
		  //
		  break;
		case DO_NNN:
		  // Convert the Packet back to a DO for manipulation
		  do_o_orig = wire::nnnSIM::DO::FromWire(queuePDU);

		  // Renew the DO lifetime
		  do_o_orig->SetLifetime(Seconds(2));
		  // Change the DO 3N name to the new name
		  do_o_orig->SetName(newName);

		  // Find where to send the SO
		  closestSector = m_nnst->ClosestSectorFaceInfo(newName);

		  outFace = closestSector.first;
		  destAddr = closestSector.second;

		  // Send the created DO PDU
		  outFace->SendDO(do_o_orig, destAddr);
		  // Log the DO sending
		  m_outDOs(do_o_orig, outFace);
		  break;
		case DU_NNN:
		  // Convert the Packet back to a DU for manipulation
		  du_o_orig = wire::nnnSIM::DU::FromWire(queuePDU);

		  // Renew the DU lifetime
		  du_o_orig->SetLifetime(Seconds(2));

		  // Change the DU 3N names to the new names if necessary
		  if (du_o_orig->GetDstName() == *oldName)
		    du_o_orig->SetDstName(newName);

		  if (du_o_orig->GetSrcName() == *oldName)
		    du_o_orig->SetSrcName(newName);

		  // Find where to send the DU
		  closestSector = m_nnst->ClosestSectorFaceInfo(du_o_orig->GetDstNamePtr());

		  outFace = closestSector.first;
		  destAddr = closestSector.second;

		  // Send the created DU PDU
		  outFace->SendDU(du_o_orig, destAddr);
		  // Log the DU sending
		  m_outDUs(du_o_orig, outFace);
		  break;
		default:
		  NS_LOG_INFO("flushBuffer obtained unknown PDU");
	      }
	      // Pop the queue and continue
	      addrQueue.pop ();
	    }

	  // Make sure we delete the entry for oldName in the buffer
	  m_node_pdu_buffer->RemoveDestination(oldName);
	}
    }

    void
    ForwardingStrategy::OnEN (Ptr<Face> face, Ptr<EN> en_p)
    {
      NS_LOG_FUNCTION (this);

      m_inENs(en_p, face);

      // Find if we can produce 3N names
      if (m_produce3Nnames && Has3NName ())
	{
	  NNNAddress myAddr = GetNode3NName ();
	  NS_LOG_INFO("We are in " << myAddr << " producing 3N name for new node");

	  // Get the first Address from the EN PDU
	  Address destAddr = en_p->GetOnePoa(0);

	  // Get all the PoA Address in the EN PDU to fill the NNST
	  std::vector<Address> poaAddrs = en_p->GetPoas();

	  // Produce a 3N name
	  Ptr<NNNAddress> produced3Nname = produce3NName ();

	  // Add the new information to the NNST
	  m_nnst->Add(produced3Nname, face, poaAddrs, m_3n_lease_time, m_standardMetric);

	  // Add the information the the leased NodeNameContainer
	  m_leased_names->addEntry(produced3Nname, m_3n_lease_time);

	  // Now create the AEN PDU to respond
	  Ptr<AEN> aen_p = Create<AEN> (produced3Nname);
	  // Ensure that the lease time is set right
	  aen_p->SetLeasetime(m_3n_lease_time);

	  // Send the created AEN PDU out the way it came
	  face->SendAEN(aen_p, destAddr);

	  m_outAENs(aen_p, face);
	}
      else
	{
	  m_dropENs(en_p, face);
	}
    }

    void
    ForwardingStrategy::OnAEN (Ptr<Face> face, Ptr<AEN> aen_p)
    {
      NS_LOG_FUNCTION (this);

      m_inAENs(aen_p, face);

      Ptr<const NNNAddress> obtainedName = aen_p->GetNamePtr();

      NS_LOG_INFO("Obtained AEN with " << *obtainedName);

      // Check if we have a 3N name
      if (Has3NName ())
	{
	  // As long as the name is not the same, we can use the name
	  if (*GetNode3NNamePtr() != *obtainedName)
	    {
	      NS_LOG_INFO("Node had " << GetNode3NName () << " now taking " << *obtainedName);
	      SetNode3NName(obtainedName, aen_p->GetLeasetime());
	    }
	}
      else
	{
	  NS_LOG_INFO("Node has no name, taking " << *obtainedName);
	  SetNode3NName(obtainedName, aen_p->GetLeasetime());
	}
    }

    void
    ForwardingStrategy::OnREN (Ptr<Face> face, Ptr<REN> ren_p)
    {
      NS_LOG_FUNCTION (this);

      m_inRENs(ren_p, face);

      // Check we can actually produce 3N names
      if(m_produce3Nnames && Has3NName ())
	{
	  NNNAddress myAddr = GetNode3NName ();

	  // Get the 3N name that the node was using
	  Ptr<NNNAddress> reenroll = Create<NNNAddress> (ren_p->GetName ());

	  NS_LOG_INFO("We are in " << myAddr << " producing 3N name for " << *reenroll);

	  // Get the first Address from the REN PDU
	  Address destAddr = ren_p->GetOnePoa(0);

	  // Get all the PoA Address in the REN PDU to fill the NNST
	  std::vector<Address> poaAddrs = ren_p->GetPoas ();

	  // Produce a new 3N name
	  Ptr<NNNAddress> produced3Nname = produce3NName ();

	  // Add the new information to the NNST
	  m_nnst->Add(produced3Nname, face, poaAddrs, m_3n_lease_time, m_standardMetric);

	  // Add the information the the leased NodeNameContainer
	  m_leased_names->addEntry(produced3Nname, m_3n_lease_time);

	  // Now create the AEN PDU to respond
	  Ptr<AEN> aen_p = Create<AEN> (produced3Nname);
	  // Ensure that the lease time is set right
	  aen_p->SetLeasetime(m_3n_lease_time);

	  // Send the created AEN PDU out the way it came
	  face->SendAEN(aen_p, destAddr);

	  // Log that the AEN was sent
	  m_outAENs(aen_p, face);

	  Time remaining = ren_p->GetRemainLease();

	  // Regardless of the name, we need to update the NNPT
	  m_nnpt->addEntry(reenroll, produced3Nname, remaining);

	  // If we happen to be in the same subsector, the buffer will have something
	  // the check is good practice
	  flushBuffer(reenroll, produced3Nname);

	  // Check if the Node was in our subsector
	  if (! reenroll->isSubSector(myAddr))
	    {
	      // If node was not originally in our sector, create a INF PDU
	      NS_LOG_INFO("Creating INF PDU");
	      Ptr<INF> inf_o = Create<INF> ();

	      // Fill the necessary information
	      inf_o->SetOldName(reenroll);
	      inf_o->SetNewName(produced3Nname);
	      inf_o->SetRemainLease(remaining);

	      // Find where to send the INF
	      std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo(reenroll->getSectorName());

	      Ptr<Face> outFace = tmp.first;
	      Address destAddr = tmp.second;

	      // Send the created INF PDU
	      outFace->SendINF(inf_o, destAddr);

	      // Log that the INF PDU was sent
	      m_outINFs(inf_o, outFace);
	    }
	}
      else
	{
	  m_dropRENs(ren_p, face);
	}
    }

    void
    ForwardingStrategy::OnDEN (Ptr<Face> face, Ptr<DEN> den_p)
    {
      NS_LOG_FUNCTION (this);

      m_inDENs(den_p, face);

      NNNAddress myAddr = GetNode3NName ();
      Ptr<NNNAddress> leavingAddr = Create<NNNAddress> (den_p->GetNamePtr()->getName());

      NS_LOG_INFO("We are in " << myAddr << ", " << *leavingAddr << " is leaving");

      // We know the node sending the DEN is moving. His lease time will be maintained
      // All we need to do is tell the buffer to keep the packets to that destination
      m_node_pdu_buffer->AddDestination(leavingAddr);

      // If the DEN packet arrives at a node that is less than 2 hops away, then we
      // forward the DEN packet to the parent of this node
      if (leavingAddr->distance(myAddr) <= 2 && leavingAddr->isSubSector(myAddr))
	{
	  // Now we forward the DEN information to the higher hierarchical nodes
	  std::vector<std::pair<Ptr<Face>, Address> > hierarchicalFaces = m_nnst->OneHopParentSectorFaceInfo(myAddr);
	  std::vector<std::pair<Ptr<Face>, Address> >::iterator it;

	  Ptr<Face> outFace;
	  Address destAddr;

	  for (it = hierarchicalFaces.begin(); it != hierarchicalFaces.end (); ++it)
	    {
	      outFace = it->first;
	      destAddr = it->second;

	      outFace->SendDEN(den_p, destAddr);

	      // Log that the DEN PDU was sent
	      m_outDENs(den_p, outFace);
	    }
	}
    }

    void
    ForwardingStrategy::OnINF (Ptr<Face> face, Ptr<INF> inf_p)
    {
      NS_LOG_FUNCTION (this);

      m_inINFs(inf_p, face);

      // Update our NNPT with the information in the INF PDU
      m_nnpt->addEntry(inf_p->GetOldNamePtr(), inf_p->GetNewNamePtr(), inf_p->GetRemainLease());

      NNNAddress endSector = inf_p->GetOldNamePtr()->getSectorName();
      NNNAddress myAddr = GetNode3NName ();

      Ptr<NNNAddress> oldName = Create<NNNAddress> (inf_p->GetOldName());
      Ptr<NNNAddress> newName = Create<NNNAddress> (inf_p->GetNewName());

      // We have inserted the INF information. Now flush the relevant buffer
      flushBuffer(oldName, newName);

      if (myAddr != endSector)
	{
	  NS_LOG_INFO("We are in " << myAddr << " receiving an INF. Have not yet reached the delegated Sector " << endSector);

	  // Roughly pick the next hop that would bring us closer to the endSector
          std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo(endSector);

          Ptr<Face> outFace = tmp.first;
          Address destAddr = tmp.second;

          outFace->SendINF(inf_p, destAddr);

          // Log that the INF PDU was sent
          m_outINFs(inf_p, outFace);
	}
    }

    void
    ForwardingStrategy::OnNULLp (Ptr<Face> face, Ptr<NULLp> null_p)
    {
      NS_LOG_FUNCTION (this);

      m_inNULLps(null_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = null_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU>(null_p);

      ProcessICNPDU(pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::OnSO (Ptr<Face> face, Ptr<SO> so_p)
    {
      NS_LOG_FUNCTION (this);

      m_inSOs(so_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = so_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU>(so_p);

      ProcessICNPDU(pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::OnDO (Ptr<Face> face, Ptr<DO> do_p)
    {
      NS_LOG_FUNCTION (this);

      m_inDOs(do_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = do_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU>(do_p);

      ProcessICNPDU(pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::OnDU (Ptr<Face> face, Ptr<DU> du_p)
    {
      NS_LOG_FUNCTION (this);

      m_inDUs(du_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = du_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU>(du_p);

      ProcessICNPDU(pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::ProcessICNPDU (Ptr<NNNPDU> pdu, Ptr<Face> face, Ptr<Packet> icn_pdu)
    {
      bool receivedInterest =false;
      bool receivedData = false;
      Ptr<ndn::Interest> interest;
      Ptr<ndn::Data> data;

      try {
	  ndn::HeaderHelper::Type ndnType = ndn::HeaderHelper::GetNdnHeaderType(icn_pdu);
	  switch (ndnType)
	  {
	    case ndn::HeaderHelper::INTEREST_NDNSIM:
	      interest = ndn::Wire::ToInterest (icn_pdu, ndn::Wire::WIRE_FORMAT_NDNSIM);
	      receivedInterest = true;
	      break;
	    case ndn::HeaderHelper::INTEREST_CCNB:
	      interest = ndn::Wire::ToInterest (icn_pdu, ndn::Wire::WIRE_FORMAT_CCNB);
	      receivedInterest = true;
	      break;
	    case ndn::HeaderHelper::CONTENT_OBJECT_NDNSIM:
	      data = ndn::Wire::ToData (icn_pdu, ndn::Wire::WIRE_FORMAT_NDNSIM);
	      receivedData = true;
	      break;
	    case ndn::HeaderHelper::CONTENT_OBJECT_CCNB:
	      data = ndn::Wire::ToData (icn_pdu, ndn::Wire::WIRE_FORMAT_CCNB);
	      receivedData = true;
	      break;
	    default:
	      NS_FATAL_ERROR ("Not supported NDN header");
	  }
      }
      catch (ndn::UnknownHeaderException)
      {
	  NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
      }

      // If the PDU is an Interest
      if (receivedInterest)
	{
	  ProcessInterest (pdu, face, interest);
	}

      // If the PDU is Data
      if (receivedData)
	{
	  ProcessData(pdu, face, data);
	}
    }

    void
    ForwardingStrategy::UpdatePITEntry (Ptr<pit::Entry> pitEntry, Ptr<NNNPDU> pdu, Ptr<Face> face, Time lifetime)
    {
      // Pointers for PDU types
      Ptr<SO> so_i;
      Ptr<DU> du_i;

      // Find out the type of PDU we are dealing with
      uint32_t pduid = pdu->GetPacketId();
      switch(pduid)
      {
	case NULL_NNN:
	  // Add the Face
	  pitEntry->AddIncoming(face);
	  // Show that you received a NULL PDU
	  pitEntry->SetReceivedNULLPDU(true);
	  break;
	case SO_NNN:
	  // Convert pointer to SO
	  so_i = DynamicCast<SO>(pdu);
	  // Add the Face
	  pitEntry->AddIncoming(face, so_i->GetNamePtr());
	  break;
	case DU_NNN:
	  // Convert pointer to DU
	  du_i = DynamicCast<DU>(pdu);
	  // Add the Face
	  pitEntry->AddIncoming(face, du_i->GetSrcNamePtr());
	  break;
	default:
	  break;
      }
      // Update PIT entry lifetime
      pitEntry->UpdateLifetime (lifetime);
    }

    void
    ForwardingStrategy::ProcessInterest (Ptr<NNNPDU> pdu, Ptr<Face> face, Ptr<ndn::Interest> interest)
    {
      NS_LOG_FUNCTION (face << interest->GetName ());
      // Log the Interest PDU
      m_inInterests (interest, face);

      // Search for the PIT with the Interest
      Ptr<pit::Entry> pitEntry = m_pit->Lookup (*interest);
      bool similarInterest = true;
      if (pitEntry == 0)
	{
	  // Flag that we have seen this Interest
	  similarInterest = false;
	  pitEntry = m_pit->Create (interest);
	  if (pitEntry != 0)
	    {
	      // Log the creation of a PIT entry
	      DidCreatePitEntry (face, interest, pitEntry);
	    }
	  else
	    {
	      // Log the failure to create a PIT entry
	      FailedToCreatePitEntry (face, interest);
	      // Finish processing
	      return;
	    }
	}

      bool isDuplicated = true;
      // Check if the Interest has been seen before
      if (!pitEntry->IsNonceSeen (interest->GetNonce ()))
	{
	  // Not seen, add the Nonce
	  pitEntry->AddSeenNonce (interest->GetNonce ());
	  // Flag that we have a duplicated object
	  isDuplicated = false;
	}

      if (isDuplicated)
	{
	  // Log that we received a duplicate Interest
	  DidReceiveDuplicateInterest (face, interest, pitEntry);
	  // Finish processing
	  return;
	}

      // Check if we have this particular Interest in the ContentStore
      Ptr<ndn::Data> contentObject = m_contentStore->Lookup (interest);
      if (contentObject != 0)
	{
	  // Update the PIT
	  UpdatePITEntry(pitEntry, pdu, face, Seconds(1));

	  // Do data plane performance measurements
	  WillSatisfyPendingInterest (0, pitEntry);

	  // Actually satisfy pending interest
	  SatisfyPendingInterest (pdu, 0, contentObject, pitEntry);
	  return;
	}

      // In the case of similar Interest, update the PIT entry
      if (similarInterest && ShouldSuppressIncomingInterest (face, interest, pitEntry))
	{
	  // Update the PIT
	  UpdatePITEntry(pitEntry, pdu, face, interest->GetInterestLifetime());

	  // Suppress this interest if we're still expecting data from some other face
	  NS_LOG_DEBUG ("Suppress interests");
	  m_dropInterests (interest, face);

	  DidSuppressSimilarInterest (face, interest, pitEntry);
	  return;
	}

      if (similarInterest)
	{
	  // Log that we obtained a similar Interest
	  DidForwardSimilarInterest (face, interest, pitEntry);
	}

      PropagateInterest (pdu, face, interest, pitEntry);
    }

    void
    ForwardingStrategy::ProcessData (Ptr<NNNPDU> pdu, Ptr<Face> face, Ptr<ndn::Data> data)
    {
      NS_LOG_FUNCTION (face << data->GetName ());
      // Log the Data PDU
      m_inData (data, face);

      // Lookup PIT entry
      Ptr<pit::Entry> pitEntry = m_pit->Lookup (*data);

      // Using 3N, we allow all data to be cached - will probably need to be discussed
      if (pitEntry != 0)
	{
	  // Add to content store
	  m_contentStore->Add (data);
	  // Log that this node actually asked for this data
	  DidReceiveSolicitedData (face, data, true);
	}
      else
	{
	  // Add to content store
	  m_contentStore->Add (data);
	  // Log that this node is proactively caching this data
	  DidReceiveUnsolicitedData (face, data, true);
	}

      while (pitEntry != 0)
	{
	  // Do data plane performance measurements
	  WillSatisfyPendingInterest (face, pitEntry);

	  // Actually satisfy pending interest
	  SatisfyPendingInterest (pdu, face, data, pitEntry);

	  // Lookup another PIT entry
	  pitEntry = m_pit->Lookup (*data);
	}
    }

    void
    ForwardingStrategy::WillEraseTimedOutPendingInterest (Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      NS_LOG_DEBUG ("WillEraseTimedOutPendingInterest for " << pitEntry->GetPrefix ());

      for (pit::Entry::out_container::iterator face = pitEntry->GetOutgoing ().begin ();
	  face != pitEntry->GetOutgoing ().end ();
	  face ++)
	{
	  // NS_LOG_DEBUG ("Face: " << face->m_face);
	  pitEntry->GetFibEntry ()->UpdateStatus (face->m_face, fib::FaceMetric::NDN_FIB_YELLOW);
	}

      m_timedOutInterests (pitEntry);
    }

    void
    ForwardingStrategy::AddFace (Ptr<Face> face)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::RemoveFace (Ptr<Face> face)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::DidAddNNSTEntry (Ptr<nnst::Entry> NNSTEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::WillRemoveNNSTEntry (Ptr<nnst::Entry> NNSTEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::DidAddNNPTEntry (Ptr<nnpt::Entry> NNPTEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::WillRemoveNNPTEntry (Ptr<nnpt::Entry> NNPTEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::DidAddFibEntry (Ptr<fib::Entry> fibEntry)
    {
    }

    void
    ForwardingStrategy::WillRemoveFibEntry (Ptr<fib::Entry> fibEntry)
    {
    }

    void
    ForwardingStrategy::DidCreatePitEntry (Ptr<Face> inFace,
                                           Ptr<const ndn::Interest> interest,
                                           Ptr<pit::Entry> pitEntrypitEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::FailedToCreatePitEntry (Ptr<Face> inFace,
                                                Ptr<const ndn::Interest> interest)
    {
      NS_LOG_FUNCTION (this);
      m_dropInterests (interest, inFace);
    }

    void
    ForwardingStrategy::DidReceiveDuplicateInterest (Ptr<Face> inFace,
                                                     Ptr<const ndn::Interest> interest,
                                                     Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      /////////////////////////////////////////////////////////////////////////////////////////
      //                                                                                     //
      // !!!! IMPORTANT CHANGE !!!! Duplicate interests will create incoming face entry !!!! //
      //                                                                                     //
      /////////////////////////////////////////////////////////////////////////////////////////
      pitEntry->AddIncoming (inFace);
      m_dropInterests (interest, inFace);
    }

    void
    ForwardingStrategy::DidSuppressSimilarInterest (Ptr<Face> face,
                                                    Ptr<const ndn::Interest> interest,
                                                    Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::DidForwardSimilarInterest (Ptr<Face> inFace,
                                                   Ptr<const ndn::Interest> interest,
                                                   Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::DidExhaustForwardingOptions (Ptr<Face> inFace,
                                                     Ptr<const ndn::Interest> interest,
                                                     Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this << boost::cref (*inFace));
      if (pitEntry->AreAllOutgoingInVain ())
	{
	  m_dropInterests (interest, inFace);

	  // All incoming interests cannot be satisfied. Remove them
	  pitEntry->ClearIncoming ();

	  // Remove also outgoing
	  pitEntry->ClearOutgoing ();

	  // Set pruning timout on PIT entry (instead of deleting the record)
	  m_pit->MarkErased (pitEntry);
	}
    }

    bool
    ForwardingStrategy::DetectRetransmittedInterest (Ptr<Face> inFace,
                                                     Ptr<const ndn::Interest> interest,
                                                     Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      pit::Entry::in_iterator existingInFace = pitEntry->GetIncoming ().find (inFace);

      bool isRetransmitted = false;

      if (existingInFace != pitEntry->GetIncoming ().end ())
	{
	  // this is almost definitely a retransmission. But should we trust the user on that?
	  isRetransmitted = true;
	}

      return isRetransmitted;
    }

    void
    ForwardingStrategy::WillSatisfyPendingInterest (Ptr<Face> inFace,
                                                    Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      if (inFace != 0)
	{
	  // Update metric status for the incoming interface in the corresponding FIB entry
	  pitEntry->GetFibEntry ()->UpdateStatus (inFace, fib::FaceMetric::NDN_FIB_GREEN);
	}

      pit::Entry::out_iterator out = pitEntry->GetOutgoing ().find (inFace);

      // If we have sent interest for this data via this face, then update stats.
      if (out != pitEntry->GetOutgoing ().end ())
	{
	  pitEntry->GetFibEntry ()->UpdateFaceRtt (inFace, Simulator::Now () - out->m_sendTime);
	}

      m_satisfiedInterests (pitEntry);
    }

    void
    ForwardingStrategy::SatisfyPendingInterest (Ptr<NNNPDU> pdu,
                                                Ptr<Face> inFace,
                                                Ptr<const ndn::Data> data,
                                                Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      if (inFace != 0)
	pitEntry->RemoveIncoming (inFace);

      // Convert the Data PDU into a NS-3 Packet
      Ptr<Packet> icn_pdu = ndn::Wire::FromData (data);

      // Pointers and flags for PDU types
      Ptr<NULLp> nullp_i;
      bool wasNULL = false;
      Ptr<SO> so_i;
      bool wasSO = false;
      Ptr<DO> do_i;
      bool wasDO = false;
      Ptr<DU> du_i;
      bool wasDU = false;

      uint32_t pduid = pdu->GetPacketId();
      switch(pduid)
      {
	case NULL_NNN:
	  // Convert pointer to NULLp PDU
	  nullp_i = DynamicCast<NULLp> (pdu);
	  wasNULL = true;
	  break;
	case SO_NNN:
	  // Convert pointer to SO PDU
	  so_i = DynamicCast<SO> (pdu);
	  wasSO = true;
	  break;
	case DO_NNN:
	  // Convert pointer to DO PDU
	  do_i = DynamicCast<DO> (pdu);
	  wasDO = true;
	  break;
	case DU_NNN:
	  // Convert pointer to DU PDU
	  du_i = DynamicCast<DU> (pdu);
	  wasDU = true;
	  break;
	default:
	  break;
      }

      // Satisfy all pending Interests with the Data we received on each Face
      BOOST_FOREACH (const pit::IncomingFace &incoming, pitEntry->GetIncoming ())
      {
	bool ok = false;
	bool sentSomething = false;

	// Obtain the distinct 3N names associated to this Face and go through them
	std::vector<Ptr<NNNAddress> > distinct = incoming.m_addrs->GetDistinctDestinations ();
	BOOST_FOREACH (Ptr<NNNAddress> j, distinct)
	{
	  bool subSector = (*j == GetNode3NName ());
	  // Obtain all the 3N names aggregated in this sector
	  std::vector<Ptr<NNNAddress> > addrs = incoming.m_addrs->GetCompleteDestinations(j);

	  // If the aggregation is the same as the 3N Name the node is using, then
	  // everything aggregated is probably connected to it
	  if (subSector)
	    {
	      // Obtain all the 3N names associated to this Face and Sector and go through them
	      BOOST_FOREACH (Ptr<NNNAddress> i, addrs)
		  {
		NNNAddress newdst;

		// Check if the NNPT has any information for this particular 3N name
		if (m_nnpt->foundOldName(i))
		  {
		    // Retrieve the new 3N name destination
		    newdst = m_nnpt->findPairedNamePtr (i)->getName ();
		  }
		else
		  {
		    newdst = *i;
		  }

		// Roughly pick the next hop that would bring us closer to newdst
		std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo (newdst);

		Ptr<Face> outFace = tmp.first;
		Address destAddr = tmp.second;

		if (wasNULL || wasSO || wasDO)
		  {
		    // Since we don't have more information about this 3N name, create a DO to push the
		    // Data to a new location
		    Ptr<DO> do_o_spec = Create<DO> ();
		    // Set the new 3N name
		    do_o_spec->SetName (newdst);
		    // Set the lifetime of the 3N PDU
		    do_o_spec->SetLifetime (m_3n_lifetime);
		    // Configure payload for PDU
		    do_o_spec->SetPayload (icn_pdu);
		    // Signal that the PDU had an ICN PDU as payload
		    do_o_spec->SetPDUPayloadType (NNN_NNN);

		    // Send the DO PDU out the selected Face
		    ok = outFace->SendDO(do_o_spec, destAddr);

		    // Log that a Data PDU was sent
		    DidSendOutData (inFace, outFace, data, pitEntry);
		    // Log that a DO PDU was sent
		    m_outDOs(do_o_spec, outFace);

		    // Something caused an error
		    if (!ok)
		      {
			// Log Data drops
			m_dropData (data, incoming.m_face);
			// Log DO PDU drop
			m_dropDOs (do_o_spec, outFace);
			NS_LOG_DEBUG ("Cannot satisfy data to " << newdst << " via "<< *incoming.m_face);
		      }
		    else
		      {
			if (incoming.m_face == outFace)
			  // Actually sent something using this Face
			  sentSomething = true;
		      }
		  }
		else if (wasDU)
		  {
		    // We know that the Data was brought by a DU PDU, meaning we know the origin
		    // Create a new DU PDU to send the data
		    Ptr<DU> du_o_spec = Create<DU> ();
		    // Use the original DU's Src 3N name
		    du_o_spec->SetSrcName (du_i->GetSrcName ());
		    // Set the new 3N name destination
		    du_o_spec->SetDstName (newdst);
		    // Set the lifetime of the 3N PDU
		    du_o_spec->SetLifetime (m_3n_lifetime);
		    // Configure payload for PDU
		    du_o_spec->SetPayload (icn_pdu);
		    // Signal that the PDU had an ICN PDU as payload
		    du_o_spec->SetPDUPayloadType (NNN_NNN);

		    // Send the DU PDU out the selected Face
		    ok = outFace->SendDU (du_o_spec, destAddr);

		    // Log that a Data PDU was sent
		    DidSendOutData (inFace, outFace, data, pitEntry);
		    // Log that a DO PDU was sent
		    m_outDUs (du_o_spec, outFace);

		    // Something caused an error
		    if (!ok)
		      {
			// Log Data drops
			m_dropData (data, incoming.m_face);
			// Log DU PDU drop
			m_dropDUs (du_o_spec, outFace);
			NS_LOG_DEBUG ("Cannot satisfy data to " << newdst << " via "<< *incoming.m_face);
		      }
		    else
		      {
			if (incoming.m_face == outFace)
			  // Actually sent something using this Face
			  sentSomething = true;
		      }
		  }
		  }

	      // If we had received a NULL PDU with this Interest at some point, and have no 3N names
	      // saved, return a NULL PDU with the information
	      if (pitEntry->GetReceivedNULLPDU() && !sentSomething)
		{
		  Ptr<NULLp> null_p_o = Create<NULLp> ();
		  // Set the lifetime of the 3N PDU
		  null_p_o->SetLifetime (m_3n_lifetime);
		  // Configure payload for PDU
		  null_p_o->SetPayload (icn_pdu);
		  // Signal that the PDU had an ICN PDU as payload
		  null_p_o->SetPDUPayloadType (NNN_NNN);

		  // Log that a Data PDU was sent
		  DidSendOutData (inFace, incoming.m_face, data, pitEntry);
		  // Log that a DO PDU was sent
		  m_outNULLps (null_p_o, incoming.m_face);

		  NS_LOG_DEBUG ("Satisfy " << *incoming.m_face);

		  if (!ok)
		    {
		      m_dropData (data, incoming.m_face);
		      m_dropNULLps (null_p_o, incoming.m_face);
		      NS_LOG_DEBUG ("Cannot satisfy data to " << *incoming.m_face);
		    }
		}
	    }
	  else
	    {
	      // In this case, we are not dealing with a subsector
	      // Obtain all the 3N names associated to this Face and Sector and go through them
	      BOOST_FOREACH(Ptr<NNNAddress> i, addrs)
		  {
		NNNAddress newdst;

		// Only push a new PDU is the 3N name has changed
		// Check if the NNPT has any information for this particular 3N name
		if (m_nnpt->foundOldName(i))
		  {
		    // Retrieve the new 3N name destination
		    newdst = m_nnpt->findPairedNamePtr (i)->getName ();

		    // Roughly pick the next hop that would bring us closer to newdst
		    std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo (newdst);

		    Ptr<Face> outFace = tmp.first;
		    Address destAddr = tmp.second;

		    if (wasNULL || wasSO || wasDO)
		      {
			// Since we don't have more information about this 3N name, create a DO to push the
			// Data to a new location
			Ptr<DO> do_o_spec = Create<DO> ();
			// Set the new 3N name
			do_o_spec->SetName (newdst);
			// Set the lifetime of the 3N PDU
			do_o_spec->SetLifetime (m_3n_lifetime);
			// Configure payload for PDU
			do_o_spec->SetPayload (icn_pdu);
			// Signal that the PDU had an ICN PDU as payload
			do_o_spec->SetPDUPayloadType (NNN_NNN);

			// Send the DO PDU out the selected Face
			ok = outFace->SendDO(do_o_spec, destAddr);

			// Log that a Data PDU was sent
			DidSendOutData (inFace, outFace, data, pitEntry);
			// Log that a DO PDU was sent
			m_outDOs(do_o_spec, outFace);

			// Something caused an error
			if (!ok)
			  {
			    // Log Data drops
			    m_dropData (data, incoming.m_face);
			    // Log DO PDU drop
			    m_dropDOs (do_o_spec, outFace);
			    NS_LOG_DEBUG ("Cannot satisfy data to " << newdst << " via "<< *incoming.m_face);
			  }
			else
			  {
			    if (incoming.m_face == outFace)
			      // Actually sent something using this Face
			      sentSomething = true;
			  }
		      }
		    else if (wasDU)
		      {
			// We know that the Data was brought by a DU PDU, meaning we know the origin
			// Create a new DU PDU to send the data
			Ptr<DU> du_o_spec = Create<DU> ();
			// Use the original DU's Src 3N name
			du_o_spec->SetSrcName (du_i->GetSrcName ());
			// Set the new 3N name destination
			du_o_spec->SetDstName (newdst);
			// Set the lifetime of the 3N PDU
			du_o_spec->SetLifetime (m_3n_lifetime);
			// Configure payload for PDU
			du_o_spec->SetPayload (icn_pdu);
			// Signal that the PDU had an ICN PDU as payload
			du_o_spec->SetPDUPayloadType (NNN_NNN);

			// Send the DU PDU out the selected Face
			ok = outFace->SendDU (du_o_spec, destAddr);

			// Log that a Data PDU was sent
			DidSendOutData (inFace, outFace, data, pitEntry);
			// Log that a DO PDU was sent
			m_outDUs (du_o_spec, outFace);

			// Something caused an error
			if (!ok)
			  {
			    // Log Data drops
			    m_dropData (data, incoming.m_face);
			    // Log DU PDU drop
			    m_dropDUs (du_o_spec, outFace);
			    NS_LOG_DEBUG ("Cannot satisfy data to " << newdst << " via "<< *incoming.m_face);
			  }
			else
			  {
			    if (incoming.m_face == outFace)
			      // Actually sent something using this Face
			      sentSomething = true;
			  }
		      }
		  }

		// If we haven't sent something at this point, send a NULL PDU with the
		// relevant information
		if (!sentSomething)
		  {
		    Ptr<NULLp> null_p_o = Create<NULLp> ();
		    // Set the lifetime of the 3N PDU
		    null_p_o->SetLifetime (m_3n_lifetime);
		    // Configure payload for PDU
		    null_p_o->SetPayload (icn_pdu);
		    // Signal that the PDU had an ICN PDU as payload
		    null_p_o->SetPDUPayloadType (NNN_NNN);

		    // Log that a Data PDU was sent
		    DidSendOutData (inFace, incoming.m_face, data, pitEntry);
		    // Log that a DO PDU was sent
		    m_outNULLps (null_p_o, incoming.m_face);

		    NS_LOG_DEBUG ("Satisfy " << *incoming.m_face);

		    if (!ok)
		      {
			m_dropData (data, incoming.m_face);
			m_dropNULLps (null_p_o, incoming.m_face);
			NS_LOG_DEBUG ("Cannot satisfy data to " << *incoming.m_face);
		      }
		  }
		  }
	    }
	}
      }

      // All incoming interests are satisfied. Remove them
      pitEntry->ClearIncoming ();

      // Remove all outgoing faces
      pitEntry->ClearOutgoing ();

      // Set pruning timeout on PIT entry (instead of deleting the record)
      m_pit->MarkErased (pitEntry);
    }

    void
    ForwardingStrategy::DidSendOutData (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const ndn::Data> data,
                                        Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      m_outData (data, inFace == 0, outFace);
    }

    void
    ForwardingStrategy::DidReceiveSolicitedData (Ptr<Face> inFace,
                                                 Ptr<const ndn::Data> data,
                                                 bool didCreateCacheEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::DidReceiveUnsolicitedData (Ptr<Face> inFace,
                                                   Ptr<const ndn::Data> data,
                                                   bool didCreateCacheEntry)
    {
      NS_LOG_FUNCTION (this);
    }

    bool
    ForwardingStrategy::ShouldSuppressIncomingInterest (Ptr<Face> inFace,
                                                        Ptr<const ndn::Interest> interest,
                                                        Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      bool isNew = pitEntry->GetIncoming ().size () == 0 && pitEntry->GetOutgoing ().size () == 0;

      if (isNew) return false; // never suppress new interests

      bool isRetransmitted = m_detectRetransmissions && // a small guard
	  DetectRetransmittedInterest (inFace, interest, pitEntry);

      if (pitEntry->GetOutgoing ().find (inFace) != pitEntry->GetOutgoing ().end ())
	{
	  NS_LOG_DEBUG ("Non duplicate interests from the face we have sent interest to. Don't suppress");
	  // got a non-duplicate interest from the face we have sent interest to
	  // Probably, there is no point in waiting data from that face... Not sure yet

	  // If we're expecting data from the interface we got the interest from ("producer" asks us for "his own" data)
	  // Mark interface YELLOW, but keep a small hope that data will come eventually.

	  // ?? not sure if we need to do that ?? ...

	  // pitEntry->GetFibEntry ()->UpdateStatus (inFace, fib::FaceMetric::NDN_FIB_YELLOW);
	}
      else
	if (!isNew && !isRetransmitted)
	  {
	    return true;
	  }

      return false;
    }

    bool
    ForwardingStrategy::CanSendOutInterest (Ptr<Face> inFace,
                                            Ptr<Face> outFace,
                                            Ptr<const ndn::Interest> interest,
                                            Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      if (outFace == inFace)
	{
	  // NS_LOG_DEBUG ("Same as incoming");
	  return false; // same face as incoming, don't forward
	}

      pit::Entry::out_iterator outgoing =
	  pitEntry->GetOutgoing ().find (outFace);

      if (outgoing != pitEntry->GetOutgoing ().end ())
	{
	  if (!m_detectRetransmissions)
	    return false; // suppress
	  else if (outgoing->m_retxCount >= pitEntry->GetMaxRetxCount ())
	    {
	      // NS_LOG_DEBUG ("Already forwarded before during this retransmission cycle (" <<outgoing->m_retxCount << " >= " << pitEntry->GetMaxRetxCount () << ")");
	      return false; // already forwarded before during this retransmission cycle
	    }
	}

      return true;
    }

    bool
    ForwardingStrategy::TrySendOutInterest (Ptr<Face> inFace,
                                            Ptr<Face> outFace,
                                            Ptr<const ndn::Interest> interest,
                                            Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      if (!CanSendOutInterest (inFace, outFace, interest, pitEntry))
	{
	  return false;
	}

      pitEntry->AddOutgoing (outFace);

      //transmission
      bool successSend = false;
      //bool successSend = outFace->SendInterest (interest);
      if (!successSend)
	{
	  m_dropInterests (interest, outFace);
	}

      DidSendOutInterest (inFace, outFace, interest, pitEntry);

      return true;
    }

    void
    ForwardingStrategy::DidSendOutInterest (Ptr<Face> inFace,
                                            Ptr<Face> outFace,
                                            Ptr<const ndn::Interest> interest,
                                            Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      m_outInterests (interest, outFace);
    }

    void
    ForwardingStrategy::PropagateInterest (Ptr<NNNPDU> pdu,
                                           Ptr<Face> inFace,
                                           Ptr<const ndn::Interest> interest,
                                           Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      bool isRetransmitted = m_detectRetransmissions && // a small guard
	  DetectRetransmittedInterest (inFace, interest, pitEntry);

      pitEntry->AddIncoming (inFace/*, interest->GetInterestLifetime ()*/);
      /// @todo Make lifetime per incoming interface
      pitEntry->UpdateLifetime (interest->GetInterestLifetime ());

      bool propagated = DoPropagateInterest (pdu, inFace, interest, pitEntry);

      if (!propagated && isRetransmitted) //give another chance if retransmitted
	{
	  // increase max number of allowed retransmissions
	  pitEntry->IncreaseAllowedRetxCount ();

	  // try again
	  propagated = DoPropagateInterest (pdu, inFace, interest, pitEntry);
	}

      // if (!propagated)
      //   {
      //     NS_LOG_DEBUG ("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      //     NS_LOG_DEBUG ("+++ Not propagated ["<< interest->GetName () <<"], but number of outgoing faces: " << pitEntry->GetOutgoing ().size ());
      //     NS_LOG_DEBUG ("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      //   }

      // ForwardingStrategy will try its best to forward PDU to at least one interface.
      // If no interests was propagated, then there is not other option for forwarding or
      // ForwardingStrategy failed to find it.
      if (!propagated && pitEntry->AreAllOutgoingInVain ())
	{
	  DidExhaustForwardingOptions (inFace, interest, pitEntry);
	}
    }

    bool
    ForwardingStrategy::DoPropagateInterest (Ptr<NNNPDU> pdu,
                                             Ptr<Face> inFace,
                                             Ptr<const ndn::Interest> interest,
                                             Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);
      NS_ASSERT_MSG (m_pit != 0, "PIT should be aggregated with forwarding strategy");

      int propagatedCount = 0;

      BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
      {
	if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED ||
	    metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_YELLOW)
	  break; //propagate only to green faces

	if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
	  {
	    continue;
	  }

	propagatedCount++;
	break; // propagate only one interest
      }

      bool greenOk = propagatedCount > 0;

      if (greenOk)
	return true;

      propagatedCount = 0;

      BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
      {
	NS_LOG_DEBUG ("Trying " << boost::cref(metricFace));
	if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in the front of the list
	  break;

	if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
	  {
	    continue;
	  }

	propagatedCount++;
      }

      NS_LOG_INFO ("Propagated to " << propagatedCount << " faces");
      return propagatedCount > 0;
    }

    void
    ForwardingStrategy::NotifyNewAggregate ()
    {
      if (m_nnpt == 0)
	{
	  m_nnpt = GetObject<NNPT> ();
	}
      if (m_nnst == 0)
	{
	  m_nnst = GetObject<NNST> ();
	}
      if (m_pit == 0)
	{
	  m_pit = GetObject<Pit> ();
	}
      if (m_fib == 0)
	{
	  m_fib = GetObject<Fib> ();
	}

      if (m_contentStore == 0)
	{
	  m_contentStore = GetObject<ndn::ContentStore> ();
	}

      Object::NotifyNewAggregate ();
    }

    void
    ForwardingStrategy::DoDispose ()
    {
      m_nnpt = 0;
      m_nnst = 0;

      m_pit = 0;
      m_fib = 0;
      m_contentStore = 0;

      Object::DoDispose ();
    }
  } // namespace nnn
} // namespace ns3
