/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-addr-entry.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-addr-entry.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-addr-entry.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nnn-addr-entry.h"

namespace ns3
{
  namespace nnn
  {

    NNNAddrEntry::NNNAddrEntry ()
    : m_sector (Create<NNNAddress> ())
    , m_addresses (std::vector<Ptr<NNNAddress> > ())
    , item_ (0)
    , m_totaladdr (0)
    {
    }

    NNNAddrEntry::~NNNAddrEntry ()
    {
    }

    Ptr<NNNAddress>
    NNNAddrEntry::GetSector() const
    {
      return m_sector;
    }

    void
    NNNAddrEntry::SetSector(Ptr<NNNAddress> sector)
    {
      m_sector = sector;
    }

    void
    NNNAddrEntry::SetTrie(trie::iterator item)
    {
      item_ = item;
    }

    uint16_t
    NNNAddrEntry::GetNumAddresses() const
    {
      return m_totaladdr;
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrEntry::GetAddresses () const
    {
      return m_addresses;
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrEntry::GetCompleteAddresses () const
    {
      std::vector<Ptr<NNNAddress> > compAddr;

      for (int i = 0; i < m_addresses.size(); i++)
	{
	  NNNAddress tmp = *m_sector + *m_addresses[i];
	  compAddr.push_back(Create<NNNAddress> (tmp.toDotHex()));
	}

      return compAddr;
    }

    void
    NNNAddrEntry::AddAddress (Ptr<NNNAddress> addr)
    {
      if (addr->isOneLabel () && !m_sector->isEmpty())
	{
	  m_addresses.push_back(addr);
	  m_totaladdr++;
	}
    }

    std::ostream& operator<< (std::ostream& os, const NNNAddrEntry &entry)
    {

      std::vector<Ptr<NNNAddress> > all = entry.GetAddresses ();
      Ptr<NNNAddress> sector = entry.GetSector();

      for(std::vector<Ptr<NNNAddress> >::iterator it = all.begin(); it != all.end(); ++it)
	{
	  os << *sector << "." << *it << std::endl;
	}
      os << std::endl;
      return os;
    }
  } /* namespace nnn */
} /* namespace ns3 */
