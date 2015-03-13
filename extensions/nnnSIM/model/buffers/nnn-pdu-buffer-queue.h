/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-pdu-buffer-queue.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pdu-buffer-queue.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pdu-buffer-queue.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PDU_QUEUE_H_
#define PDU_QUEUE_H_

#include <queue>

#include "../nnn-pdus.h"

namespace ns3
{
  namespace nnn
  {

    class PDUQueue : public SimpleRefCount<PDUQueue>
    {
    public:
      PDUQueue ();
      virtual
      ~PDUQueue ();

      bool
      isEmpty();

      void
      clear ();

      Ptr<Packet>
      pop ();

      void
      push (Ptr<Packet> pdu);

      std::queue<Ptr<Packet> >
      popQueue ();

      uint
      size ();

    private:
      std::queue<Ptr<Packet> > buffer;
    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* PDU_QUEUE_H_ */
