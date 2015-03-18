/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnst.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnst.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst.h.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NNN_NNST_H_
#define NNN_NNST_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

using namespace ::boost;
using namespace ::boost::multi_index;

#include <ns3-dev/ns3/event-id.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/object.h>

#include "nnn-nnst-entry-facemetric.h"
#include "../nnn-naming.h"
#include "../nnn-face.h"
#include "../fw/nnn-forwarding-strategy.h"
#include "../../utils/trie/trie.h"
#include "../../utils/trie/counting-policy.h"
#include "../../utils/trie/trie-with-policy.h"

namespace ns3
{
  namespace nnn
  {
    class L3Protocol;

    class NULLp;
    typedef NULLp NULLpHeader;
    class SO;
    typedef SO SOHeader;
    class DO;
    typedef DO DOHeader;
    class EN;
    typedef EN ENHeader;
    class AEN;
    typedef AEN AENHeader;
    class REN;
    typedef REN RENHeader;
    class INF;
    typedef INF INFHeader;

    namespace nnst {

      class Entry;

    }

    /**
     * @ingroup nnn
     * @defgroup nnn NNST
     */
    class NNST : public Object,
    protected nnnSIM::trie_with_policy<
    NNNAddress,
    nnnSIM::smart_pointer_payload_traits<nnst::Entry>,
    nnnSIM::counting_policy_traits>
    {
    public:

      typedef nnnSIM::trie_with_policy<
	  NNNAddress,
	  nnnSIM::smart_pointer_payload_traits<nnst::Entry>,
	  nnnSIM::counting_policy_traits
	  > super;

      static TypeId GetTypeId ();

      NNST();

      ~NNST();

      Ptr<nnst::Entry>
      ClosestSector (const NNNAddress &prefix);
      
      Ptr<const NNNAddress>
      ClosestSectorNameInfo (const NNNAddress &prefix);

      Ptr<const NNNAddress>
      ClosestSectorNameInfo (Ptr<const NNNAddress> prefix);

      std::pair<Ptr<Face>, Address>
      ClosestSectorFaceInfo (const NNNAddress &prefix, uint32_t skip);

      std::pair<Ptr<Face>, Address>
      ClosestSectorFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip);

      std::vector<Ptr<const NNNAddress> >
      OneHopNameInfo (const NNNAddress &prefix);

      std::vector<Ptr<const NNNAddress> >
      OneHopNameInfo (Ptr<const NNNAddress> prefix);

      std::vector<std::pair<Ptr<Face>, Address> >
      OneHopFaceInfo (const NNNAddress &prefix, uint32_t skip);

      std::vector<std::pair<Ptr<Face>, Address> >
      OneHopFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip);

      std::vector<Ptr<const NNNAddress> >
      OneHopSubSectorNameInfo (const NNNAddress &prefix);

      std::vector<Ptr<const NNNAddress> >
      OneHopSubSectorNameInfo (Ptr<const NNNAddress> prefix);

      std::vector<std::pair<Ptr<Face>, Address> >
      OneHopSubSectorFaceInfo (const NNNAddress &prefix, uint32_t skip);

      std::vector<std::pair<Ptr<Face>, Address> >
      OneHopSubSectorFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip);

      std::vector<Ptr<const NNNAddress> >
      OneHopParentSectorNameInfo (const NNNAddress &prefix);

      std::vector<Ptr<const NNNAddress> >
      OneHopParentSectorNameInfo (Ptr<const NNNAddress> prefix);

      std::vector<std::pair<Ptr<Face>, Address> >
      OneHopParentSectorFaceInfo (const NNNAddress &prefix, uint32_t skip);

      std::vector<std::pair<Ptr<Face>, Address> >
      OneHopParentSectorFaceInfo (Ptr<const NNNAddress> prefix, uint32_t skip);

      Ptr<nnst::Entry>
      Find (const NNNAddress &prefix);

      // This one should be eliminated
      Ptr<nnst::Entry>
      Add (const NNNAddress &prefix, Ptr<Face> face, int32_t metric);

      // This one as well
      Ptr<nnst::Entry>
      Add (const Ptr<const NNNAddress> &prefix, Ptr<Face> face, int32_t metric);

      Ptr<nnst::Entry>
      Add (const NNNAddress &prefix, Ptr<Face> face, Address poa, Time lease_expire, int32_t metric);

      Ptr<nnst::Entry>
      Add (const Ptr<const NNNAddress> &prefix, std::vector<Ptr<Face> > faces, Address poa, Time lease_expire, int32_t metric);

      Ptr<nnst::Entry>
      Add (const Ptr<const NNNAddress> &prefix, Ptr<Face> face, std::vector<Address> poas, Time lease_expire, int32_t metric);

      Ptr<nnst::Entry>
      Add (const Ptr<const NNNAddress> &prefix, Ptr<Face> face, Address poa, Time lease_expire, int32_t metric);

//      void
//      Invalidate ();

      void
      UpdateStatus (const NNNAddress &prefix, Ptr<Face> face, nnst::FaceMetric::Status status);

      void
      UpdateLeaseTime (const NNNAddress &prefix, Time n_lease);

      /**
       * \brief Add or update routing metric of FIB next hop
       *
       * Initial status of the next hop is set to YELLOW
       */
      void
      AddOrUpdateRoutingMetric (const NNNAddress &prefix, Ptr<Face> face, int32_t metric);

      void
      UpdateFaceRtt (const NNNAddress &prefix, Ptr<Face> face, const Time &sample);

      void
      InvalidateAll ();

      void
      Remove (const Ptr<const NNNAddress> &prefix);

      void
      RemoveFromAll (Ptr<Face> face);

      void
      RemoveFromAll (Address poa);

      void
      Print (std::ostream &os) const;

      void
      PrintByMetric () const;

      void
      PrintByAddress () const;

      void
      PrintByLease () const;

      void
      PrintByFace () const;

      virtual Ptr<const nnst::Entry>
      Begin () const;

      Ptr<nnst::Entry>
      Begin ();

      Ptr<const nnst::Entry>
      End () const;

      Ptr<nnst::Entry>
      End ();

      Ptr<const nnst::Entry>
      Next (Ptr<const nnst::Entry> item) const;

      Ptr<nnst::Entry>
      Next (Ptr<nnst::Entry> item);

      uint32_t
      GetSize ();

      Ptr<NNST>
      GetNNST (Ptr<Object> node);

      bool
      FoundName (const NNNAddress &prefix);

      std::vector<Address>
      GetAllPoas (const NNNAddress &prefix);

    protected:
      // inherited from Object class
      virtual void NotifyNewAggregate (); ///< @brief Notify when object is aggregated
      virtual void DoDispose (); ///< @brief Perform cleanup

    private:
      Ptr<nnst::Entry>
      Add (const Ptr<const NNNAddress> &prefix, Ptr<Face> face, Address poa, Time lease_expire, int32_t metric, char c);

      /**
       * @brief Remove reference to a face from the entry. If entry had only this face, the whole
       * entry will be removed
       */
      void
      RemoveFace (super::parent_trie &item, Ptr<Face> face);

      void
      RemovePoA (super::parent_trie &item, Address poa);

      void
      cleanExpired(Ptr<nnst::Entry> item);
    };

    std::ostream& operator<< (std::ostream& os, const NNST &nnst);

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NNST_H_ */
