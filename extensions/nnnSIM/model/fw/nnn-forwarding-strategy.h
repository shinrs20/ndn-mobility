/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-forwarding-strategy.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-forwarding-strategy.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-forwarding-strategy.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_FORWARDING_STRATEGY_H
#define NNN_FORWARDING_STRATEGY_H

#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/object.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/traced-callback.h>

namespace ns3 {
  namespace nnn {

    /**
     * @ingroup nnn
     * @defgroup nnn-fw NNN forwarding strategies
     */


    /**
     * @ingroup nnn-fw
     * @brief Namespace for Forwarding Strategy operations
     */
    namespace fw {
    }

    class Face;

    class NULLp;
    class SO;
    class DO;
    class EN;
    class AEN;
    class REN;
    class DEN;
    class INF;
    class DU;

    class NamesContainer;
    class NamesContainerEntry;

    class NNST;
    class NNPT;

    namespace nnst {
      class Entry;
      class FaceMetric;
    }

    namespace nnpt { class Entry; }

    //class NNSTFaceMetric;

    /**
     * @ingroup nnn-fw
     * @brief Abstract base class for Nnn forwarding strategies
     */
    class ForwardingStrategy : public Object
    {
    public:
      static TypeId GetTypeId ();

      /**
       * @brief Helper function to retrieve logging name for the forwarding strategy
       */
      static std::string GetLogName ();

      /**
       * @brief Default constructor
       */
      ForwardingStrategy ();
      virtual ~ForwardingStrategy ();

      /**
       * \brief Actual processing of incoming Nnn SOs.
       *
       * Processing SO packets
       * @param face incoming face
       * @param so_p SO packet
       */
      virtual void
      OnSO (Ptr<Face> face, Ptr<SO> so_p);

      /**
       * \brief Actual processing of incoming Nnn content objects
       *
       * Processing DO packets
       * @param face incoming face
       * @param do_p DO packet
       */
      virtual void
      OnDO (Ptr<Face> face, Ptr<DO> do_p);

      /**
       * \brief Actual processing of incoming Nnn NULLps.
       *
       * Processing NULLp packets
       * @param face incoming face
       * @param null_p NULLp packet
       */
      virtual void
      OnNULLp (Ptr<Face> face, Ptr<NULLp> null_p);

      /**
       * \brief Actual processing of incoming Nnn content objects
       *
       * Processing EN packets
       * @param face    incoming face
       * @param en_p    EN packet
       */
      virtual void
      OnEN (Ptr<Face> face, Ptr<EN> en_p);

      /**
       * \brief Actual processing of incoming Nnn AENs.
       *
       * Processing AEN packets
       * @param face     incoming face
       * @param aen_p AEN packet
       */
      virtual void
      OnAEN (Ptr<Face> face, Ptr<AEN> aen_p);

      /**
       * \brief Actual processing of incoming Nnn RENs
       *
       * Processing REN packets
       * @param face    incoming face
       * @param ren_p    REN packet
       */
      virtual void
      OnREN (Ptr<Face> face, Ptr<REN> ren_p);

      /**
       * \brief Actual processing of incoming Nnn DENs
       *
       * Processing DEN packets
       * @param face    incoming face
       * @param den_p    DEN packet
       */
      virtual void
      OnDEN (Ptr<Face> face, Ptr<DEN> den_p);

      /**
       * \brief Actual processing of incoming Nnn INFs
       *
       * Processing INF packets
       * @param face    incoming face
       * @param inf_p    INF packet
       */
      virtual void
      OnINF (Ptr<Face> face, Ptr<INF> inf_p);

      /**
       * \brief Actual processing of incoming NNN DU packets
       *
       * Processing DU packets
       * @param face    incoming face
       * @param du_p    DU packet
       */
      virtual void
      OnDU (Ptr<Face> face, Ptr<DU> du_p);

      /**
       * @brief Event fired just before PIT entry is removed by timeout
       * @param pitEntry PIT entry to be removed

  virtual void
  WillEraseTimedOutPendingSO (Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Event fired every time face is added to NNN stack
       * @param face face to be removed
       */
      virtual void
      AddFace (Ptr<Face> face);

      /**
       * @brief Event fired every time face is removed from NNN stack
       * @param face face to be removed
       *
       * For example, when an application terminates, AppFace is removed and this method called by NNN stack.
       */
      virtual void
      RemoveFace (Ptr<Face> face);

