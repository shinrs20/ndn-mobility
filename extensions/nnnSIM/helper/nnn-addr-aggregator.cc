/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-addr-aggregator.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-addr-aggregator.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-addr-aggregator.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>

#include "nnn-addr-aggregator.h"

NS_LOG_COMPONENT_DEFINE("nnn.AddrAggregator");

namespace ns3
{
  namespace nnn
  {

    NNNAddrAggregator::NNNAddrAggregator ()
    : m_totaladdr (0)
    , m_totaldest (0)
    {
    }

    NNNAddrAggregator::~NNNAddrAggregator ()
    {
    }

    uint16_t
    NNNAddrAggregator::GetNumDestinations(Ptr<NNNAddress> sector)
    {
      return m_sectorNum[*sector];
    }

    uint16_t
    NNNAddrAggregator::GetNumDistinctDestinations () const
    {
      return super::getPolicy().size();
    }

    uint16_t
    NNNAddrAggregator::GetNumTotalDestinations () const
    {
      uint16_t dests = 0;

      Ptr<const NNNAddrEntry> tmp;

      for (tmp = Begin(); tmp != End(); tmp = Next(tmp))
	{
	  dests += tmp->GetNumAddresses();
	}

      return dests;
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrAggregator::GetDestinations (Ptr<NNNAddress> sector)
    {
      super::iterator item = super::find_exact(*sector);

      if (item == super::end ())
	return std::vector<Ptr<NNNAddress> > ();
      else
	{
	  if (item->payload() == 0)
	    return std::vector<Ptr<NNNAddress> > ();
	  else
	    return item->payload()->GetAddresses();
	}
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrAggregator::GetCompleteDestinations (Ptr<NNNAddress> sector)
    {
      super::iterator item = super::find_exact(*sector);

      if (item == super::end ())
	return std::vector<Ptr<NNNAddress> > ();
      else
	{
	  if (item->payload() == 0)
	    return std::vector<Ptr<NNNAddress> > ();
	  else
	    return item->payload()->GetCompleteAddresses();
	}
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrAggregator::GetDistinctDestinations() const
    {
      std::vector<Ptr<NNNAddress> > distinct;
      Ptr<const NNNAddrEntry> tmp;

      for (tmp = Begin (); tmp != End (); tmp = Next(tmp))
	{
	  distinct.push_back(tmp->GetSector());
	}

      return distinct;
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrAggregator::GetTotalDestinations () const
    {
      std::vector<Ptr<NNNAddress> > distinct;
      Ptr<const NNNAddrEntry> tmp;

      for (tmp = Begin (); tmp != End (); tmp = Next(tmp))
	{
	  std::vector<Ptr<NNNAddress> > tmpV = tmp->GetCompleteAddresses ();
	  distinct.insert(distinct.end(), tmpV.begin(), tmpV.end());
	}

      return distinct;
    }

    void
    NNNAddrAggregator::AddDestination(Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO("Inserting " << *addr);

      Ptr<NNNAddress> sector = Create<NNNAddress> (addr->getSectorName());
      Ptr<NNNAddress> lastlabel = Create<NNNAddress> (addr->getLastLabel());

      std::pair< super::iterator, bool> result = super::insert(*sector, 0);

      if (result.first != super::end ())
	{
	  if (result.second)
	    {
	      NS_LOG_INFO("New position sector: " << *sector << " lastlabel: " << *lastlabel);
	      Ptr<NNNAddrEntry> newEntry = Create<NNNAddrEntry> ();

	      newEntry->SetSector(sector);
	      newEntry->AddAddress (lastlabel);
	      newEntry->SetTrie (result.first);
	      result.first->set_payload (newEntry);

	      m_totaldest++;
	      m_totaladdr++;
	      m_sectorNum[*sector] = 1;
	    }
	  else
	    {
	      NS_LOG_INFO("Sector already present: " << *sector << " lastlabel: " << *lastlabel);
	      Ptr<NNNAddress> apparent = result.first->payload()->GetSector();
	      NS_LOG_INFO("Testing against sector: " << *apparent);

	      if (*apparent == *sector)
		{
		  result.first->payload()->AddAddress(lastlabel);
		  m_totaladdr++;
		  m_sectorNum[*sector]++;
		}
	    }
	}
    }

    void
    NNNAddrAggregator::RemoveDestination(Ptr<NNNAddress> addr)
    {
      Ptr<NNNAddress> sector = Create<NNNAddress> (addr->getSectorName());
      Ptr<NNNAddress> lastlabel = Create<NNNAddress> (addr->getLastLabel());

      super::iterator item = super::find_exact(*sector);

      if (item != super::end ())
	{
	  Ptr<NNNAddrEntry> tmp = item->payload();
	  if (tmp != 0)
	    {
	      tmp->RemoveAddress(addr);
	    }

	  if (tmp->GetNumAddresses () < 1)
	    {
	      super::erase(item);
	    }
	}
    }

    void
    NNNAddrAggregator::Print (std::ostream &os) const
    {
      os << "3N name aggregation" << std::endl;

      std::vector<Ptr<NNNAddress> > tmp = GetTotalDestinations ();

      for (int i = 0; i < tmp.size(); i++)
	{
	  os << "<Dest" << i << ">" << *tmp[i] << "</Dest" << i << ">" << std::endl;
	}
    }

    Ptr<const NNNAddrEntry>
    NNNAddrAggregator::Begin () const
    {
      super::parent_trie::const_recursive_iterator item (super::getTrie ());
      super::parent_trie::const_recursive_iterator end (0);
      for (; item != end; item++)
	{
	  if (item->payload () == 0) continue;
	  break;
	}

      if (item == end)
	return End ();
      else
	return item->payload ();
    }

    Ptr<const NNNAddrEntry>
    NNNAddrAggregator::End () const
    {
      return 0;
    }

    Ptr<const NNNAddrEntry>
    NNNAddrAggregator::Next (Ptr<const NNNAddrEntry> from) const
    {
      if (from == 0) return 0;

      super::parent_trie::const_recursive_iterator item (*StaticCast<const NNNAddrEntry> (from)->to_iterator ());
      super::parent_trie::const_recursive_iterator end (0);
      for (item++; item != end; item++)
	{
	  if (item->payload () == 0) continue;
	  break;
	}

      if (item == end)
	return End ();
      else
	return item->payload ();
    }

    std::ostream& operator<< (std::ostream& os, const NNNAddrAggregator &addraggr)
    {
      addraggr.Print (os);
      return os;
    }
  } /* namespace nnn */
} /* namespace ns3 */
