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
    : m_addresses (std::vector<Ptr<NNNAddress> > ())
    , item_ (0)
    {
    }

    NNNAddrEntry::~NNNAddrEntry ()
    {
    }

    std::vector<Ptr<NNNAddress> >
    NNNAddrEntry::GetAddresses () const
    {
      return m_addresses;
    }

    void
    NNNAddrEntry::AddAddress (Ptr<NNNAddress> addr)
    {
      if (addr->isOneLabel ())
	m_addresses.push_back(addr);
    }

    void
    NNNAddrEntry::SetTrie(trie::iterator item)
    {
      item_ = item;
    }

    std::ostream& operator<< (std::ostream& os, const NNNAddrEntry &entry)
    {

      std::vector<Ptr<NNNAddress> > all = entry.GetAddresses ();

      for(std::vector<Ptr<NNNAddress> >::iterator it = all.begin(); it != all.end(); ++it)
	{
	  os << *it << std::endl;
	}
      os << std::endl;
      return os;
    }

  } /* namespace nnn */
} /* namespace ns3 */
