/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Takahiro Miyamoto <mt3.mos@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnpt-entry.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnpt-entry.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnst-entpy.cc.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nnn-nnpt-entry.h"

namespace ns3
{
  namespace nnn
  {
    namespace nnpt
    {
      Entry::Entry()
      :m_oldName        (Create<const NNNAddress> ())
      ,m_newName        (Create<const NNNAddress> ())
      ,m_lease_expire   (Seconds (0))
      {
      }

      Entry::~Entry() {
      }

      Entry::Entry (Ptr<const NNNAddress> oldName, Ptr<const NNNAddress> newName, Time lease_expire)
      :m_oldName        (oldName)
      ,m_newName        (newName)
      ,m_lease_expire   (lease_expire)
      {
      }
    } /* namespace nnpt */
  } /* namespace nnn */
} /* namespace ns3 */
