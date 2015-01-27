/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-addr-entry.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-addr-entry.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-addr-entry.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NNN_ADDR_ENTRY_H_
#define NNN_ADDR_ENTRY_H_

#include "../../naming/nnn-address.h"
#include "../../../utils/trie/trie.h"
#include "../../../utils/trie/counting-policy.h"
#include "../../../utils/trie/trie-with-policy.h"

namespace ns3
{
  namespace nnn
  {

    class NNNAddrEntry : public SimpleRefCount<NNNAddrEntry>
    {
    public:

      typedef nnnSIM::trie_with_policy<
	  NNNAddress,
	  nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
	  nnnSIM::counting_policy_traits
	  > trie;

      NNNAddrEntry ();
      virtual
      ~NNNAddrEntry ();

      void
      SetTrie (trie::iterator item);

      std::vector<Ptr<NNNAddress> >
      GetAddresses () const;

      void
      AddAddress (Ptr<NNNAddress> addr);

      trie::iterator
      to_iterator () { return item_; }

      trie::const_iterator
      to_iterator ()  const { return item_; }

    private:
      std::vector<Ptr<NNNAddress> > m_addresses;
      trie::iterator item_;
    };

    std::ostream& operator<< (std::ostream& os, const NNNAddrEntry &entry);

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_ADDR_ENTRY_H_ */
