/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-names-container.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-names-container.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-names-container.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_NAMES_CONTAINER_H_
#define NNN_NAMES_CONTAINER_H_

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/object.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/simple-ref-count.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include "nnn-names-container-entry.h"
#include "../model/nnn-naming.h"

using boost::multi_index_container;
using namespace ::boost::multi_index;

namespace ns3 {
  namespace nnn {

    struct address {};
    struct lease {};

    typedef multi_index_container<
	NamesContainerEntry,
	indexed_by<
	    // sort by NamesContainer::operator<
	    ordered_unique<
	        tag<lease>,
		identity<NamesContainerEntry>
            >,

            // sort by less<string> on NNNAddress
	    ordered_unique<
	        tag<address>,
		member<NamesContainerEntry,NNNAddress,&NamesContainerEntry::m_name>
            >
        >
    > names_set;

    typedef names_set::index<address>::type names_set_by_name;
    typedef names_set::index<lease>::type names_set_by_lease;

    class NamesContainer : public SimpleRefCount<NamesContainer>
    {

    public:
      NamesContainer();

      virtual
      ~NamesContainer();

      void
      addEntry (NamesContainerEntry nameEntry);

      void
      addEntry (NNNAddress name, Time lease_expire);

      void
      addEntry (NNNAddress name, Time lease_expire, Time renew);

      void
      deleteEntry (NamesContainerEntry nameEntry);

      void
      deleteEntry (NNNAddress name);

      bool
      foundName (NNNAddress name);

      NamesContainerEntry
      findEntry (NNNAddress name);

      NNNAddress
      findNewestName ();

      void
      updateLeaseTime (NNNAddress name, Time lease_expire);

      void
      updateLeaseTime (NNNAddress name, Time lease_expire, Time renew);

      uint32_t
      size ();

      bool
      isEmpty ();

      Time
      findNameExpireTime (NNNAddress name);

      void
      cleanExpired ();

      void
      printByAddress ();

      void
      printByLease ();

      names_set container;
    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_NAMES_CONTAINER_H_ */