      /**
       * @brief Event fired every time a NNST entry is added to NNST
       * @param NNSTEntry NNST entry that was added
       */
      virtual void
      DidAddNNSTEntry (Ptr<nnst::Entry> NNSTEntry);

      /**
       * @brief Fired just before NNST entry will be removed from NNST
       * @param NNSTEntry NNST entry that will be removed
       */
      virtual void
      WillRemoveNNSTEntry (Ptr<nnst::Entry> NNSTEntry);

      /**
       * @brief Event fired every time a NNPT entry is added to NNPT
       * @param NNPTEntry NNPT entry that was added
       */
      virtual void
      DidAddNNPTEntry (Ptr<nnpt::Entry> NNPTEntry);

      /**
       * @brief Fired just before NNPT entry will be removed from NNST
       * @param NNPTEntry NNPT entry that will be removed
       */
      virtual void
      WillRemoveNNPTEntry (Ptr<nnpt::Entry> NNPTEntry);

    protected:
      /**
       * @brief An event that is fired every time a new NNST entry is created
       *
       * @param inFace  incoming face
       * @param header  deserialized SO header
       * @param nnstEntry created PIT entry (incoming and outgoing face sets are empty)
       *
       */
      virtual void
      DidCreateNNSTEntry (Ptr<Face> inFace, Ptr<const SO> so_p, Ptr<nnst::Entry> nnstEntry);

      /**
       * @brief An event that is fired every time a new PIT entry cannot be created (e.g., PIT container imposes a limit)
       *
       * Note that this call can be called only for non-similar SO (i.e., there is an attempt to create a new PIT entry).
       * For any non-similar SOs, either FailedToCreatePitEntry or DidCreatePitEntry is called.
       *
       * @param inFace   incoming face
       * @param so_p SO packet

  virtual void
  FailedToCreatePitEntry (Ptr<Face> inFace,
                          Ptr<const SO> so_p);
       */

      /**
       * @brief An event that is fired every time a duplicated SO is received
       *
       * This even is the last action that is performed before the SO processing is halted
       *
       * @param inFace  incoming face
       * @param so_p SO packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
       *
       * @see DidReceiveDuplicateSO, DidSuppressSimilarSO, DidForwardSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidReceiveDuplicateSO (Ptr<Face> inFace,
                               Ptr<const SO> so_p,
                               Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief An event that is fired every time when a similar SO is received and suppressed (collapsed)
       *
       * This even is the last action that is performed before the SO processing is halted
       *
       * @param inFace  incoming face
       * @param so_p SO packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
       *
       * @see DidReceiveDuplicateSO, DidForwardSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidSuppressSimilarSO (Ptr<Face> inFace,
                              Ptr<const SO> so_p,
                              Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief An event that is fired every time when a similar SO is received and further forwarded (not suppressed/collapsed)
       *
       * This even is fired just before handling the SO to PropagateSO method
       *
       * @param inFace  incoming face
       * @param so_p SO packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
       *
       * @see DidReceiveDuplicateSO, DidSuppressSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidForwardSimilarSO (Ptr<Face> inFace,
                             Ptr<const SO> so_p,
                             Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief An even that is fired when SO cannot be forwarded
       *
       * Note that the event will not fire if  retransmission detection is enabled (by default)
       * and retransmitted SO cannot by forwarded.  For more details, refer to the implementation.
       *
       * @param inFace  incoming face
       * @param so_p SO header
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
       *
       * @see DetectRetransmittedSO

  virtual void
  DidExhaustForwardingOptions (Ptr<Face> inFace,
                               Ptr<const SO> so_p,
                               Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Method that implements logic to distinguish between new and retransmitted SO
       *
       * This method is called only when DetectRetransmissions attribute is set true (by default).
       *
       * Currently, the retransmission detection logic relies on the fact that list of incoming faces
       * already has inFace (i.e., a similar SO is received on the same face more than once).
       *
       * @param inFace  incoming face
       * @param so_p SO header
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
       * @return true if SO should be considered as retransmitted

  virtual bool
  DetectRetransmittedSO (Ptr<Face> inFace,
                               Ptr<const SO> so_p,
                               Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Even fired just before SO will be satisfied
       *
       * Note that when SO is satisfied from the cache, incoming face will be 0
       *
       * @param inFace  incoming face
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO

  virtual void
  WillSatisfyPendingSO (Ptr<Face> inFace,
                              Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Actual procedure to satisfy SO
       *
       * Note that when SO is satisfied from the cache, incoming face will be 0
       *
       * @param inFace  incoming face
       * @param do_p    DO packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO

  virtual void
  SatisfyPendingSO (Ptr<Face> inFace, // 0 allowed (from cache)
                          Ptr<const DO> do_p,
                          Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Event which is fired just after DO was send out on the face
       *
       * @param inFace   incoming face of the DO
       * @param outFace  outgoing face
       * @param do_p     DO packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated SO

  virtual void
  DidSendOutDO (Ptr<Face> inFace,
                  Ptr<Face> outFace,
                  Ptr<const DO> do_p,
                  Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Event which is fired every time a requested (solicited) DO packet (there is an active PIT entry) is received
       *
       * @param inFace  incoming face
       * @param do_p    DO packet
       * @param didCreateCacheEntry flag indicating whether a cache entry was added for this DO packet or not (e.g., packet already exists in cache)

  virtual void
  DidReceiveSolicitedDO (Ptr<Face> inFace,
                           Ptr<const DO> do_p,
                           bool didCreateCacheEntry);
       */

