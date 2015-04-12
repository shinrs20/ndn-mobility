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

#include "../nnn-naming.h"

#include "../nnn-pdus.h"
#include "../nnn-nnnsim-wire.h"

#include "../pit/nnn-pit.h"
#include "../pit/nnn-pit-entry.h"
#include "../pit/nnn-pit-entry-incoming-face.h"
#include "../pit/nnn-pit-entry-outgoing-face.h"
#include "../fib/nnn-fib.h"

#include "../nnst/nnn-nnst.h"
#include "../nnst/nnn-nnst-entry.h"
#include "../nnst/nnn-nnst-entry-facemetric.h"
#include "../nnpt/nnn-nnpt.h"

#include "../../helper/nnn-names-container.h"
#include "../../helper/nnn-face-container.h"
#include "../buffers/nnn-pdu-buffer.h"
#include "../addr-aggr/nnn-addr-aggregator.h"
#include "../../helper/nnn-header-helper.h"

#include "../../helper/nnn-face-container.h"

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
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/object-base.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/string.h>
#include <ns3-dev/ns3/type-id.h>
#include <ns3-dev/ns3/trace-source-accessor.h>
#include <ns3-dev/ns3/nstime.h>

#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace ll = boost::lambda;

// Max label
#define MAX3NLABEL 429496729

namespace ns3
{
  namespace nnn
  {
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
	  .SetGroupName ("Nnn")
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

