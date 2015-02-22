/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Takahiro Miyamoto <mt3.mos@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnpt-entry.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnpt-entry.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst-entpy.h.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NNN_NNPT_ENTRY_H_
#define NNN_NNPT_ENTRY_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simple-ref-count.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/traced-value.h>

#include "../nnn-naming.h"
#include "../nnn-face.h"
#include "../../utils/trie/trie.h"
#include "../../utils/trie/counting-policy.h"
#include "../../utils/trie/trie-with-policy.h"
#include "../../helper/nnn-face-container.h"
#include "../nnn-naming.h"

namespace ns3 {
  namespace nnn {

    /**
     * @ingroup nnn-nnpt
     * @brief structure for PIT entry
     *
     * All set-methods are virtual, in case index rearrangement is necessary in the derived classes
     */
    class NNPTEntry :  public SimpleRefCount<NNPTEntry>
    {
    public:
      /**
       * \brief PIT entry constructor
       * \param prefix Prefix of the PIT entry
       * \param offsetTime Relative time to the current moment, representing PIT entry lifetime
       * \param fibEntry A FIB entry associated with the PIT entry
       */
      NNPTEntry ();

      NNPTEntry (Ptr<const NNNAddress> oldName, Ptr<const NNNAddress> newName, Time lease_expire);

      NNPTEntry (Ptr<const NNNAddress> oldName, Ptr<const NNNAddress> newName, Time lease_expire, Time renew);

      virtual
      ~NNPTEntry ();

      bool
      operator< (const NNPTEntry e) const { return m_lease_expire < e.m_lease_expire; }

      Ptr<const NNNAddress> m_oldName;
      Ptr<const NNNAddress> m_newName;
      Time m_lease_expire;
      Time m_renew;
    };

    inline std::ostream &
    operator<< (std::ostream &os, const NNPTEntry &nnpt)
    {
      os << *nnpt.m_oldName << "\t" << *nnpt.m_newName  << "\t" << nnpt.m_lease_expire << "\t" << nnpt.m_renew << std::endl;
      return os;
    }

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NNPT_ENTRY_H_ */
