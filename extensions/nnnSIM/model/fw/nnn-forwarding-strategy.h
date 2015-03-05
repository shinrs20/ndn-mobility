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

#include <ns3-dev/ns3/name.h>
#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-content-store.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include "../fib/nnn-fib.h"
#include "../pit/nnn-pit.h"

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
    class DU;
    class EN;
    class AEN;
    class REN;
    class DEN;
    class INF;

    class NamesContainer;
    class NamesContainerEntry;

    class NNST;
    class NNPT;

    namespace nnst {
      class Entry;
      class FaceMetric;
    }

    namespace nnpt { class Entry; }

    namespace name { class Component; }

    class NNNAddress;

    class PDUBuffer;

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

      // Obtain a random number between min and max
      uint64_t
      obtain_Num(uint64_t min, uint64_t max);

      virtual void
      SetNode3NName (Ptr<const NNNAddress> name, Time lease);

      virtual const NNNAddress&
      GetNode3NName ();

      virtual Ptr<const NNNAddress>
      GetNode3NNamePtr ();

      // Produces a random 3N name under the delegated name space
      virtual Ptr<NNNAddress>
      produce3NName ();

      virtual bool
      Has3NName ();

      virtual void
      flushBuffer (Ptr<NNNAddress> oldName, Ptr<NNNAddress> newName);

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
       * \brief Actual processing of incoming Nnn NULLps.
       *
       * Processing NULLp packets
       * @param face incoming face
       * @param null_p NULLp packet
       */
      virtual void
      OnNULLp (Ptr<Face> face, Ptr<NULLp> null_p);

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
       * \brief Actual processing of incoming NNN DU packets
       *
       * Processing DU packets
       * @param face    incoming face
       * @param du_p    DU packet
       */
      virtual void
      OnDU (Ptr<Face> face, Ptr<DU> du_p);

      virtual Ptr<pit::Entry>
      ProcessInterest (Ptr<Face> face, Ptr<ndn::Interest> interest, Ptr<NNNAddress> addr);

      virtual Ptr<pit::Entry>
      ProcessData (Ptr<Face> face, Ptr<ndn::Data> data);

      /**
       * @brief Event fired just before PIT entry is removed by timeout
       * @param pitEntry PIT entry to be removed
       */
      virtual void
      WillEraseTimedOutPendingInterest (Ptr<pit::Entry> pitEntry);

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

      /**
       * @brief Event fired every time a FIB entry is added to FIB
       * @param fibEntry FIB entry that was added
       */
      virtual void
      DidAddFibEntry (Ptr<fib::Entry> fibEntry);

      /**
       * @brief Fired just before FIB entry will be removed from FIB
       * @param fibEntry FIB entry that will be removed
       */
      virtual void
      WillRemoveFibEntry (Ptr<fib::Entry> fibEntry);

    protected:
      /**
       * @brief An event that is fired every time a new PIT entry is created
       *
       * Note that if NDN node is receiving a similar interest (interest for the same name),
       * then either DidReceiveDuplicateInterest, DidSuppressSimilarInterest, or DidForwardSimilarInterest
       * will be called
       *
       * Suppression of similar Interests is controlled using ShouldSuppressIncomingInterest virtual method
       *
       * @param inFace  incoming face
       * @param header  deserialized Interest header
       * @param pitEntry created PIT entry (incoming and outgoing face sets are empty)
       *
       * @see DidReceiveDuplicateInterest, DidSuppressSimilarInterest, DidForwardSimilarInterest, ShouldSuppressIncomingInterest
       */
      virtual void
      DidCreatePitEntry (Ptr<Face> inFace,
                         Ptr<const ndn::Interest> interest,
                         Ptr<pit::Entry> pitEntry);

      /**
       * @brief An event that is fired every time a new PIT entry cannot be created (e.g., PIT container imposes a limit)
       *
       * Note that this call can be called only for non-similar Interest (i.e., there is an attempt to create a new PIT entry).
       * For any non-similar Interests, either FailedToCreatePitEntry or DidCreatePitEntry is called.
       *
       * @param inFace   incoming face
       * @param interest Interest packet
       */
      virtual void
      FailedToCreatePitEntry (Ptr<Face> inFace,
                              Ptr<const ndn::Interest> interest);

      /**
       * @brief An event that is fired every time a duplicated Interest is received
       *
       * This even is the last action that is performed before the Interest processing is halted
       *
       * @param inFace  incoming face
       * @param interest Interest packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       *
       * @see DidReceiveDuplicateInterest, DidSuppressSimilarInterest, DidForwardSimilarInterest, ShouldSuppressIncomingInterest
       */
      virtual void
      DidReceiveDuplicateInterest (Ptr<Face> inFace,
                                   Ptr<const ndn::Interest> interest,
                                   Ptr<pit::Entry> pitEntry);

      /**
       * @brief An event that is fired every time when a similar Interest is received and suppressed (collapsed)
       *
       * This even is the last action that is performed before the Interest processing is halted
       *
       * @param inFace  incoming face
       * @param interest Interest packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       *
       * @see DidReceiveDuplicateInterest, DidForwardSimilarInterest, ShouldSuppressIncomingInterest
       */
      virtual void
      DidSuppressSimilarInterest (Ptr<Face> inFace,
                                  Ptr<const ndn::Interest> interest,
                                  Ptr<pit::Entry> pitEntry);

      /**
       * @brief An event that is fired every time when a similar Interest is received and further forwarded (not suppressed/collapsed)
       *
       * This even is fired just before handling the Interest to PropagateInterest method
       *
       * @param inFace  incoming face
       * @param interest Interest packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       *
       * @see DidReceiveDuplicateInterest, DidSuppressSimilarInterest, ShouldSuppressIncomingInterest
       */
      virtual void
      DidForwardSimilarInterest (Ptr<Face> inFace,
                                 Ptr<const ndn::Interest> interest,
                                 Ptr<pit::Entry> pitEntry);

      /**
       * @brief An even that is fired when Interest cannot be forwarded
       *
       * Note that the event will not fire if  retransmission detection is enabled (by default)
       * and retransmitted Interest cannot by forwarded.  For more details, refer to the implementation.
       *
       * @param inFace  incoming face
       * @param interest Interest header
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       *
       * @see DetectRetransmittedInterest
       */
      virtual void
      DidExhaustForwardingOptions (Ptr<Face> inFace,
                                   Ptr<const ndn::Interest> interest,
                                   Ptr<pit::Entry> pitEntry);

      /**
       * @brief Method that implements logic to distinguish between new and retransmitted interest
       *
       * This method is called only when DetectRetransmissions attribute is set true (by default).
       *
       * Currently, the retransmission detection logic relies on the fact that list of incoming faces
       * already has inFace (i.e., a similar interest is received on the same face more than once).
       *
       * @param inFace  incoming face
       * @param interest Interest header
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       * @return true if Interest should be considered as retransmitted
       */
      virtual bool
      DetectRetransmittedInterest (Ptr<Face> inFace,
                                   Ptr<const ndn::Interest> interest,
                                   Ptr<pit::Entry> pitEntry);

      /**
       * @brief Even fired just before Interest will be satisfied
       *
       * Note that when Interest is satisfied from the cache, incoming face will be 0
       *
       * @param inFace  incoming face
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       */
      virtual void
      WillSatisfyPendingInterest (Ptr<Face> inFace,
                                  Ptr<pit::Entry> pitEntry);

      /**
       * @brief Actual procedure to satisfy Interest
       *
       * Note that when Interest is satisfied from the cache, incoming face will be 0
       *
       * @param inFace  incoming face
       * @param data    Data packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       */
      virtual void
      SatisfyPendingInterest (Ptr<Face> inFace, // 0 allowed (from cache)
                              Ptr<const ndn::Data> data,
                              Ptr<pit::Entry> pitEntry);

      /**
       * @brief Event which is fired just after data was send out on the face
       *
       * @param inFace   incoming face of the Data
       * @param outFace  outgoing face
       * @param data     Data packet
       * @param pitEntry an existing PIT entry, corresponding to the duplicated Interest
       */
      virtual void
      DidSendOutData (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const ndn::Data> data,
                      Ptr<pit::Entry> pitEntry);

      /**
       * @brief Event which is fired every time a requested (solicited) DATA packet (there is an active PIT entry) is received
       *
       * @param inFace  incoming face
       * @param data    Data packet
       * @param didCreateCacheEntry flag indicating whether a cache entry was added for this data packet or not (e.g., packet already exists in cache)
       */
      virtual void
      DidReceiveSolicitedData (Ptr<Face> inFace,
                               Ptr<const ndn::Data> data,
                               bool didCreateCacheEntry);

      /**
       * @brief Event which is fired every time an unsolicited DATA packet (no active PIT entry) is received
       *
       * The current implementation allows ignoring unsolicited DATA (by default), or cache it by setting
       * attribute CacheUnsolicitedData true
       *
       * @param inFace  incoming face
       * @param data    Data packet
       * @param didCreateCacheEntry flag indicating whether a cache entry was added for this data packet or not (e.g., packet already exists in cache)
       */
      virtual void
      DidReceiveUnsolicitedData (Ptr<Face> inFace,
                                 Ptr<const ndn::Data> data,
                                 bool didCreateCacheEntry);

      /**
       * @brief Method implementing logic to suppress (collapse) similar Interests
       *
       * In the base class implementation this method checks list of incoming/outgoing faces of the PIT entry
       * (for new Intersets, both lists are empty before this call)
       *
       * For more details, refer to the source code.
       *
       * @param inFace  incoming face
       * @param interest Interest packet
       * @param payload Data payload
       */
      virtual bool
      ShouldSuppressIncomingInterest (Ptr<Face> inFace,
                                      Ptr<const ndn::Interest> interest,
                                      Ptr<pit::Entry> pitEntry);

      /**
       * @brief Method to check whether Interest can be send out on the particular face or not
       *
       * In the base class, this method perfoms two checks:
       * 1. If inFace is equal to outFace (when equal, Interest forwarding is prohibited)
       * 2. Whether Interest should be suppressed (list of outgoing faces include outFace),
       * considering (if enabled) retransmission logic
       *
       * @param inFace     incoming face of the Interest
       * @param outFace    proposed outgoing face of the Interest
       * @param interest   Interest packet
       * @param pitEntry   reference to PIT entry (reference to corresponding FIB entry inside)
       *
       * @see DetectRetransmittedInterest
       */
      virtual bool
      CanSendOutInterest (Ptr<Face> inFace,
                          Ptr<Face> outFace,
                          Ptr<const ndn::Interest> interest,
                          Ptr<pit::Entry> pitEntry);

      /**
       * @brief Method implementing actual interest forwarding, taking into account CanSendOutInterest decision
       *
       * If event returns false, then there is some kind of a problem exists
       *
       * @param inFace     incoming face of the Interest
       * @param outFace    proposed outgoing face of the Interest
       * @param interest Interest packet
       * @param pitEntry   reference to PIT entry (reference to corresponding FIB entry inside)
       *
       * @see CanSendOutInterest
       */
      virtual bool
      TrySendOutInterest (Ptr<Face> inFace,
                          Ptr<Face> outFace,
                          Ptr<const ndn::Interest> interest,
                          Ptr<pit::Entry> pitEntry);

      /**
       * @brief Event fired just after forwarding the Interest
       *
       * @param inFace     incoming face of the Interest
       * @param outFace    outgoing face of the Interest
       * @param interest Interest packet
       * @param pitEntry   reference to PIT entry (reference to corresponding FIB entry inside)
       */
      virtual void
      DidSendOutInterest (Ptr<Face> inFace,
                          Ptr<Face> outFace,
                          Ptr<const ndn::Interest> interest,
                          Ptr<pit::Entry> pitEntry);

      /**
       * @brief Wrapper method, which performs general tasks and calls DoPropagateInterest method
       *
       * General tasks so far are adding face to the list of incoming face, updating
       * PIT entry lifetime, calling DoPropagateInterest, and retransmissions (enabled by default).
       *
       * @param inFace     incoming face
       * @param interest   Interest packet
       * @param pitEntry   reference to PIT entry (reference to corresponding FIB entry inside)
       *
       * @see DoPropagateInterest
       */
      virtual void
      PropagateInterest (Ptr<Face> inFace,
                         Ptr<const ndn::Interest> interest,
                         Ptr<pit::Entry> pitEntry);

      /**
       * @brief Virtual method to perform Interest propagation according to the forwarding strategy logic
       *
       * In most cases, this is the call that needs to be implemented/re-implemented in order
       * to perform forwarding of Interests according to the desired logic.
       *
       * There is also PropagateInterest method (generally, do not require to be overriden)
       * which performs general tasks (adding face to the list of incoming face, updating
       * PIT entry lifetime, calling DoPropagateInterest, as well as perform retransmissions (enabled by default).
       *
       * @param inFace     incoming face
       * @param interest   Interest packet
       * @param pitEntry   reference to PIT entry (reference to corresponding FIB entry inside)
       *
       * @return true if interest was successfully propagated, false if all options have failed
       *
       * @see PropagateInterest
       */
      virtual bool
      DoPropagateInterest (Ptr<Face> inFace,
                           Ptr<const ndn::Interest> interest,
                           Ptr<pit::Entry> pitEntry);
    protected:
      // inherited from Object class
      virtual void NotifyNewAggregate (); ///< @brief Even when object is aggregated to another Object
      virtual void DoDispose (); ///< @brief Do cleanup

    protected:
      Ptr<NNPT> m_nnpt; ///< \brief Reference to NNPT
      Ptr<NNST> m_nnst; ///< \brief Reference to NNST

      Ptr<NamesContainer> m_node_names; ///< \brief 3N names container for personal names
      Ptr<NamesContainer> m_leased_names; ///< \brief 3N names container for node leased names

      Ptr<PDUBuffer> m_node_pdu_buffer; /// <\brief Buffer for Node using forwarding strategy

      Ptr<Pit> m_pit; ///< \brief Reference to PIT to which this forwarding strategy is associated
      Ptr<Fib> m_fib; ///< \brief Reference to FIB to which this forwarding strategy is associated
      Ptr<ndn::ContentStore> m_contentStore; ///< \brief Content store (for caching purposes only)

      bool m_cacheUnsolicitedDataFromApps;
      bool m_cacheUnsolicitedData;
      bool m_detectRetransmissions;
      bool m_produce3Nnames;

      Time m_3n_lease_time;
      int32_t m_standardMetric;

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const EN>,
      Ptr<const Face> > m_outENs; ///< @brief trace of outgoing EN

      TracedCallback<Ptr<const EN>,
      Ptr<const Face> > m_inENs; ///< @brief trace of incoming EN

      TracedCallback<Ptr<const EN>,
      Ptr<const Face> > m_dropENs;  ///< @brief trace of dropped EN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const AEN>,
      Ptr<const Face> > m_outAENs; ///< @brief trace of outgoing AEN

      TracedCallback<Ptr<const AEN>,
      Ptr<const Face> > m_inAENs; ///< @brief trace of incoming AEN

      TracedCallback<Ptr<const AEN>,
      Ptr<const Face> > m_dropAENs;  ///< @brief trace of dropped AEN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const DEN>,
      Ptr<const Face> > m_outDENs; ///< @brief trace of outgoing DEN

      TracedCallback<Ptr<const DEN>,
      Ptr<const Face> > m_inDENs; ///< @brief trace of incoming DEN

      TracedCallback<Ptr<const DEN>,
      Ptr<const Face> > m_dropDENs;  ///< @brief trace of dropped DEN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const REN>,
      Ptr<const Face> > m_outRENs; ///< @brief trace of outgoing REN

      TracedCallback<Ptr<const REN>,
      Ptr<const Face> > m_inRENs; ///< @brief trace of incoming REN

      TracedCallback<Ptr<const REN>,
      Ptr<const Face> > m_dropRENs;  ///< @brief trace of dropped REN

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_outINFs; ///< @brief trace of outgoing INF

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_inINFs; ///< @brief trace of incoming INF

      TracedCallback<Ptr<const INF>,
      Ptr<const Face> > m_dropINFs;  ///< @brief trace of dropped INF

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const NULLp>,
      Ptr<const Face> > m_outNULLps; ///< @brief trace of outgoing NULLp

      TracedCallback<Ptr<const NULLp>,
      Ptr<const Face> > m_inNULLps; ///< @brief trace of incoming NULLp

      TracedCallback<Ptr<const NULLp>,
      Ptr<const Face> > m_dropNULLps;  ///< @brief trace of dropped NULLp

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const SO>,
      Ptr<const Face> > m_outSOs; ///< @brief Transmitted SOs trace

      TracedCallback<Ptr<const SO>,
      Ptr<const Face> > m_inSOs; ///< @brief trace of incoming SOs

      TracedCallback<Ptr<const SO>,
      Ptr<const Face> > m_dropSOs; ///< @brief trace of dropped SOs

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const DO>,
      Ptr<const Face> > m_outDOs; ///< @brief trace of outgoing DO

      TracedCallback<Ptr<const DO>,
      Ptr<const Face> > m_inDOs; ///< @brief trace of incoming DO

      TracedCallback<Ptr<const DO>,
      Ptr<const Face> > m_dropDOs;  ///< @brief trace of dropped DO

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const DU>,
      Ptr<const Face> > m_outDUs; ///< @brief trace of outgoing DO

      TracedCallback<Ptr<const DU>,
      Ptr<const Face> > m_inDUs; ///< @brief trace of incoming DO

      TracedCallback<Ptr<const DU>,
      Ptr<const Face> > m_dropDUs;  ///< @brief trace of dropped DO

      ////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const ndn::Interest>,
      Ptr<const Face> > m_outInterests; ///< @brief Transmitted interests trace

      TracedCallback<Ptr<const ndn::Interest>,
      Ptr<const Face> > m_inInterests; ///< @brief trace of incoming Interests

      TracedCallback<Ptr<const ndn::Interest>,
      Ptr<const Face> > m_dropInterests; ///< @brief trace of dropped Interests

      ////////////////////////////////////////////////////////////////////

      TracedCallback<Ptr<const ndn::Data>,
      bool /*from cache*/,
      Ptr<const Face> > m_outData; ///< @brief trace of outgoing Data

      TracedCallback<Ptr<const ndn::Data>,
      Ptr<const Face> > m_inData; ///< @brief trace of incoming Data

      TracedCallback<Ptr<const ndn::Data>,
      Ptr<const Face> > m_dropData;  ///< @brief trace of dropped Data

      ////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////

      TracedCallback< Ptr<const pit::Entry> > m_satisfiedInterests;
      TracedCallback< Ptr<const pit::Entry> > m_timedOutInterests;
    private:
      // Number generator
      boost::random::mt19937_64 gen;
    };

  } // namespace nnn
} // namespace ns3

#endif /* NNN_FORWARDING_STRATEGY_H */