	  .AddTraceSource ("OutOENs",  "OutOENs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outOENs))
	  .AddTraceSource ("InOENs",   "InOENs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inOENs))
	  .AddTraceSource ("DropOENs", "DropOENs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropOENs))

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

	  .AddTraceSource ("SatisfiedInterests", "SatisfiedInterests", MakeTraceSourceAccessor (&ForwardingStrategy::m_satisfiedInterests))
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

	  .AddAttribute ("3NLeaseACK", "Timeout for leasing a 3N name (Only in use if Produce3Nnames is used)",
	                 StringValue ("5s"),
	                 MakeTimeAccessor (&ForwardingStrategy::m_3n_lease_ack_timeout),
	                 MakeTimeChecker ())

	  .AddAttribute ("3NLifetime", "LifeTime for 3N PDUs",
	                 StringValue ("3s"),
	                 MakeTimeAccessor (&ForwardingStrategy::m_3n_lifetime),
	                 MakeTimeChecker ())

	  .AddAttribute ("AckTimeout", "Timeout for ACK based PDUs",
	                 StringValue ("5s"),
	                 MakeTimeAccessor (&ForwardingStrategy::m_ack_timeout),
	                 MakeTimeChecker ())

	  .AddAttribute ("StandardMetric", "Standard Metric in NNST for new entries (Only in use if Produce3NNames is used)",
	                 IntegerValue (6),
	                 MakeIntegerAccessor (&ForwardingStrategy::m_standardMetric),
	                 MakeIntegerChecker<int32_t>())

	  .AddTraceSource ("Got3NName", "Traces when the forwarding strategy has a 3N name",
			   MakeTraceSourceAccessor (&ForwardingStrategy::m_got3Nname))

	  .AddTraceSource ("No3NName", "Traces when the forwarding strategy is waiting for a 3N name",
			   MakeTraceSourceAccessor (&ForwardingStrategy::m_no3Nname))

	  // Required for testing at this moment
	  .AddConstructor <ForwardingStrategy> ()
	  ;
      return tid;
    }

    ForwardingStrategy::ForwardingStrategy ()
    : m_awaiting_response    (Create<NNST> ())
    , m_faces                (Create<FaceContainer> ())
    , m_node_names           (Create<NamesContainer> ())
    , m_leased_names         (Create<NamesContainer> ())
    , m_node_pdu_buffer      (Create<PDUBuffer> ())
    , m_producedNameNumber   (0)
    , m_no_oen               (false)
    {
      m_node_names->RegisterCallbacks(
	  MakeCallback (&ForwardingStrategy::Reenroll, this),
	  MakeCallback (&ForwardingStrategy::Enroll, this)
      );
    }

    ForwardingStrategy::~ForwardingStrategy ()
    {
    }

    void
    ForwardingStrategy::SetNode3NName (Ptr<const NNNAddress> name, Time lease, bool fixed)
    {
      NS_LOG_FUNCTION (this);
      // Modification to know which node if being called
      Ptr<Node> node = this->GetObject<Node> ();

      NS_LOG_INFO("Adding 3N name (" << *name << ") to node " << node->GetId());
      m_node_names->addEntry(name, lease, fixed);
      // TracedCallback to let Apps know we have a name
      m_got3Nname ();
    }

    const NNNAddress&
    ForwardingStrategy::GetNode3NName ()
    {
      return *m_node_names->findNewestName();
    }

    Ptr<const NNNAddress>
    ForwardingStrategy::GetNode3NNamePtr ()
    {
      return m_node_names->findNewestName();
    }

    Ptr<NNNAddress>
    ForwardingStrategy::produce3NName ()
    {
      NS_LOG_FUNCTION (this);
      bool produced = false;

      Ptr<NNNAddress> final;

      if (Has3NName ())
	{
	  // Get this nodes currently functioning 3N name and copy it to a new variable
	  NNNAddress base = GetNode3NName ();

	  while (!produced)
	    {
	      // Create an empty container for the 3N name
	      Ptr<NNNAddress> ret;

	      // Create a random numerical component
	      name::Component tmp = name::Component ();
	      // Add the information
	      tmp.fromNumber(m_producedNameNumber);

	      // Create a NNNAddress with base
	      ret = Create <NNNAddress> (base.toDotHex());
	      // Append the new component
	      ret->append(tmp);

	      // Check if by unfortunate circumstances the created name has already been leased
	      if (! (m_leased_names->foundName(ret) || (m_node_lease_times.find (ret) != m_node_lease_times.end ())))
		{
		  produced = true;
		  final = ret;
		}
	      else
		{
		  NS_LOG_INFO ("We have already produced " << *ret << " cycling through");
		  m_producedNameNumber++;
		}
	    }

	  NS_LOG_INFO("Produced a 3N name (" << *final << ")");
	}
      return final;
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
    ForwardingStrategy::flushBuffer(Ptr<Face> face, Ptr<NNNAddress> oldName, Ptr<NNNAddress> newName)
    {
      NS_LOG_FUNCTION (this << face->GetId () << *oldName << " to " << *newName);
      NNNAddress myAddr = GetNode3NName ();
      if (m_node_pdu_buffer->DestinationExists(oldName))
	{

	  NS_LOG_INFO ("On (" << myAddr << ") found a queue for (" << *oldName << "), attempting to flush");
	  // The actual queue
	  std::queue<Ptr<Packet> > addrQueue = m_node_pdu_buffer->PopQueue(oldName);

	  // Dummy Pointers to the PDU types
	  Ptr<DO> do_o_orig;
	  Ptr<DU> du_o_orig;

	  uint32_t do_flush = 0;
	  uint32_t du_flush = 0;

	  std::pair<Ptr<Face>, Address> closestSector;

	  Ptr<Face> outFace;
	  Address destAddr;

	  while (!addrQueue.empty())
	    {
	      // Get the PDU at the front of the queue
	      Ptr<Packet> queuePDU = addrQueue.front();

	      switch(HeaderHelper::GetNNNHeaderType(queuePDU))
	      {
		case DO_NNN:
		  // Convert the Packet back to a DO for manipulation
		  do_o_orig = wire::nnnSIM::DO::FromWire(queuePDU);

		  // Renew the DO lifetime
		  do_o_orig->SetLifetime(m_3n_lifetime);
		  // Change the DO 3N name to the new name
		  do_o_orig->SetName(newName);

		  // Find where to send the SO
		  closestSector = m_nnst->ClosestSectorFaceInfo(newName, 0);

		  outFace = closestSector.first;
		  destAddr = closestSector.second;

		  // Send the created DO PDU
		  outFace->SendDO(do_o_orig, destAddr);
		  // Log the DO sending
		  m_outDOs(do_o_orig, outFace);
		  do_flush++;
		  break;
		case DU_NNN:
		  // Convert the Packet back to a DU for manipulation
		  du_o_orig = wire::nnnSIM::DU::FromWire(queuePDU);

		  // Renew the DU lifetime
		  du_o_orig->SetLifetime(m_3n_lifetime);

		  // Change the DU 3N names to the new names if necessary
		  if (du_o_orig->GetDstName() == *oldName)
		    du_o_orig->SetDstName(newName);

		  if (du_o_orig->GetSrcName() == *oldName)
		    du_o_orig->SetSrcName(newName);

		  // Find where to send the DU
		  closestSector = m_nnst->ClosestSectorFaceInfo(du_o_orig->GetDstNamePtr(), 0);

		  outFace = closestSector.first;
		  destAddr = closestSector.second;

		  // Send the created DU PDU
		  outFace->SendDU(du_o_orig, destAddr);
		  // Log the DU sending
		  m_outDUs(du_o_orig, outFace);
		  du_flush++;
		  break;
		default:
		  NS_LOG_INFO("Obtained unknown PDU");
	      }
	      // Pop the queue and continue
	      addrQueue.pop ();
	    }

	  // Make sure we delete the entry for oldName in the buffer
	  m_node_pdu_buffer->RemoveDestination(oldName);

	  NS_LOG_INFO ("On (" << myAddr << ") flushed (" << *oldName << ") -> (" << *newName << ") <->  DO: " << do_flush << " DU: " << du_flush );
	}
      else
	{
	  NS_LOG_INFO ("On (" << myAddr << ") no buffer found for (" << *oldName << "), continuing");
	}
    }

    void
    ForwardingStrategy::OnEN (Ptr<Face> face, Ptr<EN> en_p)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_inENs (en_p, face);

      // Find if we can produce 3N names
      if (m_produce3Nnames && Has3NName ())
	{
	  NNNAddress myAddr = GetNode3NName ();
	  NS_LOG_INFO("On (" << myAddr << ") producing 3N name for new node");

	  // Get the first Address from the EN PDU (this probably requires more tuning)
	  Address destAddr = en_p->GetOnePoa(0);

	  NS_LOG_INFO ("On (" << myAddr << "), will return OEN to " << destAddr);

	  // Get all the PoA Address in the EN PDU to fill the NNST
	  std::vector<Address> poaAddrs = en_p->GetPoas();

	  NS_LOG_INFO ("Received PoA Names: ");
	  for (int i = 0; i < poaAddrs.size (); i++)
	    {
	      NS_LOG_INFO (i << ": " << poaAddrs[i]);
	    }

	  // Produce a 3N name
	  Ptr<NNNAddress> produced3Nname = produce3NName ();

	  // Add the new information into the Awaiting Response NNST type structure
	  // Create a 5 second timeout - remember absolute time
	  m_awaiting_response->Add(produced3Nname, face, poaAddrs, Simulator::Now () + m_3n_lease_ack_timeout, m_standardMetric);

	  NS_LOG_INFO ("On (" << myAddr << ") creating OEN PDU to send");
	  // Create an OEN PDU to respond
	  Ptr<OEN> oen_p = Create<OEN> (produced3Nname);
	  oen_p->SetLifetime(m_3n_lifetime);
	  // Ensure that the lease time is set in the PDU
	  // We send the lease out in absolute simulator time
	  Time absoluteLease = Simulator::Now () + m_3n_lease_time;
	  oen_p->SetLeasetime(absoluteLease);
	  // Add the PoA names to the PDU
	  oen_p->AddPoa(poaAddrs);
	  // Add my name to the PDU
	  oen_p->SetSrcName (GetNode3NName ());
	  // Add personal PoAs
	  oen_p->AddPersonalPoa (GetAllPoANames (face));

	  // Send the create OEN PDU out the way it came
	  face->SendOEN(oen_p, destAddr);

	  NS_LOG_INFO ("Making a lease entry in (" << myAddr << ") for (" <<*produced3Nname << ") until " << absoluteLease.GetSeconds());

	  // Maintain the lease time given to the 3N name for further checking
	  m_node_lease_times[oen_p->GetNamePtr()] = absoluteLease;

	  m_outOENs (oen_p, face);
	}
      else
	{
	  m_dropENs (en_p, face);
	}
    }

    void
    ForwardingStrategy::OnAEN (Ptr<Face> face, Ptr<AEN> aen_p)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_inAENs (aen_p, face);

      // Find out if we can produce 3N names
      if (m_produce3Nnames)
	{
	  // Get the 3N name from the AEN
	  NNNAddress tmp = aen_p->GetName ();
	  NNNAddress myAddr = GetNode3NName ();

	  NS_LOG_INFO("On (" << myAddr << ") obtained AEN for (" << tmp << ")");
	  // Assure that the name in the AEN is under the delegated 3N name
	  if (myAddr == tmp.getSectorName ())
	    {
	      // Check if we have this entry in the waiting list
	      if (m_awaiting_response->FoundName(tmp))
		{
		  NS_LOG_INFO("Found lease entry for seen (" << tmp << ")");
		  // Get the list of PoAs in the AEN PDU
		  std::vector<Address> receivedPoas = aen_p->GetPoas ();

		  std::vector<Address> sorted_receivedPoas (receivedPoas.size ());
		  std::partial_sort_copy (receivedPoas.begin (), receivedPoas.end (), sorted_receivedPoas.begin (), sorted_receivedPoas.end ());

		  // Get the list of stored PoAs
		  std::vector<Address> storedPoas = m_awaiting_response->GetAllPoas (tmp);

		  std::vector<Address> sorted_storedPoas (storedPoas.size ());
		  std::partial_sort_copy (storedPoas.begin (), storedPoas.end (), sorted_storedPoas.begin (), sorted_storedPoas.end ());

		  // Get the name that will finally be used
		  Ptr<const NNNAddress> newName = aen_p->GetNamePtr ();

		  // Check differences
		  std::vector<Address> diff;

		  // Check if there are any differences - remember the vectors must be sorted!
		  std::set_difference(sorted_receivedPoas.begin (), sorted_receivedPoas.end (),
		                      sorted_storedPoas.begin (), sorted_storedPoas.end (),
		                      std::inserter(diff, diff.begin ()));

		  // There has to be no differences
		  if (diff.size () == 0)
		    {
		      NS_LOG_INFO("Found no differences in PoAs for (" << tmp << ")");

		      Time absoluteLeaseTime = aen_p->GetLeasetime();
		      std::map<Ptr<const NNNAddress>, Time, PtrNNNComp>::iterator it;

		      it = m_node_lease_times.find(newName);

		      // Check that the lease times coincide with what was recorded
		      // Remember that the Wire classes round to Time values to uint16_t so we
		      // will not have the femtoseconds
		      if (it != m_node_lease_times.end ())
			{
			  Time recordedLeaseTime = it->second;

			  NS_LOG_INFO ("Checking discrepancies between recorded time " << recordedLeaseTime << " and obtained time " << absoluteLeaseTime << " in (" << myAddr << ") for (" << tmp << ")");

			  if (absoluteLeaseTime <= recordedLeaseTime)
			    {
			      // No issues
			      NS_LOG_INFO ("Lease time checks out for (" << tmp << ")");
			      m_node_lease_times.erase(newName);
			    }
			  else
			    {
			      NS_LOG_INFO ("Found discrepancy between recorded time and obtained for (" << tmp << ")");
			      return;
			    }
			}
		      else
			{
			  NS_LOG_INFO ("No record found for " << *newName << " returning");
			  return;
			}

		      NS_LOG_INFO("Adding NNST information for (" << tmp << ") until " << absoluteLeaseTime.GetSeconds ());

		      // Add the new information to the NNST
		      m_nnst->Add(newName, face, storedPoas, absoluteLeaseTime, m_standardMetric);

		      NS_LOG_DEBUG ("On (" << myAddr << "): " << *m_nnst);

		      NS_LOG_INFO("Adding lease information for (" << tmp << ") until " << absoluteLeaseTime.GetSeconds ());
		      // Add the information the the leased NodeNameContainer
		      m_leased_names->addEntry(newName, absoluteLeaseTime, false);

		      NS_LOG_INFO("Saving face " << *face << " for REN purposes");
		      // Add the Face where the AEN came from (used for REN purposes)
		      m_returnEN_faces.insert (face);

		      // If there is an NNPT entry, it means we had a REN before. We need to
		      // ensure the network of this change
		      if (m_nnpt->foundNewName (newName))
			{
			  Ptr<NNNAddress> registeredOldName = Create<NNNAddress> (m_nnpt->findPairedOldName(newName));
			  Ptr<NNNAddress> registeredNewName = Create<NNNAddress> (newName->getName());

			  NS_LOG_INFO("We have had a reenrolling node used to go by (" << *registeredOldName << ") now uses (" << *registeredNewName << ")");
			  NS_LOG_INFO("Attempting to flush buffer");
			  // If we happen to be in the same subsector, the buffer will have something
			  // the check is good practice
			  flushBuffer (face, registeredOldName, registeredNewName);

			  // Check if the Node was in our subsector
			  if (! registeredOldName->isSubSector (myAddr))
			    {
			      // If node was not originally in our sector, create a INF PDU
			      NS_LOG_INFO ("(" << *registeredOldName << ") was not in our sector, creating INF PDU");
			      Ptr<INF> inf_o = Create<INF> ();
			      inf_o->SetLifetime(m_3n_lifetime);

			      // Fill the necessary information
			      inf_o->SetOldName (registeredOldName);
			      inf_o->SetNewName (registeredNewName);
			      inf_o->SetRemainLease (m_nnpt->findNameExpireTime(registeredOldName));

			      // Find where to send the INF
			      std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo (registeredOldName->getSectorName(), 0);

			      Ptr<Face> outFace = tmp.first;
			      Address destAddr = tmp.second;

			      // Send the created INF PDU
			      outFace->SendINF (inf_o, destAddr);

			      // Log that the INF PDU was sent
			      m_outINFs (inf_o, outFace);
			    }
			}
		    }
		  else
		    {
		      NS_LOG_INFO ("Error - found differences in PoAs for (" << tmp << "), rejecting!");

		      NS_LOG_INFO ("Received PoAs -");
		      for (int i = 0; i < storedPoas.size (); i++ )
			{
			  NS_LOG_INFO (i << ": " << storedPoas[i]);
			}
		      NS_LOG_INFO ("Store PoAs -");
		      for (int i = 0; i < receivedPoas.size (); i++ )
			{
			  NS_LOG_INFO (i << ": " << receivedPoas[i]);
			}

		      NS_LOG_INFO ("Difference in PoAs -");
		      for (int i = 0; i < diff.size (); i ++)
			{
			  NS_LOG_INFO (i << ": " << diff[i]);
			}
		    }
		}
	    }
	}
    }

    void
    ForwardingStrategy::OnREN (Ptr<Face> face, Ptr<REN> ren_p)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_inRENs (ren_p, face);

      // Check we can actually produce 3N names
      if(m_produce3Nnames && Has3NName ())
	{
	  NNNAddress myAddr = GetNode3NName ();

	  // Get the 3N name that the node was using
	  Ptr<const NNNAddress> reenroll = ren_p->GetNamePtr();

	  NS_LOG_INFO("On (" << myAddr << ") producing 3N name for reenrolling node (" << *reenroll << ")");

	  // Get the first Address from the REN PDU
	  Address destAddr = ren_p->GetOnePoa(0);

	  // Get all the PoA Address in the REN PDU to fill the NNST
	  std::vector<Address> poaAddrs = ren_p->GetPoas ();

	  // Produce a new 3N name
	  Ptr<const NNNAddress> produced3Nname = produce3NName ();

	  // Add the new information into the Awaiting Response NNST type structure
	  // Create a 5 second timeout - must be in absolute simulator time
	  m_awaiting_response->Add(produced3Nname, face, poaAddrs, Simulator::Now () + m_3n_lease_ack_timeout, m_standardMetric);

	  NS_LOG_INFO("On (" << myAddr << ") creating OEN PDU to send");
	  // Create an OEN PDU to respond
	  Ptr<OEN> oen_p = Create<OEN> (produced3Nname->getName());
	  oen_p->SetLifetime(m_3n_lifetime);
	  // Ensure that the lease time is set in the PDU
	  // We send the lease out in absolute simulator time
	  Time absoluteLease = Simulator::Now () + m_3n_lease_time;
	  oen_p->SetLeasetime(absoluteLease);
	  // Add the PoA names to the PDU
	  oen_p->AddPoa(poaAddrs);
	  // Add my name to the PDU
	  oen_p->SetSrcName (GetNode3NName ());
	  // Add personal PoAs
	  oen_p->AddPersonalPoa (GetAllPoANames (face));

	  // Send the create OEN PDU out the way it came
	  face->SendOEN(oen_p, destAddr);

	  m_outOENs (oen_p, face);

	  Time remaining = ren_p->GetRemainLease ();
	  NS_LOG_INFO("On (" << myAddr << ") creating an NNPT entry for (" << *reenroll << ") -> (" << *produced3Nname << ") until " << remaining.GetSeconds ());

	  // Regardless of the name, we need to update the NNPT
	  m_nnpt->addEntry (reenroll, produced3Nname, remaining);

	  NS_LOG_INFO ("Making a lease entry in (" << myAddr << ") for (" <<*produced3Nname << ") until " << absoluteLease.GetSeconds ());
	  // Maintain the lease time given to the 3N name for further checking
	  m_node_lease_times[oen_p->GetNamePtr()] = absoluteLease;
	}
      else
	{
	  m_dropRENs (ren_p, face);
	}
    }

