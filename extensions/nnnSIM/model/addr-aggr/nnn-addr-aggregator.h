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
#include "../../utils/trie/trie-with-policy.h"
#include "../../utils/trie/counting-policy.h"

namespace ns3
{
  namespace nnn
  {
    class NNNAddrEntry : public Object
    {
    public:
      typedef nnnSIM::trie_with_policy<
	  NNNAddress,
	  nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
	  nnnSIM::counting_policy_traits
	  > trie;

      struct PtrNNNComp
      {
	bool operator () (const Ptr<NNNAddress> &lhs , const Ptr<NNNAddress>  &rhs) const  {
	  return *lhs < *rhs;
	}
      };

      NNNAddrEntry ()
      : m_sector (Create<NNNAddress> ())
      , m_addresses (std::set<Ptr<NNNAddress>, PtrNNNComp> ())
      , item_ (0)
      , m_totaladdr (0)
      {
      }

      Ptr<NNNAddress>
      GetSector() const
      {
	return m_sector;
      }

      void
      SetSector(Ptr<NNNAddress> sector)
      {
	m_sector = sector;
      }

      void
      SetTrie (trie::iterator item)
      {
	item_ = item;
      }

      uint16_t
      GetNumAddresses() const
      {
	return m_totaladdr;
      }

      std::vector<Ptr<NNNAddress> >
      GetAddresses () const
      {
	std::vector<Ptr<NNNAddress> > addr;

	std::set<Ptr<NNNAddress>, PtrNNNComp>::iterator it;

	for (it = m_addresses.begin(); it != m_addresses.end(); ++it)
	  {
	    addr.push_back(*it);
	  }

	return addr;
      }

      std::vector<Ptr<NNNAddress> >
      GetCompleteAddresses () const
      {

	std::vector<Ptr<NNNAddress> > compAddr;

	std::set<Ptr<NNNAddress>, PtrNNNComp>::iterator it;

	for (it = m_addresses.begin(); it != m_addresses.end(); ++it)
	  {
	    NNNAddress tmp = *m_sector + **it;
	    compAddr.push_back(Create<NNNAddress> (tmp.toDotHex()));
	  }

	return compAddr;
      }

      void
      AddAddress (Ptr<NNNAddress> addr)
      {
	if (addr->isOneLabel ())
	  {
	    m_addresses.insert(addr);
	    m_totaladdr++;
	  }
      }

      void
      RemoveAddress (Ptr<NNNAddress> addr)
      {
	Ptr<NNNAddress> sector = Create<NNNAddress> (addr->getSectorName());
	Ptr<NNNAddress> lastlabel = Create<NNNAddress> (addr->getLastLabel());

	if (m_sector->getName() == sector->getName())
	  {
	    m_addresses.erase(lastlabel);
	    m_totaladdr--;
	  }
      }

      bool
      CompleteAddressExists (Ptr<NNNAddress> addr)
      {

	Ptr<NNNAddress> sector = Create<NNNAddress> (addr->getSectorName());
	Ptr<NNNAddress> lastLabel = Create<NNNAddress> (addr->getLastLabel());

	if (m_sector->getName() == sector->getName())
	  {
	    return LastLabelExists(lastLabel);
	  }
	else
	  return false;
      }

      bool
      LastLabelExists (Ptr<NNNAddress> addr)
      {
	return (m_addresses.find(addr) != m_addresses.end());
      }

      trie::iterator
      to_iterator () { return item_; }

      trie::const_iterator
      to_iterator ()  const { return item_; }

    private:
      Ptr<NNNAddress> m_sector;
      std::set<Ptr<NNNAddress>, PtrNNNComp> m_addresses;
      uint16_t m_totaladdr;
      trie::iterator item_;
    };

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
