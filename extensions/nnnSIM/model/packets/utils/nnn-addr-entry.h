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

      struct PtrNNNComp
      {
        bool operator () (const Ptr<NNNAddress> &lhs , const Ptr<NNNAddress>  &rhs) const  {
          return *lhs < *rhs;
        }
      };

      typedef ns3::nnn::nnnSIM::trie_with_policy<
	  NNNAddress,
	  ns3::nnn::nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
	  ns3::nnn::nnnSIM::counting_policy_traits
      > trie;

      NNNAddrEntry ();

      virtual
      ~NNNAddrEntry ();

      Ptr<NNNAddress>
      GetSector () const;

      void
      SetSector (Ptr<NNNAddress> sector);

      void
      SetTrie (trie::iterator item);

      uint16_t
      GetNumAddresses () const;

      std::vector<Ptr<NNNAddress> >
      GetAddresses () const;

      std::vector<Ptr<NNNAddress> >
      GetCompleteAddresses () const;

      void
      AddAddress (Ptr<NNNAddress> addr);

      void
      RemoveAddress (Ptr<NNNAddress> addr);

      bool
      CompleteAddressExists (Ptr<NNNAddress> addr);

      bool
      LastLabelExists (Ptr<NNNAddress> addr);

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

    std::ostream& operator<< (std::ostream& os, const NNNAddrEntry &entry);

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_ADDR_ENTRY_H_ */