    void
    ForwardingStrategy::OnDEN (Ptr<Face> face, Ptr<DEN> den_p)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_inDENs (den_p, face);

      NNNAddress myAddr = GetNode3NName ();
      Ptr<NNNAddress> leavingAddr = Create<NNNAddress> (den_p->GetNamePtr ()->getName ());

      NS_LOG_INFO ("On (" << myAddr << "), (" << *leavingAddr << ") is leaving");
      NS_LOG_INFO ("Adding (" << *leavingAddr << ") to buffers");

      // We know the node sending the DEN is moving. His lease time will be maintained
      // All we need to do is tell the buffer to keep the packets to that destination
      m_node_pdu_buffer->AddDestination (leavingAddr);

      // If the DEN packet arrives at a node that is less than 2 hops away, then we
      // forward the DEN packet to the parent of this node
      if (leavingAddr->distance (myAddr) <= 2 && leavingAddr->isSubSector (myAddr))
	{
	  NS_LOG_INFO ("We can still attempt to propagate DEN");
	  // Now we forward the DEN information to the higher hierarchical nodes
	  std::vector<std::pair<Ptr<Face>, Address> > hierarchicalFaces = m_nnst->OneHopParentSectorFaceInfo (myAddr, 0);
	  std::vector<std::pair<Ptr<Face>, Address> >::iterator it;

	  Ptr<Face> outFace;
	  Address destAddr;
	  bool propagated = false;
	  bool ok = false;

	  for (it = hierarchicalFaces.begin (); it != hierarchicalFaces.end (); ++it)
	    {
	      outFace = it->first;
	      destAddr = it->second;

	      NS_LOG_INFO ("Pushing DEN out Face " << *outFace);

	      ok = outFace->SendDEN (den_p, destAddr);

	      // Log that the DEN PDU was sent
	      m_outDENs (den_p, outFace);

	      if (ok)
		propagated = true;
	    }

	  if (propagated)
	    NS_LOG_INFO ("On (" << myAddr << ") found parent sectors to propagate DEN to");
	  else
	    NS_LOG_INFO ("On (" << myAddr << ") no parent sectors to propagate DEN to, stopping propagation");
	}
      else
	{
	  NS_LOG_INFO ("On (" << myAddr << ") we have left the sector and are too far, stopping propagation");
	}
    }

