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
				    /*
    .AddTraceSource ("OutSOs",  "OutSOs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outSOs))
    .AddTraceSource ("InSOs",   "InSOs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inSOs))
    .AddTraceSource ("DropSOs", "DropSOs", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropSOs))

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    .AddTraceSource ("OutDO",  "OutDO",  MakeTraceSourceAccessor (&ForwardingStrategy::m_outDO))
    .AddTraceSource ("InDO",   "InDO",   MakeTraceSourceAccessor (&ForwardingStrategy::m_inDO))
    .AddTraceSource ("DropDO", "DropDO", MakeTraceSourceAccessor (&ForwardingStrategy::m_dropDO))

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    .AddTraceSource ("SatisfiedSOs",  "SatisfiedSOs",  MakeTraceSourceAccessor (&ForwardingStrategy::m_satisfiedSOs))
    .AddTraceSource ("TimedOutSOs",   "TimedOutSOs",   MakeTraceSourceAccessor (&ForwardingStrategy::m_timedOutSOs))

    .AddAttribute ("CacheUnsolicitedSOFromApps", "Cache unsolicited DO that has been pushed from applications",
                   BooleanValue (true),
                   MakeBooleanAccessor (&ForwardingStrategy::m_cacheUnsolicitedDOFromApps),
                   MakeBooleanChecker ())

    .AddAttribute ("CacheUnsolicitedDO", "Cache overheard DO that have not been requested",
                   BooleanValue (false),
                   MakeBooleanAccessor (&ForwardingStrategy::m_cacheUnsolicitedDO),
                   MakeBooleanChecker ())

    .AddAttribute ("DetectRetransmissions", "If non-duplicate SO is received on the same face more than once, "
                                            "it is considered a retransmission",
                   BooleanValue (true),
                   MakeBooleanAccessor (&ForwardingStrategy::m_detectRetransmissions),
                   MakeBooleanChecker ())
    ;
				     */
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

    void
    ForwardingStrategy::DidCreateNNSTEntry (Ptr<Face> inFace, Ptr<const SO> so_p, Ptr<nnst::Entry> nnstEntry)
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
    ForwardingStrategy::OnNULLp (Ptr<Face> face, Ptr<NULLp> null_p)
    {
      NS_LOG_FUNCTION (this);



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

    /*
void
ForwardingStrategy::OnSO (Ptr<Face> inFace,
                                Ptr<SO> so_p)
{
  NS_LOG_FUNCTION (inFace << so_p->GetName ());
  m_inSOs (so_p, inFace);

  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*so_p);
  bool similarSO = true;
  if (pitEntry == 0)
    {
      similarSO = false;
      pitEntry = m_pit->Create (so_p);
      if (pitEntry != 0)
        {
          DidCreatePitEntry (inFace, so_p, pitEntry);
        }
      else
        {
          FailedToCreatePitEntry (inFace, so_p);
          return;
        }
    }

  bool isDuplicated = true;
  if (!pitEntry->IsNonceSeen (so_p->GetNonce ()))
    {
      pitEntry->AddSeenNonce (so_p->GetNonce ());
      isDuplicated = false;
    }

  if (isDuplicated)
    {
      DidReceiveDuplicateSO (inFace, so_p, pitEntry);
      return;
    }

  Ptr<DO> contentObject;
  contentObject = m_contentStore->Lookup (so_p);
  if (contentObject != 0)
    {
      FwHopCountTag hopCountTag;
      if (so_p->GetPayload ()->PeekPacketTag (hopCountTag))
        {
          contentObject->GetPayload ()->AddPacketTag (hopCountTag);
        }

      pitEntry->AddIncoming (inFace ); // Seconds (1.0)

       Do data plane performance measurements
       WillSatisfyPendingSO (0, pitEntry);

       Actually satisfy pending SO
      SatisfyPendingSO (0, contentObject, pitEntry);
       return;
       }

        if (similarSO && ShouldSuppressIncomingSO (inFace, so_p, pitEntry))
          {
            pitEntry->AddIncoming (inFace, so->GetSOLifetime ());
            // update PIT entry lifetime
            pitEntry->UpdateLifetime (so_p->GetSOLifetime ());

            // Suppress this SO if we're still expecting Data from some other face
            NS_LOG_DEBUG ("Suppress SOs");
            m_dropSOs (so_p, inFace);

            DidSuppressSimilarSO (inFace, so_p, pitEntry);
            return;
          }

        if (similarSO)
          {
            DidForwardSimilarSO (inFace, so_p, pitEntry);
          }

        PropagateSO (inFace, so_p, pitEntry);
      }
     */

    /*
void
ForwardingStrategy::OnDO (Ptr<Face> inFace,
                            Ptr<DO> do_p)
{
  NS_LOG_FUNCTION (inFace << do_p->GetName ());
  m_inDO (do_p, inFace);

  // Lookup PIT entry
  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*do_p);
  if (pitEntry == 0)
    {
      bool cached = false;

      if (m_cacheUnsolicitedDO || (m_cacheUnsolicitedDOFromApps && (inFace->GetFlags () & Face::APPLICATION)))
        {
          // Optimistically add or update entry in the content store
          cached = m_contentStore->Add (do_p);
        }
      else
        {
          // Drop DO packet if PIT entry is not found
          // (unsolicited DO packets should not "poison" content store)

          //drop dulicated or not requested DO packet
          m_dropDO (do_p, inFace);
        }

      DidReceiveUnsolicitedDO (inFace, do_p, cached);
      return;
    }
  else
    {
      bool cached = m_contentStore->Add (do_p);
      DidReceiveSolicitedDO (inFace, do_p, cached);
    }

  while (pitEntry != 0)
    {
      // Do Data plane performance measurements
      WillSatisfyPendingSO (inFace, pitEntry);

      // Actually satisfy pending SO
      SatisfyPendingSO (inFace, do_p, pitEntry);

      // Lookup another PIT entry
      pitEntry = m_pit->Lookup (*do_p);
    }
}

void
ForwardingStrategy::DidCreatePitEntry (Ptr<Face> inFace,
                                       Ptr<const SO> so_p,
                                       Ptr<pit::Entry> pitEntrypitEntry)
{
}

void
ForwardingStrategy::FailedToCreatePitEntry (Ptr<Face> inFace,
                                            Ptr<const SO> so_p)
{
  m_dropSOs (so_p, inFace);
}

void
ForwardingStrategy::DidReceiveDuplicateSO (Ptr<Face> inFace,
                                                 Ptr<const SO> so_p,
                                                 Ptr<pit::Entry> pitEntry)
{
  /////////////////////////////////////////////////////////////////////////////////////////
  //                                                                                     //
  // !!!! IMPORTANT CHANGE !!!! Duplicate SOs will create incoming face entry !!!! //
  //                                                                                     //
  /////////////////////////////////////////////////////////////////////////////////////////
  pitEntry->AddIncoming (inFace);
  m_dropSOs (so_p, inFace);
}
     */
    /*
void
ForwardingStrategy::DidSuppressSimilarSO (Ptr<Face> face,
                                                Ptr<const SO> so_p                                            Ptr<pit::Entry> pitEntry)
{
}

void
ForwardingStrategy::DidForwardSimilarSO (Ptr<Face> inFace,
                                               Ptr<const SO> so_p,
                                               Ptr<pit::Entry> pitEntry)
{
}
     */
    /*
void
ForwardingStrategy::DidExhaustForwardingOptions (Ptr<Face> inFace,
                                                 Ptr<const SO> so_p,
                                                 Ptr<pit::Entry> pitEntry)
{
  NS_LOG_FUNCTION (this << boost::cref (*inFace));
  if (pitEntry->AreAllOutgoingInVain ())
    {
      m_dropSOs (so_p, inFace);

      // All incoming SOs cannot be satisfied. Remove them
      pitEntry->ClearIncoming ();

      // Remove also outgoing
      pitEntry->ClearOutgoing ();

      // Set pruning timout on PIT entry (instead of deleting the record)
      m_pit->MarkErased (pitEntry);
    }
}



bool
ForwardingStrategy::DetectRetransmittedSO (Ptr<Face> inFace,
                                                 Ptr<const SO> so_p                                             Ptr<pit::Entry> pitEntry)
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
     */
    /*
void
ForwardingStrategy::SatisfyPendingSO (Ptr<Face> inFace,
                                            Ptr<const DO> do_p,
                                            Ptr<pit::Entry> pitEntry)
{
  if (inFace != 0)
    pitEntry->RemoveIncoming (inFace);

  //satisfy all pending incoming SOs
  BOOST_FOREACH (const pit::IncomingFace &incoming, pitEntry->GetIncoming ())
    {
      bool ok = incoming.m_face->SendDO (do_p);

      DidSendOutDO (inFace, incoming.m_face, do_p, pitEntry);
      NS_LOG_DEBUG ("Satisfy " << *incoming.m_face);

      if (!ok)
        {
          m_dropDO (do_p, incoming.m_face);
          NS_LOG_DEBUG ("Cannot satisfy DO to " << *incoming.m_face);
        }
    }

  // All incoming SOs are satisfied. Remove them
  pitEntry->ClearIncoming ();

  // Remove all outgoing faces
  pitEntry->ClearOutgoing ();

  // Set pruning timout on PIT entry (instead of deleting the record)
  m_pit->MarkErased (pitEntry);
}
     */
    /*
void
ForwardingStrategy::DidReceiveSolicitedDO (Ptr<Face> inFace,
                                             Ptr<const DO> do_p,
                                             bool didCreateCacheEntry)
{
  // do nothing
}
     */
    /*
void
ForwardingStrategy::DidReceiveUnsolicitedDO (Ptr<Face> inFace,
                                               Ptr<const DO> do_p,
                                               bool didCreateCacheEntry)
{
  // do nothing
}
     */
    /*
void
ForwardingStrategy::WillSatisfyPendingSO (Ptr<Face> inFace,
                                                Ptr<pit::Entry> pitEntry)
{
  pit::Entry::out_iterator out = pitEntry->GetOutgoing ().find (inFace);

  // If we have sent SO for this DO via this face, then update stats.
  if (out != pitEntry->GetOutgoing ().end ())
    {
      pitEntry->GetNNSTEntry ()->UpdateFaceRtt (inFace, Simulator::Now () - out->m_sendTime);
    }

  m_satisfiedSOs (pitEntry);
}
     */
    /*
bool
ForwardingStrategy::ShouldSuppressIncomingSO (Ptr<Face> inFace,
                                                    Ptr<const SO> so_p,
                                                    Ptr<pit::Entry> pitEntry)
{
  bool isNew = pitEntry->GetIncoming ().size () == 0 && pitEntry->GetOutgoing ().size () == 0;

  if (isNew) return false; // never suppress new SOs

  bool isRetransmitted = m_detectRetransmissions && // a small guard
                         DetectRetransmittedSO (inFace, so_p, pitEntry);

  if (pitEntry->GetOutgoing ().find (inFace) != pitEntry->GetOutgoing ().end ())
    {
      NS_LOG_DEBUG ("Non duplicate SOs from the face we have sent SO to. Don't suppress");
      // got a non-duplicate SO from the face we have sent SO to
      // Probably, there is no point in waiting DO from that face... Not sure yet

      // If we're expecting DO from the interface we got the SO from ("producer" asks us for "his own" DO)
      // Mark interface YELLOW, but keep a small hope that DO will come eventually.

      // ?? not sure if we need to do that ?? ...

      // pitEntry->GetNNSTEntry ()->UpdateStatus (inFace, NNST::FaceMetric::_NNST_YELLOW);
    }
  else
    if (!isNew && !isRetransmitted)
      {
        return true;
      }

  return false;
}
     */
    /*
void
ForwardingStrategy::PropagateSO (Ptr<Face> inFace,
                                       Ptr<const SO> so_p,
                                       Ptr<pit::Entry> pitEntry)
{
  bool isRetransmitted = m_detectRetransmissions && // a small guard
                         DetectRetransmittedSO (inFace, so_p, pitEntry);

  pitEntry->AddIncoming (inFace); //, so_p->GetSOLifetime ());
  /// @todo Make lifetime per incoming interface       */
    /*  pitEntry->UpdateLifetime (so_p->GetSOLifetime ());

  bool propagated = DoPropagateSO (inFace, so_p, pitEntry);

  if (!propagated && isRetransmitted) //give another chance if retransmitted
    {
      // increase max number of allowed retransmissions
      pitEntry->IncreaseAllowedRetxCount ();

      // try again
      propagated = DoPropagateSO (inFace, so_p, pitEntry);
    }

  // if (!propagated)
  //   {
  //     NS_LOG_DEBUG ("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  //     NS_LOG_DEBUG ("+++ Not propagated ["<< so->GetName () <<"], but number of outgoing faces: " << pitEntry->GetOutgoing ().size ());
  //     NS_LOG_DEBUG ("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  //   }

  // ForwardingStrategy will try its best to forward packet to at least one interface.
  // If no so was propagated, then there is not other option for forwarding or
  // ForwardingStrategy failed to find it.
  if (!propagated && pitEntry->AreAllOutgoingInVain ())
    {
      DidExhaustForwardingOptions (inFace, so_p, pitEntry);
    }
}
     */
    /*
bool
ForwardingStrategy::CanSendOutSO (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const SO> so_p,
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
     */
    /*
bool
ForwardingStrategy::TrySendOutSO (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const SO> so_p,
                                        Ptr<pit::Entry> pitEntry)
{
  if (!CanSendOutSO (inFace, outFace, so_p, pitEntry))
    {
      return false;
    }

  pitEntry->AddOutgoing (outFace);

  //transmission
  bool successSend = outFace->SendSO (so_p);
  if (!successSend)
    {
      m_dropSOs (so_p, outFace);
    }

  DidSendOutSO (inFace, outFace, so_p, pitEntry);

  return true;
}
     */
    /*
void
ForwardingStrategy::DidSendOutSO (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const SO> so_p,
                                        Ptr<pit::Entry> pitEntry)
{
  m_outSOs (so_p, outFace);
}
     */
    /*
void
ForwardingStrategy::DidSendOutDO (Ptr<Face> inFace,
                                    Ptr<Face> outFace,
                                    Ptr<const DO> do_p,
                                    Ptr<pit::Entry> pitEntry)
{
  m_outDO (do_p, inFace == 0, outFace);
}
     */
    /*
void
ForwardingStrategy::WillEraseTimedOutPendingSO (Ptr<pit::Entry> pitEntry)
{
  m_timedOutSOs (pitEntry);
}
     */
    void
    ForwardingStrategy::AddFace (Ptr<Face> face)
    {
      // do nothing here
    }

    void
    ForwardingStrategy::RemoveFace (Ptr<Face> face)
    {
      // do nothing here
    }

    //void
    //ForwardingStrategy::DidAddNNSTEntry (Ptr<nnst::Entry> NNSTEntry)
    //{
    //  // do nothing here
    //}
    //
    //void
    //ForwardingStrategy::WillRemoveNNSTEntry (Ptr<nnst::Entry> NNSTEntry)
    //{
    //  // do nothing here
    //}


  } // namespace nnn
} // namespace ns3