      /**
       * @brief Event which is fired every time an unsolicited DO packet (no active PIT entry) is received
       *
       * The current implementation allows ignoring unsolicited DO (by default), or cache it by setting
       * attribute CacheUnsolicitedDO true
       *
       * @param inFace  incoming face
       * @param do_p    DO packet
       * @param didCreateCacheEntry flag indicating whether a cache entry was added for this DO packet or not (e.g., packet already exists in cache)

  virtual void
  DidReceiveUnsolicitedDO (Ptr<Face> inFace,
                             Ptr<const DO> do_p,
                             bool didCreateCacheEntry);
       */

      /**
       * @brief Method implementing logic to suppress (collapse) similar SOs
       *
       * In the base class implementation this method checks list of incoming/outgoing faces of the PIT entry
       * (for new Intersets, both lists are empty before this call)
       *
       * For more details, refer to the source code.
       *
       * @param inFace  incoming face
       * @param so_p SO packet
       * @param payload DO payload

  virtual bool
  ShouldSuppressIncomingSO (Ptr<Face> inFace,
                                  Ptr<const SO> so_p,
                                  Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Method to check whether SO can be send out on the particular face or not
       *
       * In the base class, this method perfoms two checks:
       * 1. If inFace is equal to outFace (when equal, SO forwarding is prohibited)
       * 2. Whether SO should be suppressed (list of outgoing faces include outFace),
       * considering (if enabled) retransmission logic
       *
       * @param inFace     incoming face of the SO
       * @param outFace    proposed outgoing face of the SO
       * @param so_p   SO packet
       * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
       *
       * @see DetectRetransmittedSO

  virtual bool
  CanSendOutSO (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const SO> so_p,
                      Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Method implementing actual SO forwarding, taking into account CanSendOutSO decision
       *
       * If event returns false, then there is some kind of a problem exists
       *
       * @param inFace     incoming face of the SO
       * @param outFace    proposed outgoing face of the SO
       * @param so_p SO packet
       * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
       *
       * @see CanSendOutSO

  virtual bool
  TrySendOutSO (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const SO> so_p,
                      Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Event fired just after forwarding the SO
       *
       * @param inFace     incoming face of the SO
       * @param outFace    outgoing face of the SO
       * @param so_p SO packet
       * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)

  virtual void
  DidSendOutSO (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const SO> so_p,
                      Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Wrapper method, which performs general tasks and calls DoPropagateSO method
       *
       * General tasks so far are adding face to the list of incoming face, updating
       * PIT entry lifetime, calling DoPropagateSO, and retransmissions (enabled by default).
       *
       * @param inFace     incoming face
       * @param so_p   SO packet
       * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
       *
       * @see DoPropagateSO

  virtual void
  PropagateSO (Ptr<Face> inFace,
                     Ptr<const SO> so_p,
                     Ptr<pit::Entry> pitEntry);
       */

