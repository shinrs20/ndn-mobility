/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-addr-aggregator.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-addr-aggregator.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-addr-aggregator.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef NNN_ADDR_AGGREGATOR_H_
#define NNN_ADDR_AGGREGATOR_H_

#include "../naming/nnn-address.h"

#include "../../utils/trie/trie.h"
#include "../../utils/trie/counting-policy.h"
#include "../../utils/trie/trie-with-policy.h"

namespace ns3
{
  namespace nnn
  {
    class NNNAddrEntry;

    class NNNAddrAggregator : public SimpleRefCount<NNNAddrAggregator>,
    protected nnnSIM::trie_with_policy<
    NNNAddress,
    nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
    nnnSIM::counting_policy_traits
    >
    {
    public:
      typedef nnnSIM::trie_with_policy<
	  NNNAddress,
	  nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
	  nnnSIM::counting_policy_traits
      > super;

      NNNAddrAggregator ();
      virtual
      ~NNNAddrAggregator ();

      uint16_t
      GetNumDestinations (Ptr<NNNAddress> sector);

      uint16_t
      GetNumDistinctDestinations () const;

      uint16_t
      GetNumTotalDestinations () const;

      std::vector<Ptr<NNNAddress> >
      GetDestinations (Ptr<NNNAddress> sector);

      std::vector<Ptr<NNNAddress> >
      GetCompleteDestinations (Ptr<NNNAddress> sector);

      std::vector<Ptr<NNNAddress> >
      GetDistinctDestinations () const;

      std::vector<Ptr<NNNAddress> >
      GetTotalDestinations () const;

      void
      AddDestination (Ptr<NNNAddress> addr);

      void
      RemoveDestination (Ptr<NNNAddress> addr);

      bool
      DestinationExists (Ptr<NNNAddress> addr);

      bool
      isEmpty ();

      void
      Print (std::ostream &os) const;

      Ptr<const NNNAddrEntry>
      Begin () const;

      Ptr<const NNNAddrEntry>
      End () const;

      Ptr<const NNNAddrEntry>
      Next (Ptr<const NNNAddrEntry> from) const;

    private:
      uint16_t m_totaladdr;
      uint16_t m_totaldest;
      std::map<NNNAddress,uint16_t> m_sectorNum;
    };

    std::ostream& operator<< (std::ostream& os, const NNNAddrAggregator &addraggr);

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_ADDR_AGGREGATOR_H_ */