    void
    ForwardingStrategy::OnOEN (Ptr<Face> face, Ptr<OEN> oen_p)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_inOENs (oen_p, face);

      Ptr<const NNNAddress> obtainedName = oen_p->GetNamePtr();
      // The OEN PDU sends the lease expiry time in absolute simulator time
      Time lease = oen_p->GetLeasetime();

      NS_LOG_INFO("Obtained with (" << *obtainedName << ") with lease until " << lease.GetSeconds());

      bool willUseName = false;

      // Get a list of this node's PoAs
      std::vector<Address> myPoas = GetAllPoANames (face);

      // Set difference requires the vectors to be sorted. Must unfortunately be done.
      // Create the sorted vector to be the size of myPoas
      std::vector<Address> sorted_myPoas (myPoas.size ());
      std::partial_sort_copy (myPoas.begin (), myPoas.end (), sorted_myPoas.begin (), sorted_myPoas.end ());

      // Get the list of PoAs in the OEN
      std::vector<Address> oenPoas = oen_p->GetPoas ();

      std::vector<Address> sorted_oenPoas (oenPoas.size ());
      std::partial_sort_copy (oenPoas.begin (), oenPoas.end (), sorted_oenPoas.begin (), sorted_oenPoas.end ());

      // Check differences
      std::vector<Address> diff;

      // Check if there are any differences
      std::set_difference(sorted_myPoas.begin (), sorted_myPoas.end (),
                          sorted_oenPoas.begin (), sorted_oenPoas.end (),
                          std::inserter(diff, diff.begin ()));

