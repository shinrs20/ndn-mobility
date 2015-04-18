/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Ilya Moiseenko
 *
 *  nnn-consumer.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-consumer.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-consumer.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ns3-dev/ns3/boolean.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/double.h>
#include <ns3-dev/ns3/integer.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/string.h>
#include <ns3-dev/ns3/uinteger.h>

#include <ns3-dev/ns3/ndn-name.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndn-header-helper.h>
#include <ns3-dev/ns3/ndn-wire.h>
#include <ns3-dev/ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h>
#include <ns3-dev/ns3/ndnSIM/utils/ndn-rtt-estimator.h>
#include <ns3-dev/ns3/ndnSIM/utils/ndn-rtt-mean-deviation.h>

#include <boost/ref.hpp>

#include "../model/nnn-app-face.h"
#include "nnn-consumer.h"
#include "../model/fw/nnn-forwarding-strategy.h"

NS_LOG_COMPONENT_DEFINE ("nnn.Consumer");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (Consumer);

    TypeId
    Consumer::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::Consumer")
	  .SetGroupName ("Nnn")
	  .SetParent<App> ()
	  .AddAttribute ("StartSeq", "Initial sequence number",
			 IntegerValue (0),
			 MakeIntegerAccessor(&Consumer::m_seq),
			 MakeIntegerChecker<int32_t>())
	  .AddAttribute ("Prefix","Name of the Interest",
			 StringValue ("/"),
			 MakeNameAccessor (&Consumer::m_interestName),
			 ndn::MakeNameChecker ())
	  .AddAttribute ("LifeTime", "LifeTime for interest packet",
			 StringValue ("2s"),
			 MakeTimeAccessor (&Consumer::m_interestLifeTime),
			 MakeTimeChecker ())
	  .AddAttribute ("RetxTimer",
			 "Timeout defining how frequent retransmission timeouts should be checked",
			 StringValue ("50ms"),
			 MakeTimeAccessor (&Consumer::GetRetxTimer, &Consumer::SetRetxTimer),
			 MakeTimeChecker ())
	  .AddAttribute ("UseSO", "Node using 3N SO PDUs",
			 BooleanValue (false),
			 MakeBooleanAccessor(&Consumer::m_useSO),
			 MakeBooleanChecker ())
	  .AddTraceSource ("LastRetransmittedInterestDataDelay", "Delay between last retransmitted Interest and received Data",
			 MakeTraceSourceAccessor (&Consumer::m_lastRetransmittedInterestDataDelay))
	  .AddTraceSource ("FirstInterestDataDelay", "Delay between first transmitted Interest and received Data",
			 MakeTraceSourceAccessor (&Consumer::m_firstInterestDataDelay))
	;
      return tid;
    }

    Consumer::Consumer ()
    : m_rand (0, std::numeric_limits<uint32_t>::max ())
    , m_seq (0)
    , m_seqMax (0) // don't request anything
    , m_useSO (false)
    , m_possibleDestination (Create<NNNAddress> ())
    {
      NS_LOG_FUNCTION_NOARGS ();

      m_rtt = CreateObject<ndn::RttMeanDeviation> ();
    }

    void
    Consumer::SetRetxTimer (Time retxTimer)
    {
      m_retxTimer = retxTimer;
      if (m_retxEvent.IsRunning ())
	{
	  // m_retxEvent.Cancel (); // cancel any scheduled cleanup events
	  Simulator::Remove (m_retxEvent); // slower, but better for memory
	}

      // schedule even with new timeout
      m_retxEvent = Simulator::Schedule (m_retxTimer,
					 &Consumer::CheckRetxTimeout, this);
    }

    Time
    Consumer::GetRetxTimer () const
    {
      return m_retxTimer;
    }

    void
    Consumer::CheckRetxTimeout ()
    {
      Time now = Simulator::Now ();

      Time rto = m_rtt->RetransmitTimeout ();
      // NS_LOG_DEBUG ("Current RTO: " << rto.ToDouble (Time::S) << "s");

      while (!m_seqTimeouts.empty ())
	{
	  SeqTimeoutsContainer::index<i_timestamp>::type::iterator entry =
	      m_seqTimeouts.get<i_timestamp> ().begin ();
	  if (entry->time + rto <= now) // timeout expired?
	    {
	      uint32_t seqNo = entry->seq;
	      m_seqTimeouts.get<i_timestamp> ().erase (entry);
	      OnTimeout (seqNo);
	    }
	  else
	    break; // nothing else to do. All later packets need not be retransmitted
	}

      m_retxEvent = Simulator::Schedule (m_retxTimer,
					 &Consumer::CheckRetxTimeout, this);
    }

    // Application Methods
    void
    Consumer::StartApplication () // Called at time specified by Start
    {
      NS_LOG_FUNCTION_NOARGS ();

      // do base stuff
      App::StartApplication ();

      ScheduleNextPacket ();
    }

    void
    Consumer::StopApplication () // Called at time specified by Stop
    {
      NS_LOG_FUNCTION_NOARGS ();

      // cancel periodic packet generation
      Simulator::Cancel (m_sendEvent);

      // cleanup base stuff
      App::StopApplication ();
    }

    void
    Consumer::SendPacket ()
    {
      if (!m_active) return;

      NS_LOG_FUNCTION_NOARGS ();

      uint32_t seq=std::numeric_limits<uint32_t>::max (); //invalid

      while (m_retxSeqs.size ())
	{
	  seq = *m_retxSeqs.begin ();
	  m_retxSeqs.erase (m_retxSeqs.begin ());
	  break;
	}

      if (seq == std::numeric_limits<uint32_t>::max ())
	{
	  if (m_seqMax != std::numeric_limits<uint32_t>::max ())
	    {
	      if (m_seq >= m_seqMax)
		{
		  return; // we are totally done
		}
	    }

	  seq = m_seq++;
	}

      Ptr<ndn::Name> nameWithSequence = Create<ndn::Name> (m_interestName);
      nameWithSequence->appendSeqNum (seq);

      Ptr<ndn::Interest> interest = Create<ndn::Interest> ();
      interest->SetNonce               (m_rand.GetValue ());
      interest->SetName                (nameWithSequence);
      interest->SetInterestLifetime    (m_interestLifeTime);

      WillSendOutInterest (seq);

      ndn::FwHopCountTag hopCountTag;
      interest->GetPayload ()->AddPacketTag (hopCountTag);

      m_transmittedInterests (interest, this, m_face);

      // Encode the packet
      Ptr<Packet> retPkt = ndn::Wire::FromInterest(interest, ndn::Wire::WIRE_FORMAT_NDNSIM);

      // If not mobile, then we can send NULLp packets
      if (m_useSO && m_has3Nname)
	{
	  NS_LOG_INFO ("> Interest for " << seq << " using SO PDU");
	  Ptr<SO> so_o = Create<SO> ();
	  so_o->SetPDUPayloadType(NDN_NNN);
	  so_o->SetPayload(retPkt);
	  so_o->SetName(*current3Nname);
	  so_o->SetLifetime(m_3n_lifetime);

	  m_face->ReceiveSO(so_o);
	  m_transmittedSOs (so_o, this, m_face);
	}
      else
	{
	  NS_LOG_INFO ("> Interest for " << seq << " using NULLp PDU");
	  Ptr<NULLp> nullp_o = Create<NULLp> ();

	  nullp_o->SetPDUPayloadType (NDN_NNN);
	  nullp_o->SetPayload (retPkt);
	  nullp_o->SetLifetime(m_3n_lifetime);

	  m_face->ReceiveNULLp(nullp_o);
	  m_transmittedNULLps (nullp_o, this, m_face);
	}

      ScheduleNextPacket ();
    }

    ///////////////////////////////////////////////////
    //          Process incoming packets             //
    ///////////////////////////////////////////////////

    void
    Consumer::Deencapsulate3N(Ptr<Packet> packet)
    {
      NS_LOG_FUNCTION (this << packet);

      try
      {
	  ndn::HeaderHelper::Type type = ndn::HeaderHelper::GetNdnHeaderType (packet);
	  Ptr<ndn::Data> data = 0;
	  Ptr<ndn::Interest> interest = 0;
	  switch (type)
	  {
	    case ndn::HeaderHelper::CONTENT_OBJECT_NDNSIM:
	      data = ndn::Wire::ToData (packet, ndn::Wire::WIRE_FORMAT_NDNSIM);
	      break;
	    case ndn::HeaderHelper::CONTENT_OBJECT_CCNB:
	      data = ndn::Wire::ToData (packet, ndn::Wire::WIRE_FORMAT_CCNB);
	      break;
	    case ndn::HeaderHelper::INTEREST_NDNSIM:
	      interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_NDNSIM);
	      break;
	    case ndn::HeaderHelper::INTEREST_CCNB:
	      interest = ndn::Wire::ToInterest (packet, ndn::Wire::WIRE_FORMAT_CCNB);
	      break;
	  }

	  if (data != 0)
	    OnData(data);

	  if (interest != 0)
	    OnNack(interest);

	  // exception will be thrown if packet is not recognized
      }
      catch (ndn::UnknownHeaderException)
      {
	  NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
      }
    }

    void
    Consumer::OnData (Ptr<const ndn::Data> data)
    {
      if (!m_active) return;

      App::OnData (data); // tracing inside

      NS_LOG_FUNCTION (this << data);

      // NS_LOG_INFO ("Received content object: " << boost::cref(*data));

      uint32_t seq = data->GetName ().get (-1).toSeqNum ();
      NS_LOG_INFO ("< DATA for " << seq);

      int hopCount = -1;
      ndn::FwHopCountTag hopCountTag;
      if (data->GetPayload ()->PeekPacketTag (hopCountTag))
	{
	  hopCount = hopCountTag.Get ();
	}

      SeqTimeoutsContainer::iterator entry = m_seqLastDelay.find (seq);
      if (entry != m_seqLastDelay.end ())
	{
	  m_lastRetransmittedInterestDataDelay (this, seq, Simulator::Now () - entry->time, hopCount);
	}

      entry = m_seqFullDelay.find (seq);
      if (entry != m_seqFullDelay.end ())
	{
	  Time delay = Simulator::Now () - entry->time;
	  NS_LOG_INFO ("< DATA for " << seq << " delay: " << delay.GetSeconds());
	  m_firstInterestDataDelay (this, seq, delay, m_seqRetxCounts[seq], hopCount);
	}

      m_seqRetxCounts.erase (seq);
      m_seqFullDelay.erase (seq);
      m_seqLastDelay.erase (seq);

      m_seqTimeouts.erase (seq);
      m_retxSeqs.erase (seq);

      m_rtt->AckSeq (SequenceNumber32 (seq));
    }

    void
    Consumer::OnNULLp (Ptr<const NULLp> nullpObject)
    {
      if (!m_active) return;

      App::OnNULLp(nullpObject);

      NS_LOG_FUNCTION (this << nullpObject);

      Ptr<Packet> packet = nullpObject->GetPayload ()->Copy ();
      uint16_t pdutype = nullpObject->GetPDUPayloadType ();

      NS_LOG_INFO (this << " obtained pdu type " << pdutype);

      if (pdutype == NDN_NNN)
	{
	  Deencapsulate3N(packet);
	}
    }

    void
    Consumer::OnSO (Ptr<const SO> soObject)
    {
      if (!m_active) return;

      App::OnSO(soObject);

      NS_LOG_FUNCTION (this << soObject);

      Ptr<Packet> packet = soObject->GetPayload ()->Copy ();
      uint16_t pdutype = soObject->GetPDUPayloadType ();

      NS_LOG_INFO (this << " obtained pdu type " << pdutype);

      if (pdutype == NDN_NNN)
	{
	  m_possibleDestination = soObject->GetNamePtr();
	  Deencapsulate3N(packet);
	}
    }

    void
    Consumer::OnDO (Ptr<const DO> doObject)
    {
      if (!m_active) return;

      App::OnDO(doObject);

      NS_LOG_FUNCTION (this << doObject);

      Ptr<Packet> packet = doObject->GetPayload ()->Copy ();
      uint16_t pdutype = doObject->GetPDUPayloadType ();

      NS_LOG_INFO (this << " obtained pdu type " << pdutype);

      if (pdutype == NDN_NNN)
	{
	  Deencapsulate3N(packet);
	}
    }

    void
    Consumer::OnDU (Ptr<const DU> duObject)
    {
      if (!m_active) return;

      App::OnDU(duObject);

      NS_LOG_FUNCTION (this << duObject);

      Ptr<Packet> packet = duObject->GetPayload ()->Copy ();
      uint16_t pdutype = duObject->GetPDUPayloadType ();

      NS_LOG_INFO (this << " obtained pdu type " << pdutype);

      if (pdutype == NDN_NNN)
	{
	  m_possibleDestination = duObject->GetSrcNamePtr();
	  Deencapsulate3N(packet);
	}
    }

    void
    Consumer::OnNack (Ptr<const ndn::Interest> interest)
    {
      if (!m_active) return;

      App::OnNack (interest); // tracing inside

      // NS_LOG_DEBUG ("Nack type: " << interest->GetNack ());

      // NS_LOG_FUNCTION (interest->GetName ());

      // NS_LOG_INFO ("Received NACK: " << boost::cref(*interest));
      uint32_t seq = interest->GetName ().get (-1).toSeqNum ();
      NS_LOG_INFO ("< NACK for " << seq);
      // std::cout << Simulator::Now ().ToDouble (Time::S) << "s -> " << "NACK for " << seq << "\n";

      // put in the queue of interests to be retransmitted
      // NS_LOG_INFO ("Before: " << m_retxSeqs.size ());
      m_retxSeqs.insert (seq);
      // NS_LOG_INFO ("After: " << m_retxSeqs.size ());

      m_seqTimeouts.erase (seq);

      m_rtt->IncreaseMultiplier ();             // Double the next RTO ??
      ScheduleNextPacket ();
    }

    void
    Consumer::OnTimeout (uint32_t sequenceNumber)
    {
      NS_LOG_FUNCTION (sequenceNumber);
      // std::cout << Simulator::Now () << ", TO: " << sequenceNumber << ", current RTO: " << m_rtt->RetransmitTimeout ().ToDouble (Time::S) << "s\n";

      m_rtt->IncreaseMultiplier ();             // Double the next RTO
      m_rtt->SentSeq (SequenceNumber32 (sequenceNumber), 1); // make sure to disable RTT calculation for this sample
      m_retxSeqs.insert (sequenceNumber);
      ScheduleNextPacket ();
    }

    void
    Consumer::WillSendOutInterest (uint32_t sequenceNumber)
    {
      NS_LOG_DEBUG ("Trying to add " << sequenceNumber << " with " << Simulator::Now () << ". already " << m_seqTimeouts.size () << " items");

      m_seqTimeouts.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));
      m_seqFullDelay.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));

      m_seqLastDelay.erase (sequenceNumber);
      m_seqLastDelay.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));

      m_seqRetxCounts[sequenceNumber] ++;

      m_rtt->SentSeq (SequenceNumber32 (sequenceNumber), 1);
    }


  } // namespace nnn
} // namespace ns3
