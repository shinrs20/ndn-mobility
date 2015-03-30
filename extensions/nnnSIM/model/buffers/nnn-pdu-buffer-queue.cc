/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-pdu-buffer-queue.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pdu-buffer-queue.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pdu-buffer-queue.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>

#include "nnn-pdu-buffer-queue.h"

namespace ns3
{
  namespace nnn
  {

    PDUQueue::PDUQueue ()
    {
    }

    PDUQueue::~PDUQueue ()
    {
    }

    bool
    PDUQueue::isEmpty()
    {
      return buffer.empty();
    }

    void
    PDUQueue::clear ()
    {
      std::queue<Ptr<Packet> > empty;

      std::swap(buffer, empty);
    }

    Ptr<Packet>
    PDUQueue::pop ()
    {
      Ptr<Packet> tmp = buffer.front ();
      buffer.pop();
      return tmp;
    }

    void
    PDUQueue::push (Ptr<Packet> pdu)
    {
      buffer.push (pdu);
    }

    void
    PDUQueue::pushSO (Ptr<const SO> so_p)
    {
      buffer.push(Wire::FromSO(so_p, Wire::WIRE_FORMAT_NNNSIM));
    }

    void
    PDUQueue::pushDO (Ptr<const DO> do_p)
    {
      buffer.push(Wire::FromDO(do_p, Wire::WIRE_FORMAT_NNNSIM));
    }

    void
    PDUQueue::pushDU (Ptr<const DU> du_p)
    {
      buffer.push(Wire::FromDU(du_p, Wire::WIRE_FORMAT_NNNSIM));
    }

    std::queue<Ptr<Packet> >
    PDUQueue::popQueue ()
    {
      return buffer;
    }

    uint
    PDUQueue::size ()
    {
      return buffer.size();
    }

  } /* namespace nnn */
} /* namespace ns3 */