      /**
       * @brief Virtual method to perform SO propagation according to the forwarding strategy logic
       *
       * In most cases, this is the call that needs to be implemented/re-implemented in order
       * to perform forwarding of SOs according to the desired logic.
       *
       * There is also PropagateSO method (generally, do not require to be overriden)
       * which performs general tasks (adding face to the list of incoming face, updating
       * PIT entry lifetime, calling DoPropagateSO, as well as perform retransmissions (enabled by default).
       *
       * @param inFace     incoming face
       * @param so_p   SO packet
       * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
       *
       * @return true if SO was successfully propagated, false if all options have failed
       *
       * @see PropagateSO

  virtual bool
  DoPropagateSO (Ptr<Face> inFace,
                       Ptr<const SO> so_p,
                       Ptr<pit::Entry> pitEntry) = 0;
       */

    protected:
      // inherited from Object class
      virtual void NotifyNewAggregate (); ///< @brief Even when object is aggregated to another Object
      virtual void DoDispose (); ///< @brief Do cleanup

    protected:
      // Ptr<Pit> m_pit; ///< \brief Reference to PIT to which this forwarding strategy is associated
      //Ptr<NNPT> m_nnpt; ///< \brief Reference to NNPT
      Ptr<NNST> m_nnst; ///< \brief Reference to NNST

      Ptr<NamesContainer> m_node_names; ///< \brief 3N names container for personal names
      Ptr<NamesContainer> m_leased_names; ///< \brief 3N names container for node leased names

      // Ptr<ContentStore> m_contentStore; ///< \brief Content store (for caching purposes only)

      //  bool m_cacheUnsolicitedDOFromApps;
      //  bool m_cacheUnsolicitedDO;
      //  bool m_detectRetransmissions;

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const AEN>,
      Ptr<const Face> > m_outAEN; ///< @brief trace of outgoing AEN

      TracedCallback<Ptr<const AEN>,
      Ptr<const Face> > m_inAEN; ///< @brief trace of incoming AEN

      TracedCallback<Ptr<const AEN>,
      Ptr<const Face> > m_dropAEN;  ///< @brief trace of dropped AEN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const DEN>,
      Ptr<const Face> > m_outDEN; ///< @brief trace of outgoing DEN

      TracedCallback<Ptr<const DEN>,
      Ptr<const Face> > m_inDEN; ///< @brief trace of incoming DEN

      TracedCallback<Ptr<const DEN>,
      Ptr<const Face> > m_dropDEN;  ///< @brief trace of dropped DEN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const DO>,
      Ptr<const Face> > m_outDO; ///< @brief trace of outgoing DO

      TracedCallback<Ptr<const DO>,
      Ptr<const Face> > m_inDO; ///< @brief trace of incoming DO

      TracedCallback<Ptr<const DO>,
      Ptr<const Face> > m_dropDO;  ///< @brief trace of dropped DO

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const EN>,
      Ptr<const Face> > m_outEN; ///< @brief trace of outgoing EN

      TracedCallback<Ptr<const EN>,
      Ptr<const Face> > m_inEN; ///< @brief trace of incoming EN

      TracedCallback<Ptr<const EN>,
      Ptr<const Face> > m_dropEN;  ///< @brief trace of dropped EN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_outINF; ///< @brief trace of outgoing INF

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_inINF; ///< @brief trace of incoming INF

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_dropINF;  ///< @brief trace of dropped INF

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const NULLp>,
      Ptr<const Face> > m_outNULLp; ///< @brief trace of outgoing NULLp

      TracedCallback<Ptr<const NULLp>,
      Ptr<const Face> > m_inNULLp; ///< @brief trace of incoming NULLp

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_dropNULLp;  ///< @brief trace of dropped NULLp

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const REN>,
      Ptr<const Face> > m_outREN; ///< @brief trace of outgoing REN

      TracedCallback<Ptr<const REN>,
      Ptr<const Face> > m_inREN; ///< @brief trace of incoming REN

      TracedCallback<Ptr<const REN>,
      Ptr<const Face> > m_dropREN;  ///< @brief trace of dropped REN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const SO>,
      Ptr<const Face> > m_outSOs; ///< @brief Transmitted SOs trace

      TracedCallback<Ptr<const SO>,
      Ptr<const Face> > m_inSOs; ///< @brief trace of incoming SOs

      TracedCallback<Ptr<const SO>,
      Ptr<const Face> > m_dropSOs; ///< @brief trace of dropped SOs

      ////////////////////////////////////////////////////////////////////

      //  TracedCallback< Ptr<const pit::Entry> > m_satisfiedSOs;
      //  TracedCallback< Ptr<const pit::Entry> > m_timedOutSOs;
    };

  } // namespace nnn
} // namespace ns3

#endif /* NNN_FORWARDING_STRATEGY_H */
