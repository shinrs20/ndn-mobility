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

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/boolean.h>
#include <ns3-dev/ns3/string.h>

#include "nnn-forwarding-strategy.h"
#include "../nnn-face.h"
#include "../nnn-naming.h"
#include "../nnn-packets.h"
#include "../nnst/nnn-nnst.h"
#include "../nnst/nnn-nnst-entry.h"
#include "../nnst/nnn-nnst-entry-facemetric.h"
#include "../nnpt/nnn-nnpt.h"
#include "../nnpt/nnn-nnpt-entry.h"
#include "../../helper/nnn-names-container.h"

namespace ll = boost::lambda;

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

	  .AddTraceSource ("OutMDOs",  "OutMDOs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outMDOs))
	  .AddTraceSource ("InMDOs",   "InMDOs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inMDOs))
	  .AddTraceSource ("DropMDOs", "DropMDOs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropMDOs))

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

    Ptr<NNNAddress>
    ForwardingStrategy::Get3NName ()
    {
      return m_node_names->findNewestName();
    }

    void
    ForwardingStrategy::OnEN (Ptr<Face> face, Ptr<EN> en_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnAEN (Ptr<Face> face, Ptr<AEN> aen_p)
    {
      NS_LOG_FUNCTION (this);

    }

    void
    ForwardingStrategy::OnREN (Ptr<Face> face, Ptr<REN> ren_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnDEN (Ptr<Face> face, Ptr<DEN> den_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnINF (Ptr<Face> face, Ptr<INF> do_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnNULLp (Ptr<Face> face, Ptr<NULLp> null_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnSO (Ptr<Face> face, Ptr<SO> so_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnDO (Ptr<Face> face, Ptr<DO> do_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnDU (Ptr<Face> face, Ptr<DU> du_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::OnMDO (Ptr<Face> face, Ptr<MDO> mdo_p)
    {
      NS_LOG_FUNCTION (this);
    }

    void
    ForwardingStrategy::WillEraseTimedOutPendingInterest (Ptr<pit::Entry> pitEntry)
    {
      m_timedOutInterests (pitEntry);
    }

    void
    ForwardingStrategy::AddFace (Ptr<Face> face)
    {
    }

    void
    ForwardingStrategy::RemoveFace (Ptr<Face> face)
    {
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
    }

    void
    ForwardingStrategy::FailedToCreatePitEntry (Ptr<Face> inFace,
                                                Ptr<const ndn::Interest> interest)
    {
      m_dropInterests (interest, inFace);
    }

    void
    ForwardingStrategy::DidReceiveDuplicateInterest (Ptr<Face> inFace,
                                                     Ptr<const ndn::Interest> interest,
                                                     Ptr<pit::Entry> pitEntry)
    {
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
    }

    void
    ForwardingStrategy::DidForwardSimilarInterest (Ptr<Face> inFace,
                                                   Ptr<const ndn::Interest> interest,
                                                   Ptr<pit::Entry> pitEntry)
    {
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
      pit::Entry::out_iterator out = pitEntry->GetOutgoing ().find (inFace);

      // If we have sent interest for this data via this face, then update stats.
      if (out != pitEntry->GetOutgoing ().end ())
	{
	  pitEntry->GetFibEntry ()->UpdateFaceRtt (inFace, Simulator::Now () - out->m_sendTime);
	}

      m_satisfiedInterests (pitEntry);
    }

    void
    ForwardingStrategy::SatisfyPendingInterest (Ptr<Face> inFace,
                                                Ptr<const ndn::Data> data,
                                                Ptr<pit::Entry> pitEntry)
    {
      if (inFace != 0)
	pitEntry->RemoveIncoming (inFace);

      //satisfy all pending incoming Interests
      BOOST_FOREACH (const pit::IncomingFace &incoming, pitEntry->GetIncoming ())
      {
	bool ok = false;
	//incoming.m_face->SendData (data);

	DidSendOutData (inFace, incoming.m_face, data, pitEntry);
	NS_LOG_DEBUG ("Satisfy " << *incoming.m_face);

	if (!ok)
	  {
	    m_dropData (data, incoming.m_face);
	    NS_LOG_DEBUG ("Cannot satisfy data to " << *incoming.m_face);
	  }
      }

      // All incoming interests are satisfied. Remove them
      pitEntry->ClearIncoming ();

      // Remove all outgoing faces
      pitEntry->ClearOutgoing ();

      // Set pruning timout on PIT entry (instead of deleting the record)
      m_pit->MarkErased (pitEntry);
    }

    void
    ForwardingStrategy::DidSendOutData (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const ndn::Data> data,
                                        Ptr<pit::Entry> pitEntry)
    {
      m_outData (data, inFace == 0, outFace);
    }

    void
    ForwardingStrategy::DidReceiveSolicitedData (Ptr<Face> inFace,
                                                 Ptr<const ndn::Data> data,
                                                 bool didCreateCacheEntry)
    {
    }

    void
    ForwardingStrategy::DidReceiveUnsolicitedData (Ptr<Face> inFace,
                                                   Ptr<const ndn::Data> data,
                                                   bool didCreateCacheEntry)
    {
    }

    bool
    ForwardingStrategy::ShouldSuppressIncomingInterest (Ptr<Face> inFace,
                                                        Ptr<const ndn::Interest> interest,
                                                        Ptr<pit::Entry> pitEntry)
    {
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
      m_outInterests (interest, outFace);
    }

    void
    ForwardingStrategy::PropagateInterest (Ptr<Face> inFace,
                       Ptr<const ndn::Interest> interest,
                       Ptr<pit::Entry> pitEntry)
    {
    }

    bool
    ForwardingStrategy::DoPropagateInterest (Ptr<Face> inFace,
                         Ptr<const ndn::Interest> interest,
                         Ptr<pit::Entry> pitEntry)
    {
      return true;
    }

    void
    ForwardingStrategy::NotifyNewAggregate ()
    {
      /*  if (m_pit == 0)
       {
         m_pit = GetObject<Pit> ();
       }*/
      if (m_nnst == 0)
	{
	  m_nnst = GetObject<NNST> ();
	}
      /*  if (m_contentStore == 0)
       {
         m_contentStore = GetObject<ContentStore> ();
       }*/

      Object::NotifyNewAggregate ();
    }

    void
    ForwardingStrategy::DoDispose ()
    {
      //  m_pit = 0;
      //  m_contentStore = 0;
      m_nnst = 0;

      Object::DoDispose ();
    }

  } // namespace nnn
} // namespace ns3
