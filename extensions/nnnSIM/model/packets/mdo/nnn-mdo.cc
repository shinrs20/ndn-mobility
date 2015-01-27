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

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/core.hpp>
#include <boost/ref.hpp>

namespace ll = boost::lambda;

#include "nnn-mdo.h"

namespace ns3
{
  namespace nnn
  {

    MDO::MDO ()
     : NNNPacket (MDO_NNN, Seconds (0))
     , m_payload (Create<Packet> ())
     , m_PDUdatatype (NDN_NNN)
    {
    }

    uint16_t
    MDO::GetNumDistinctDestinations ()
    {
      return super::getPolicy().size();
    }

    std::vector<Ptr<NNNAddress> >
    MDO::GetDestinations (Ptr<NNNAddress> sector)
    {
      super::iterator item = super::find_exact(*sector);

      if (item == super::end ())
	return std::vector<Ptr<NNNAddress> > ();
      else
	{
	  std::vector<Ptr<NNNAddress> > dests;

	  std::vector<Ptr<NNNAddress> > addrs = item->payload ()->GetAddresses();

	  for (std::vector<Ptr<NNNAddress> >::iterator it = addrs.begin(); it != addrs.end(); ++it)
	    {
	      NNNAddress test = *sector + *(*it);
	      dests.push_back(Create<NNNAddress> (test.toDotHex()));
	    }

	  return dests;
	}
    }

    void
    MDO::AddDestination(Ptr<NNNAddress> addr)
    {
      Ptr<NNNAddress> sector = Create<NNNAddress> (addr->getSectorName());
      Ptr<NNNAddress> lastlabel = Create<NNNAddress> (addr->getLastLabel());

      std::pair< super::iterator, bool> result = super::insert(*sector, 0);

      if (result.first != super::end ())
	{
	  if (result.second)
	    {
	      Ptr<NNNAddrEntry> newEntry = Create<NNNAddrEntry> ();

	      newEntry->AddAddress (lastlabel);
	      newEntry->SetTrie (result.first);
	      result.first->set_payload (newEntry);
	    }
	  else
	    {
	      result.first->payload()->AddAddress(lastlabel);
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
    MDO::Next (Ptr<NNNAddrEntry> from) const
    {
      if (from == 0) return 0;

      super::parent_trie::recursive_iterator item (*StaticCast<NNNAddrEntry> (from)->to_iterator ());
      super::parent_trie::recursive_iterator end (0);
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