      if (diff.size () != 0)
	{
	  NS_LOG_INFO ("We found discrepancies with the destination PoAs. This PDU is not for this node");


	  NS_LOG_INFO ("Node PoAs -");
	  for (int i = 0; i < myPoas.size (); i++ )
	    {
	      NS_LOG_INFO (i << ": " << myPoas[i]);
	    }
	  NS_LOG_INFO ("Received PoAs -");
	  for (int i = 0; i < oenPoas.size (); i++ )
	    {
	      NS_LOG_INFO (i << ": " << oenPoas[i]);
	    }

	  NS_LOG_INFO ("Difference in PoAs -");
	  for (int i = 0; i < diff.size (); i ++)
	    {
	      NS_LOG_INFO (i << ": " << diff[i]);
	    }
	}
      else
	{
	  // Check if we have a 3N name
	  if (Has3NName ())
	    {
	      // As long as the name is not the same, we can use the name
	      if (*GetNode3NNamePtr() != *obtainedName)
		{
		  NS_LOG_INFO("Node had (" << GetNode3NName () << ") now taking (" << *obtainedName << ") until " << lease.GetSeconds());
		  SetNode3NName(obtainedName, lease, false);
		  willUseName = true;

		  // Add the information to NNST
		  m_nnst->Add(oen_p->GetSrcNamePtr (), face, oen_p->GetPersonalPoas (), lease, m_standardMetric);
		}
	    }
	  else
	    {
	      NS_LOG_INFO("Node has no name, taking (" << *obtainedName << ") until " << lease.GetSeconds ());
	      SetNode3NName(obtainedName, lease, false);
	      willUseName = true;

	      // Add the information to NNST
	      m_nnst->Add(oen_p->GetSrcNamePtr (), face, oen_p->GetPersonalPoas (), lease, m_standardMetric);
	    }

	  // If you start using the 3N name, execute the following
	  if (willUseName)
	    {
	      m_no_oen = true;
	      NS_LOG_INFO("Pushing AEN with Node name (" << *obtainedName << ") with lease until " << lease.GetSeconds ());
	      // Now create the AEN PDU to respond
	      Ptr<AEN> aen_p = Create<AEN> (*obtainedName);
	      aen_p->SetLifetime(m_3n_lifetime);
	      // Ensure that the lease time is set right (continues to be in absolute simulator time)
	      aen_p->SetLeasetime (lease);
	      // Add the PoAs to the response PDU
	      aen_p->AddPoa(myPoas);

	      // Send the created AEN PDU
	      face->SendAEN(aen_p);

	      m_outAENs (aen_p, face);
	    }
	  else
	    {
	      NS_LOG_INFO ("Will not be using (" << *obtainedName << ")");
	    }
	}
    }

    void
    ForwardingStrategy::OnINF (Ptr<Face> face, Ptr<INF> inf_p)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_inINFs (inf_p, face);

      NNNAddress myAddr = GetNode3NName ();

      Ptr<NNNAddress> oldName = Create<NNNAddress> (inf_p->GetOldName ());
      Ptr<NNNAddress> newName = Create<NNNAddress> (inf_p->GetNewName ());

      NS_LOG_INFO ("On (" << myAddr << ") creating NNPT Entry for Old: (" << *oldName << ") -> New: (" << *newName << ")");

      // Update our NNPT with the information in the INF PDU
      m_nnpt->addEntry (inf_p->GetOldNamePtr (), inf_p->GetNewNamePtr (), inf_p->GetRemainLease ());

      NNNAddress endSector = inf_p->GetOldNamePtr ()->getSectorName ();

      // We have inserted the INF information. Now flush the relevant buffer
      flushBuffer (face, oldName, newName);

      if (myAddr != endSector)
	{
	  NS_LOG_INFO("On (" << myAddr << ") receiving an INF. Have not yet reached delegated Sector. Forwarding to (" << endSector << ")");

	  // Roughly pick the next hop that would bring us closer to the endSector
          std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo (endSector, 0);

          Ptr<Face> outFace = tmp.first;
          Address destAddr = tmp.second;

          outFace->SendINF (inf_p, destAddr);

          // Log that the INF PDU was sent
          m_outINFs (inf_p, outFace);
	}
      else
	{
	  NS_LOG_INFO ("On (" << myAddr << ") reached sector, ending");
	}
    }

    void
    ForwardingStrategy::OnNULLp (Ptr<Face> face, Ptr<NULLp> null_p)
    {
      NS_LOG_FUNCTION (this << face->GetId () << face->GetFlags());

      m_inNULLps (null_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = null_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU> (null_p);

      ProcessICNPDU (pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::OnSO (Ptr<Face> face, Ptr<SO> so_p)
    {
      NS_LOG_FUNCTION (this << face->GetId () << face->GetFlags());

      m_inSOs (so_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = so_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU>(so_p);

      ProcessICNPDU (pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::OnDO (Ptr<Face> face, Ptr<DO> do_p)
    {
      NS_LOG_FUNCTION (this << face->GetId () << face->GetFlags());

      m_inDOs (do_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = do_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU> (do_p);

      ProcessICNPDU (pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::OnDU (Ptr<Face> face, Ptr<DU> du_p)
    {
      NS_LOG_FUNCTION (this << face->GetId () << face->GetFlags());

      m_inDUs (du_p, face);

      //Give us a rw copy of the packet
      Ptr<Packet> icn_pdu = du_p->GetPayload ()->Copy ();

      // To be able to simplify code, convert pointer to common type
      Ptr<NNNPDU> pdu = DynamicCast<NNNPDU> (du_p);

      ProcessICNPDU (pdu, face, icn_pdu);
    }

    void
    ForwardingStrategy::ProcessICNPDU (Ptr<NNNPDU> pdu, Ptr<Face> face, Ptr<Packet> icn_pdu)
    {
      NS_LOG_FUNCTION (this << face->GetId ());
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
	  ProcessData (pdu, face, data);
	}
    }

    void
    ForwardingStrategy::UpdatePITEntry (Ptr<pit::Entry> pitEntry, Ptr<NNNPDU> pdu, Ptr<Face> face, Time lifetime)
    {
      NS_LOG_FUNCTION (this);
      // Pointers for PDU types
      Ptr<SO> so_i;
      Ptr<DU> du_i;

      // Find out the type of PDU we are dealing with
      uint32_t pduid = pdu->GetPacketId ();
      switch(pduid)
      {
	case NULL_NNN:
	  // Add the Face
	  pitEntry->AddIncoming (face);
	  // Show that you received a NULL PDU
	  pitEntry->SetReceivedNULLPDU (true);
	  break;
	case SO_NNN:
	  // Convert pointer to SO
	  so_i = DynamicCast<SO> (pdu);
	  // Add the Face
	  pitEntry->AddIncoming (face, so_i->GetNamePtr ());
	  break;
	case DU_NNN:
	  // Convert pointer to DU
	  du_i = DynamicCast<DU> (pdu);
	  // Add the Face
	  pitEntry->AddIncoming (face, du_i->GetSrcNamePtr ());
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
      NS_LOG_FUNCTION (this << face->GetId () << interest->GetName ());
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
      NS_LOG_FUNCTION (this << face->GetId () << data->GetName ());
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
      NS_LOG_FUNCTION (this << face->GetId ());

      m_faces->Add (face);
    }

    void
    ForwardingStrategy::RemoveFace (Ptr<Face> face)
    {
      NS_LOG_FUNCTION (this << face->GetId ());

      m_faces->Remove (face);
    }

    std::vector<Address>
    ForwardingStrategy::GetAllPoANames (Ptr<Face> face)
    {
      NS_LOG_FUNCTION (this);
      // Vector to save the PoA names
      std::vector<Address> poanames1;
      std::vector<Address> poanames2;
      // Get the total number of addresses we have
      int totalFaces = m_faces->GetN ();
      Ptr<Face> tmp;
      Address tmpaddr;

      // Go through the Faces attached to the ForwardingStrategy
      for (int i = 0; i < totalFaces; i++)
	{
	  // Get a Face
	  tmp = m_faces->Get (i);

	  // Check that the Face is not of type APPLICATION
	  if (!tmp->isAppFace ())
	    {
	      if (face == tmp)
		{
		  tmpaddr = tmp->GetAddress ();
		  poanames1.push_back (tmpaddr);
		}
	      else
		poanames2.push_back (tmp->GetAddress ());
	    }
	}

      for (int i = 0; i < poanames2.size (); i++)
	{
	  poanames1.push_back (poanames2[i]);
	}

      return poanames1;
    }

    void
    ForwardingStrategy::Enroll ()
    {
      NS_LOG_FUNCTION (this);
      // Check whether this node has a 3N name
      if (!Has3NName ())
	{
	  // Temporary test - an enroll means we have no name, fire the traced callback
	  m_no3Nname ();
	  bool ok = false;

	  Ptr<Face> tmp;
	  // Now transmit the EN through all Faces that are not of type APPLICATION
	  for (int i = 0; i < m_faces->GetN (); i++)
	    {
	      // Get a Face
	      tmp = m_faces->Get (i);
	      // Check that the Face is not of type APPLICATION
	      if (!tmp->isAppFace ())
		{
		  NS_LOG_INFO ("Sending out Face " << boost::cref(*tmp) << " with " << tmp->GetAddress ());

		  // Obtain all the node's PoA names
		  std::vector<Address> poanames = GetAllPoANames (tmp);

		  // Create the EN PDU to transmit
		  Ptr<EN> en_o = Create<EN> ();
		  // Set the lifetime for the EN PDU
		  en_o->SetLifetime (m_3n_lifetime);

		  // Add all the PoA names we found
		  for (int i = 0; i < poanames.size (); i++)
		    {
		      NS_LOG_INFO ("Adding PoA name: " << poanames[i]);
		      en_o->AddPoa (poanames[i]);
		    }

		  // Send the EN throughout the Faces
		  ok = tmp->SendEN (en_o);

		  if (ok)
		    m_outENs (en_o, tmp);
		}
	    }

	  NS_LOG_INFO ("Scheduling an enroll should things go south");
	  // Schedule the another enroll, should things go bad
	  Simulator::Schedule (m_ack_timeout, &ForwardingStrategy::Enroll, this);
	}
      else
	{
	  NS_LOG_INFO ("Node has name (" << GetNode3NName () << "), no more need for enrollment for now");
	}
    }

    void
    ForwardingStrategy::Reenroll ()
    {
      NS_LOG_FUNCTION (this);
      std::set <Ptr<Face>, PtrFaceComp>::iterator it;

      // Check whether this node has a 3N name
      if (Has3NName () && !m_no_oen)
	{
	  bool ok = false;
	  Ptr<Face> tmp;
	  // Now transmit the REN through all Faces that are not of type APPLICATION
	  for (int i = 0; i < m_faces->GetN (); i++)
	    {
	      // Get a Face
	      tmp = m_faces->Get (i);
	      // Check to see
	      it = m_returnEN_faces.find (tmp);

	      if (it != m_returnEN_faces.end ())
		{
		  NS_LOG_INFO ("Face " << *tmp << " has been used to answer EN, skipping");
		  continue;
		}

	      // Check that the Face is not of type APPLICATION
	      if (!tmp->isAppFace ())
		{
		  std::vector<Address> poanames = GetAllPoANames (tmp);

		  // Create the REN PDU to transmit
		  Ptr<REN> ren_o = Create<REN> ();
		  Ptr<const NNNAddress> addr = GetNode3NNamePtr ();

		  // Set the lifetime for the REN PDU
		  ren_o->SetLifetime (m_3n_lifetime);
		  // Set the 3N name for the REN
		  ren_o->SetName (*addr);
		  // Write the expire time for the 3N name (Time within the simulator is absolute)
		  ren_o->SetRemainLease (m_node_names->findNameExpireTime (addr));
		  // Add all the PoA names we found
		  for (int i = 0; i < poanames.size (); i++)
		    {
		      ren_o->AddPoa (poanames[i]);
		    }

		  NS_LOG_INFO ("Sending out REN via " << *tmp << " with (" << GetNode3NName () << ")");


		  // Send the REN throughout the Faces
		  ok = tmp->SendREN (ren_o);

		  if (ok)
		    m_outRENs (ren_o, tmp);

		  NS_LOG_INFO ("Scheduling an reenroll should things go south");
		  // Schedule the another enroll, should things go bad
		  Simulator::Schedule (m_ack_timeout, &ForwardingStrategy::Reenroll, this);
		}
	    }
	}
      else
	{
	  NS_LOG_INFO ("On (" << GetNode3NName () << ") have reenrolled, flagging");
	  m_no_oen = false;
	}
    }

    void
    ForwardingStrategy::Disenroll ()
    {
      NS_LOG_FUNCTION (this);
      // Check whether this node has a 3N name
      if (Has3NName ())
	{

	  bool ok = false;

	  Ptr<Face> tmp;
	  // Now transmit the DEN through all Faces that are not of type APPLICATION
	  for (int i = 0; i < m_faces->GetN (); i++)
	    {
	      // Get a Face
	      tmp = m_faces->Get (i);
	      // Check that the Face is not of type APPLICATION
	      if (!tmp->isAppFace ())
		{
		  std::vector<Address> poanames = GetAllPoANames (tmp);

		  // Create the REN PDU to transmit
		  Ptr<DEN> den_o = Create<DEN> ();
		  Ptr<const NNNAddress> addr = GetNode3NNamePtr ();

		  // Set the lifetime for the REN PDU
		  den_o->SetLifetime (m_3n_lifetime);
		  // Set the 3N name for the REN
		  den_o->SetName (*addr);
		  // Add all the PoA names we found
		  for (int i = 0; i < poanames.size (); i++)
		    {
		      den_o->AddPoa (poanames[i]);
		    }
		  // Send the REN throughout the Faces
		  ok = tmp->SendDEN (den_o);

		  if (ok)
		    m_outDENs (den_o, tmp);
		}
	    }
	}
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
    ForwardingStrategy::DidExhaustForwardingOptions (Ptr<NNNPDU> pdu,
                                                     Ptr<Face> inFace,
                                                     Ptr<const ndn::Interest> interest,
                                                     Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this << boost::cref (*inFace));
      if (pitEntry->AreAllOutgoingInVain ())
	{
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

	  m_dropInterests (interest, inFace);

	  if (wasNULL)
	    m_dropNULLps (nullp_i, inFace);
	  else if (wasSO)
	    m_dropSOs (so_i, inFace);
	  else if (wasDO)
	    m_dropDOs (do_i, inFace);
	  else if (wasDU)
	    m_dropDUs (du_i, inFace);

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

      NNNAddress myAddr = GetNode3NName ();
      NS_LOG_INFO ("On (" << myAddr << ") Satisfying pending Interests for " << data->GetName());

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

      uint32_t pduid = pdu->GetPacketId ();
      switch(pduid)
      {
	case NULL_NNN:
	  // Convert pointer to NULLp PDU
	  nullp_i = DynamicCast<NULLp> (pdu);
	  wasNULL = true;
	  NS_LOG_INFO ("Received a NULLp");
	  break;
	case SO_NNN:
	  // Convert pointer to SO PDU
	  so_i = DynamicCast<SO> (pdu);
	  wasSO = true;
	  NS_LOG_INFO ("Received a SO");
	  break;
	case DO_NNN:
	  // Convert pointer to DO PDU
	  do_i = DynamicCast<DO> (pdu);
	  wasDO = true;
	  NS_LOG_INFO ("Received a DO");
	  break;
	case DU_NNN:
	  // Convert pointer to DU PDU
	  du_i = DynamicCast<DU> (pdu);
	  wasDU = true;
	  NS_LOG_INFO ("Received a DU");
	  break;
	default:
	  break;
      }

      // Satisfy all pending Interests with the Data we received on each Face
      BOOST_FOREACH (const pit::IncomingFace &incoming, pitEntry->GetIncoming ())
      {
	bool ok = false;
	bool sentSomething = false;

	NS_LOG_INFO ("Satisfying for Face " << incoming.m_face->GetId() << " of type " << incoming.m_face->GetFlags() << " at (" << GetNode3NName () << ")");

	/////////////////////////////////////////////////////////////////////////////////////////
	// Obtain the distinct 3N names associated to this Face and go through them
	std::vector<Ptr<NNNAddress> > distinct = incoming.m_addrs->GetDistinctDestinations ();
	// It is possible for the face to have no destinations
	if (distinct.empty())
	  {
	    NS_LOG_INFO ("Our PIT has no 3N names aggregated");
	    // If we had received a NULL PDU with this Interest at some point, and have no 3N names
	    // saved, return a NULL PDU with the information
	    if (pitEntry->GetReceivedNULLPDU() && !sentSomething)
	      {
		NS_LOG_INFO ("Satisfying previous sent NULL PDU");
		Ptr<NULLp> null_p_o = Create<NULLp> ();
		// Set the lifetime of the 3N PDU
		null_p_o->SetLifetime (m_3n_lifetime);
		// Configure payload for PDU
		null_p_o->SetPayload (icn_pdu);
		// Signal that the PDU had an ICN PDU as payload
		null_p_o->SetPDUPayloadType (NDN_NNN);

		// Send out the NULL PDU
		ok = incoming.m_face->SendNULLp (null_p_o);

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
	    NS_LOG_INFO ("Our PIT has 3N names aggregated");

	    // There is at least one 3N name in this list - go through the code
	    BOOST_FOREACH (Ptr<NNNAddress> j, distinct)
	    {
	      bool subSector = m_node_names->foundName(j);
	      NNNAddress newdst;
	      // Obtain all the 3N names aggregated in this sector
	      std::vector<Ptr<NNNAddress> > addrs = incoming.m_addrs->GetCompleteDestinations (j);

	      BOOST_FOREACH (Ptr<NNNAddress> i, addrs)
	      {
		// If the aggregation is the same as the 3N Name the node is using, then
		// everything aggregated is probably connected to it
		if (subSector)
		  {
		    NS_LOG_INFO ("We are satisfying Interests to nodes that are directly connected to us, continue");
		  }
		// We are pushing to a different sector
		else
		  {

		    NS_LOG_INFO ("On (" << myAddr << ") satisfying for 3N names in different subsector (" << *j << "), destination: (" << *i << ")");
		    if (sentSomething)
		      {
			NS_LOG_INFO ("On (" << myAddr << "), we seem to have already sent to subsector (" << *j << ") about to skip");
			continue;
		      }
		  }

		// First check to see if we happen to be the destination
		if (m_node_names->foundName(i))
		  {
		    NS_LOG_INFO ("We are the desired 3N named node destination");
		    // This also happens to mean that we satisfying an Application - do not know if
		    // this holds in future references
		    if (wasNULL)
		      {
			ok = incoming.m_face->SendNULLp (nullp_i);
			// Log that a DO PDU was sent
			m_outNULLps (nullp_i, incoming.m_face);
		      }
		    else if (wasSO)
		      {
			ok = incoming.m_face->SendSO (so_i);
			// Log that a DO PDU was sent
			m_outSOs (so_i, incoming.m_face);
		      }
		    else if (wasDO)
		      {
			ok = incoming.m_face->SendDO (do_i);
			// Log that a DO PDU was sent
			m_outDOs (do_i, incoming.m_face);
		      }
		    else if (wasDU)
		      {
			ok = incoming.m_face->SendDU (du_i);
			// Log that a DO PDU was sent
			m_outDUs (du_i, incoming.m_face);
		      }

		    // Log that a Data PDU was sent
		    DidSendOutData (inFace, incoming.m_face, data, pitEntry);

		    // Something caused an error
		    if (!ok)
		      {
			// Log Data drops
			m_dropData (data, incoming.m_face);
			// Log the type of 3N Data transfer PDU that was dropped
			if (wasNULL)
			  m_dropNULLps (nullp_i, incoming.m_face);
			else if (wasSO)
			  m_dropSOs (so_i, incoming.m_face);
			else if (wasDO)
			  m_dropDOs (do_i, incoming.m_face);
			else if (wasDU)
			  m_dropDUs (du_i, incoming.m_face);

			NS_LOG_DEBUG ("Cannot satisfy data to " << *i << " via "<< *incoming.m_face);
		      }

		    sentSomething = true;
		  }

		// Go through the normal forwarding methods

		// We may have obtained a DEN so we need to check
		if (m_node_pdu_buffer->DestinationExists (i) && !m_nnpt->foundOldName(i))
		  {
		    NS_LOG_INFO ("We are on (" << myAddr << ") we have been told to buffer this PDU to (" << *i << ")");

		    if (wasDO)
		      {
			NS_LOG_INFO ("Buffering DO");
			m_node_pdu_buffer->PushDO (i, do_i);
		      }
		    else if (wasDU)
		      {
			NS_LOG_INFO ("Buffering DU");
			m_node_pdu_buffer->PushDU (i, du_i);
		      }
		  }

		// Check if the NNPT has any information for this particular 3N name
		// Retrieve the new 3N name destination
		newdst = m_nnpt->findPairedNamePtr (i)->getName ();

		NS_LOG_INFO ("On (" << myAddr << ") Going to look at NNST size: " << m_nnst->GetSize());
		NS_LOG_INFO (*m_nnst);

		// Roughly pick the next hop that would bring us closer to newdst
		std::pair<Ptr<Face>, Address> tmp = m_nnst->ClosestSectorFaceInfo (newdst, 0);

		Ptr<Face> outFace = tmp.first;
		Address destAddr = tmp.second;

		if (wasNULL || wasSO || wasDO)
		  {
		    NS_LOG_INFO ("Satisfying for 3N name (" << *i << ") using DO in subsector");
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
		    do_o_spec->SetPDUPayloadType (NDN_NNN);

		    // Send the DO PDU out the selected Face
		    ok = outFace->SendDO (do_o_spec, destAddr);

		    // Log that a Data PDU was sent
		    DidSendOutData (inFace, outFace, data, pitEntry);
		    // Log that a DO PDU was sent
		    m_outDOs (do_o_spec, outFace);

		    // Something caused an error
		    if (!ok)
		      {
			// Log Data drops
			m_dropData (data, incoming.m_face);
			// Log DO PDU drop
			m_dropDOs (do_o_spec, outFace);
			NS_LOG_DEBUG ("Cannot satisfy data to (" << newdst << ") via "<< *incoming.m_face);
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
		    NS_LOG_INFO ("Satisfying for 3N name (" << *i << ") using DU in subsector");
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
		    du_o_spec->SetPDUPayloadType (NDN_NNN);

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

		// If we had received a NULL PDU with this Interest at some point, and still
		// haven't pushed anything in previous sections, we should do it now
		if (pitEntry->GetReceivedNULLPDU() && !sentSomething)
		  {
		    Ptr<NULLp> null_p_o = Create<NULLp> ();
		    // Set the lifetime of the 3N PDU
		    null_p_o->SetLifetime (m_3n_lifetime);
		    // Configure payload for PDU
		    null_p_o->SetPayload (icn_pdu);
		    // Signal that the PDU had an ICN PDU as payload
		    null_p_o->SetPDUPayloadType (NDN_NNN);

		    // Send out the NULL PDU
		    ok = incoming.m_face->SendNULLp (null_p_o);

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
	      // Reset for each distinct 3N subsector
	      sentSomething = false;
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
    ForwardingStrategy::TrySendOutInterest (Ptr<NNNPDU> pdu,
					    Ptr<Face> inFace,
					    Ptr<Face> outFace,
					    Address addr,
					    Ptr<const ndn::Interest> interest,
					    Ptr<pit::Entry> pitEntry)
    {
      NS_LOG_FUNCTION (this);

      // Convert the Interest PDU into a NS-3 Packet
      Ptr<Packet> icn_pdu = ndn::Wire::FromInterest (interest);

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

      // Check if we are allowed to retransmit through the selected Face
      if (!CanSendOutInterest (inFace, outFace, interest, pitEntry))
	{
	  return false;
	}

      // Update the PIT Entry with the Outgoing selected Face
      pitEntry->AddOutgoing (outFace);

      // Flag to know if what we sent was successful
      bool successSend = false;

      // Depending on the PDU type, send
      if (wasNULL)
	successSend = outFace->SendNULLp (nullp_i);
      else if (wasSO)
	successSend = outFace->SendSO (so_i);
      else if (wasDO)
	successSend = outFace->SendDO (do_i, addr);
      else if (wasDU)
	successSend = outFace->SendDU (du_i, addr);

      // Check if our sending was successful
      if (!successSend)
	{
	  // Log that an Interest PDU was dropped
	  m_dropInterests (interest, outFace);
	  // Log the type of 3N Data transfer PDU that was dropped
	  if (wasNULL)
	    m_dropNULLps (nullp_i, outFace);
	  else if (wasSO)
	    m_dropSOs (so_i, outFace);
	  else if (wasDO)
	    m_dropDOs (do_i, outFace);
	  else if (wasDU)
	    m_dropDUs (du_i, outFace);
	}

      // Log that an Interest PDU was forwarded
      DidSendOutInterest (inFace, outFace, interest, pitEntry);
      // Log the type of 3N Data transfer PDU that was forwarded
      if (wasNULL)
	m_outNULLps (nullp_i, outFace);
      else if (wasSO)
	m_outSOs (so_i, outFace);
      else if (wasDO)
	m_outDOs (do_i, outFace);
      else if (wasDU)
	m_outDUs (du_i, outFace);

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

      // Update the PIT information with the PDU information

      // Add the Incoming face as normal
      pitEntry->AddIncoming (inFace/*, interest->GetInterestLifetime ()*/);
      // Pointers and flags for PDU types
      Ptr<SO> so_i;
      Ptr<DU> du_i;

      uint32_t pduid = pdu->GetPacketId();
      switch(pduid)
      {
	case NULL_NNN:
	  // Flag the PIT to know that we had a NULLp
	  pitEntry->SetReceivedNULLPDU(true);
	  break;
	case SO_NNN:
	  // Convert pointer to SO PDU
	  so_i = DynamicCast<SO> (pdu);
	  // Add the 3N name in the SO
	  pitEntry->AddIncoming(inFace, so_i->GetNamePtr());
	  break;
	case DU_NNN:
	  // Convert pointer to DU PDU
	  du_i = DynamicCast<DU> (pdu);
	  // Add the 3N name in the DU
	  pitEntry->AddIncoming(inFace, du_i->GetSrcNamePtr());
	  break;
	default:
	  break;
      }

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
	  DidExhaustForwardingOptions (pdu, inFace, interest, pitEntry);
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

      // Get the number of Faces available
      int totalFaces = pitEntry->GetFibEntry ()->m_faces.size ();

      // Convert the Interest PDU into a NS-3 Packet
      Ptr<Packet> icn_pdu = ndn::Wire::FromInterest (interest);

      // Pointers and flags for PDU types
      Ptr<NNNPDU> pdu_i;

      Ptr<DO> do_i;
      bool wasDO = false;
      Ptr<DU> du_i;
      bool wasDU = false;

      // Since we are using a combination of ICN/3N, we copy the pointer and
      // see if 3N gives us a different Face to use. This will be used
      // specifically when the PDU has a destination, in other words when using
      // DO and DU PDUs
      Ptr<Face> foutFace;

      uint32_t pduid = pdu->GetPacketId();
      switch(pduid)
      {
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

      // Pointers to use when we have DO or DU PDUs
      std::pair<Ptr<Face>, Address> tmp;
      Address destAddr;
      NNNAddress newdst;
      Ptr<NNNAddress> newdstPtr;
      Ptr<const NNNAddress> constdstPtr;
      Ptr<DO> do_o_spec;
      Ptr<DU> du_o_spec;
      bool nnptRedirect = false;

      // The propagation rules are a little different if using a DO or DU PDU
      if (wasDO || wasDU)
	{
	  // First obtain the name
	  if (wasDO)
	    {
	      NS_LOG_INFO ("DoPropagateInterest : was DO");
	      newdst = do_i->GetName ();
	      constdstPtr = do_i->GetNamePtr();
	    }
	  else if (wasDU)
	    {
	      NS_LOG_INFO ("DoPropagateInterest : was DU");
	      newdst = du_i->GetDstName ();
	      constdstPtr = du_i->GetDstNamePtr();
	    }

	  // Check if we are already at the destination (search through all the node names acquired)
	  if (m_node_names->foundName(constdstPtr))
	    {
	      // We have reached the destination return
	      return true;
	    }

	  // We may have obtained a DEN so we need to check
	  if (m_node_pdu_buffer->DestinationExists (newdst) && !m_nnpt->foundOldName(constdstPtr))
	    {
	      NS_LOG_INFO ("We are on (" << GetNode3NName () << ") we have been told to buffer this PDU to (" << newdst << ")");

	      if (wasDO)
		{
		  NS_LOG_INFO ("Buffering DO");
		  m_node_pdu_buffer->PushDO (newdst, do_i);
		}
	      else if (wasDU)
		{
		  NS_LOG_INFO ("Buffering DU");
		  m_node_pdu_buffer->PushDU (newdst, du_i);
		}

	      // Having saved the buffer return
	      return true;
	    }

	  // Check if the NNPT has any information for this particular 3N name
	  if (m_nnpt->foundOldName(constdstPtr))
	    {
	      // Retrieve the new 3N name destination and update variable
	      newdst = m_nnpt->findPairedNamePtr (constdstPtr)->getName ();
	      // Flag that the NNPT made a change
	      nnptRedirect = true;
	    }

	  if (nnptRedirect)
	    {
	      if (wasDO)
		{
		  // Create a new DO PDU to send the data
		  do_o_spec = Create<DO> ();
		  // Set the new 3N name
		  do_o_spec->SetName (newdst);
		  // Set the lifetime of the 3N PDU
		  do_o_spec->SetLifetime (m_3n_lifetime);
		  // Configure payload for PDU
		  do_o_spec->SetPayload (icn_pdu);
		  // Signal that the PDU had an ICN PDU as payload
		  do_o_spec->SetPDUPayloadType (NNN_NNN);

		  pdu_i = DynamicCast<NNNPDU> (do_o_spec);
		}
	      else if (wasDU)
		{
		  // Create a new DU PDU to send the data
		  du_o_spec = Create<DU> ();
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

		  pdu_i = DynamicCast<NNNPDU> (du_o_spec);
		}
	    }
	  else
	    {
	      pdu_i = pdu;
	    }


	  for (int j = 0; j < totalFaces; j++)
	    {
	      // Roughly find the next hop
	      tmp = m_nnst->ClosestSectorFaceInfo (newdst, j);

	      // Update the variables for Face and PoA name
	      foutFace = tmp.first;
	      destAddr = tmp.second;

	      if (TrySendOutInterest(pdu_i, inFace, foutFace, destAddr, interest, pitEntry))
		{
		  propagatedCount++;
		  break;
		}
	    }
	}
      // For everything else, propagate like always
      else
	{
	  NS_LOG_INFO ("DoPropagateInterest : Propagating SO or NULLp");

	  // Here we pick the next place to forward to using the ICN strategy
	  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
	  {
	    NS_LOG_DEBUG ("Trying " << boost::cref(metricFace));
	    if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED ||
		metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_YELLOW)
	      break; //propagate only to green faces

	    // Now we actually attempt to forward the PDU
	    if (!TrySendOutInterest (pdu, inFace, metricFace.GetFace (), Address(), interest, pitEntry))
	      {
		continue;
	      }
	    propagatedCount++;
	    break; // propagate only one interest
	  }

	  // If filtering the Yellow and Red Faces, didn't let us send anything out, attempt the
	  // using the Yellow Faces.
	  if (!(propagatedCount > 0))
	    {
	      BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
	      {
		NS_LOG_DEBUG ("Trying " << boost::cref(metricFace));
		if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in the front of the list
		  break;

		if (!TrySendOutInterest (pdu, inFace, metricFace.GetFace (), Address(), interest, pitEntry))
		  {
		    continue;
		  }

		propagatedCount++;
	      }
	    }
	}

      return propagatedCount > 0;
    }

    void
    ForwardingStrategy::NotifyNewAggregate ()
    {
      NS_LOG_FUNCTION(this);

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
