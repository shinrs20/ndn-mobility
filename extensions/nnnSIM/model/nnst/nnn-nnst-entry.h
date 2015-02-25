/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnst-entry.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnst-entry.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst-entry.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_NNST_ENTRY_H_
#define NNN_NNST_ENTRY_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/traced-value.h>

#include "nnn-nnst.h"
#include "nnn-nnst-entry-facemetric.h"
#include "../nnn-naming.h"
#include "../nnn-face.h"
#include "../../utils/trie/trie.h"
#include "../../utils/trie/counting-policy.h"
#include "../../utils/trie/trie-with-policy.h"
#include "../../helper/nnn-face-container.h"

using namespace ::boost;
using namespace ::boost::multi_index;

namespace ns3 {
  namespace nnn {
    namespace nnst {

      /// @cond include_hidden
      class i_entry {};
      class i_face {};
      class i_poa {};
      class i_lease {};
      class i_metric {};
      class i_nth {};
      /// @endcond

      /**
       * @ingroup nnn-nnst
       * @brief Typedef for indexed face container of Entry
       *
       * Currently, there are 5 indexes:
       * - by face (used to find record and update metric)
       * - by Address
       * - by Lease time
       * - by metric
       * - by position
       */
      /// @cond include_hidden
      typedef multi_index_container<
	  FaceMetric,
	  indexed_by<
	    ordered_unique<
	      tag<i_entry>,
	      identity<FaceMetric>
          >,

            // For fast access to elements using Face
            ordered_non_unique<
              tag<i_face>,
              const_mem_fun<FaceMetric,Ptr<Face>,&FaceMetric::GetFace>
            >,

            // For fast access by PoA Address
            ordered_non_unique<
              tag<i_poa>,
              const_mem_fun<FaceMetric,Address,&FaceMetric::GetAddress>
            >,

            // For access by lease time
            ordered_non_unique<
              tag<i_lease>,
              const_mem_fun<FaceMetric,Time,&FaceMetric::GetExpireTime>
            >,

            // List of available faces ordered by (status, m_routingCost)
            ordered_non_unique<
              tag<i_metric>,
              composite_key<
                FaceMetric,
                const_mem_fun<FaceMetric,FaceMetric::Status,&FaceMetric::GetStatus>,
                const_mem_fun<FaceMetric,int32_t,&FaceMetric::GetRoutingCost>
              >
            >,

            // To optimize nth candidate selection (sacrifice a little bit
            // space to gain speed)
            random_access<
              tag<i_nth>
            >
          >
      > fmtr_set;
      /// @endcond

      typedef fmtr_set::index<i_face>::type fmtr_set_by_face;
      typedef fmtr_set::index<i_poa>::type fmtr_set_by_poa;
      typedef fmtr_set::index<i_lease>::type fmtr_set_by_lease;
      typedef fmtr_set::index<i_metric>::type fmtr_set_by_metric;
      typedef fmtr_set::index<i_nth>::type fmtr_set_by_nth;

      class Entry : public SimpleRefCount<Entry>
      {
      public:
	class NoFaces {};

	typedef nnnSIM::trie_with_policy<
	    NNNAddress,
	    nnnSIM::smart_pointer_payload_traits<Entry>,
	    nnnSIM::counting_policy_traits
	    > trie;

	Entry();

	Entry(Ptr<NNST> nnst, const Ptr<const NNNAddress> &name);

	~Entry();

	Ptr<NNST>
	GetNNST ()
	{
	  return m_nnst;
	}

	const NNNAddress&
	GetAddress () const
	{
	  return *m_address;
	}

	Ptr<const NNNAddress>
	GetAddressPtr ()
	{
	  return m_address;
	}

	void
	SetTrie (trie::iterator item);

	void
	UpdateStatus (Ptr<Face> face, FaceMetric::Status status);

	void
	UpdateLeaseTime (Time n_lease);

	/**
	 * \brief Add or update routing metric of FIB next hop
	 *
	 * Initial status of the next hop is set to YELLOW
	 */
	void AddOrUpdateRoutingMetric (Ptr<Face> face, int32_t metric);

	void
	Invalidate ();

	void
	UpdateFaceRtt (Ptr<Face> face, const Time &sample);

	const FaceMetric &
	FindBestCandidate (uint32_t skip = 0) const;

        std::pair<Ptr<Face>, Address>
        FindBestCandidateFaceInfo (uint32_t skip = 0) const;

	void
	RemoveFace (const Ptr<Face> &face);

	void
	AddPoA (Ptr<Face> face, Address poa, Time e_lease, uint32_t cost);

	std::vector<Address>
	GetPoAs ();

	std::vector<Address>
	GetPoAs (Ptr<Face> face);

	uint32_t
	GetPoAsN ();

	uint32_t
	GetPoAsN (Ptr<Face> face);

	Ptr<Face>
	GetFace (Address poa);

	bool
	isEmpty();

	void
	RemovePoA (Address poa);

	void
	cleanExpired();

	void
	printByAddress () const;

	void
	printByLease () const;

	void
	printByMetric () const;

	void
	printByFace () const;

	trie::iterator to_iterator () { return item_; }
	trie::const_iterator to_iterator ()  const { return item_; }

      public:
	Ptr<NNST> m_nnst;             ///< \brief NNST to which entry is added
	Ptr<const NNNAddress> m_address;    ///< \brief Address used for the NNST Entry
	fmtr_set m_faces;

      private:
	trie::iterator item_;
      };

      std::ostream& operator<< (std::ostream& os, const Entry &entry);


    } /* namespace nnst */
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NNST_ENTRY_H_ */
