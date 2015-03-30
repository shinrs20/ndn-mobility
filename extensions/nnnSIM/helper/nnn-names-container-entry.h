/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-names-container-entry.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-names-container-entry.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-names-container-entry.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_NAMES_CONTAINER_ENTRY_H_
#define NNN_NAMES_CONTAINER_ENTRY_H_

#include <iostream>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/object.h>
#include <ns3-dev/ns3/simple-ref-count.h>

#include "nnn-names-container-entry.h"
#include "../model/nnn-naming.h"

namespace ns3
{
  namespace nnn
  {
    class NamesContainerEntry : public SimpleRefCount<NamesContainerEntry>
    {
    public:

      NamesContainerEntry();

      NamesContainerEntry(Ptr<const NNNAddress> name, Time lease_expire, Time renew_time, bool fixed);

      virtual ~NamesContainerEntry();

      bool operator< (const NamesContainerEntry e) const { return m_lease_expire > e.m_lease_expire; }

      Ptr<const NNNAddress> m_name;
      Time m_lease_expire;
      Time m_renew_time;
      bool m_fixed;
    };

    inline std::ostream &
    operator << (std::ostream &os, const NamesContainerEntry &entry)
    {
      os << *(entry.m_name) << "\t" << entry.m_lease_expire << "\t" << entry.m_renew_time <<  "\t" << entry.m_fixed << std::endl;
      return os;
    }

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NAMES_CONTAINER_ENTRY_H_ */
