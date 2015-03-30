/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-names-container-entry.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-names-container-entry.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-names-container-entry.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nnn-names-container-entry.h"

namespace ns3
{
  namespace nnn
  {
    NamesContainerEntry::NamesContainerEntry()
    : m_name            (Create<const NNNAddress> ())
    , m_lease_expire    (Seconds (0))
    , m_renew_time      (Seconds (0))
    , m_fixed           (false)
    {
    }

    NamesContainerEntry::~NamesContainerEntry() {

    }

    NamesContainerEntry::NamesContainerEntry(Ptr<const NNNAddress> name, Time lease_expire, Time renew_time, bool fixed)
    : m_name           (name)
    , m_lease_expire   (lease_expire)
    , m_renew_time     (renew_time)
    , m_fixed          (fixed)
    {
    }

  } /* namespace nnn */
} /* namespace ns3 */
