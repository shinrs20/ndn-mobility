/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-mdo.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-mdo.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-mdo.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/unused.h>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/core.hpp>
#include <boost/ref.hpp>

namespace ll = boost::lambda;

#include "nnn-mdo.h"

NS_LOG_COMPONENT_DEFINE("nnn.MDO");

namespace ns3
{
  namespace nnn
  {

    MDO::MDO ()
     : NNNPacket (MDO_NNN, Seconds (0))
     , m_payload (Create<Packet> ())
     , m_PDUdatatype (NDN_NNN)
     , m_totaladdr (0)
     , m_totaldest (0)
    {
    }

    uint16_t
    MDO::GetNumDestinations(Ptr<NNNAddress> sector)
    {
      /*super::iterator item = super::find_exact(*sector);

      if (item == super::end ())
	return 0;
      else
	{
	  if (item->payload() == 0)
	    return 0;
	  else
	    return item->payload()->GetNumAddresses();
	}*/
      return m_sectorNum[*sector];
    }

    uint16_t
    MDO::GetNumDistinctDestinations () const
    {
      return super::getPolicy().size();
    }

    uint16_t
    MDO::GetNumTotalDestinations () const
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
    MDO::GetDestinations (Ptr<NNNAddress> sector)
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
    MDO::GetCompleteDestinations (Ptr<NNNAddress> sector)
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
    MDO::GetDistinctDestinations() const
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
    MDO::GetTotalDestinations () const
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
    MDO::AddDestination(Ptr<NNNAddress> addr)
    {
      NS_LOG_FUNCTION(this << *addr);

      NS_LOG_INFO("MDO: Inserting " << *addr);

      Ptr<NNNAddress> sector = Create<NNNAddress> (addr->getSectorName());
      Ptr<NNNAddress> lastlabel = Create<NNNAddress> (addr->getLastLabel());

      std::pair< super::iterator, bool> result = super::insert(*sector, 0);

      if (result.first != super::end ())
	{
	  if (result.second)
	    {
	      NS_LOG_INFO("MDO: New position sector: " << *sector << " lastlabel: " << *lastlabel);
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
	      NS_LOG_INFO("MDO: Sector already present: " << *sector << " lastlabel: " << *lastlabel);
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

    Ptr<const Packet>
    MDO::GetPayload () const
    {
      return m_payload;
    }

    void
    MDO::SetPayload (Ptr<Packet> payload)
    {
      m_payload = payload;
      m_wire = 0;
    }

    uint16_t
    MDO::GetPDUPayloadType() const
    {
      return m_PDUdatatype;
    }

    void
    MDO::SetPDUPayloadType (uint16_t pdu_type)
    {
      m_PDUdatatype = pdu_type;
    }

    void
    MDO::Print (std::ostream &os) const
    {
      os << "<MDO>" << std::endl;
      os << "  <TTL>" << GetLifetime () << "</TTL>" << std::endl;
      os << "  <Version>" << GetVersion () << "</Version>" << std::endl;
      os << "  <PDU Type>" << GetPDUPayloadType() << "</PDU Type>" << std::endl;

      std::vector<Ptr<NNNAddress> > tmp = GetTotalDestinations();
      int totalnum = tmp.size();
      if (totalnum > 0)
	{
	  for (int i = 0; i < totalnum; i++)
	    {
	      os << "  <Dest" << i << ">" << *tmp[i] << "</Dest" << i << ">" << std::endl;
	    }
	}


      if (m_payload != 0)
	{
	  os << "  <Payload>Yes</Payload>" << std::endl;
	} else
	  {
	    os << "  <Payload>No</Payload>" << std::endl;
	  }
      os << "</MDO>" << std::endl;
    }

    Ptr<const NNNAddrEntry>
    MDO::Begin () const
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
    MDO::End () const
    {
      return 0;
    }

    Ptr<const NNNAddrEntry>
    MDO::Next (Ptr<const NNNAddrEntry> from) const
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
  } /* namespace nnn */
} /* namespace ns3 */
