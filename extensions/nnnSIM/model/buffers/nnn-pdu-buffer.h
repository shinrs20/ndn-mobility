/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-pdu-buffer.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-pdu-buffer.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-pdu-buffer.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PDU_BUFFER_H_
#define PDU_BUFFER_H_

#include <ns3-dev/ns3/traced-value.h>
#include <ns3-dev/ns3/trace-source-accessor.h>
#include <ns3-dev/ns3/object.h>
#include <ns3-dev/ns3/ptr.h>

#include "../nnn-pdus.h"
#include "../nnn-naming.h"
#include "../../utils/trie/trie.h"
#include "../../utils/trie/counting-policy.h"
#include "../../utils/trie/trie-with-policy.h"

#include "nnn-pdu-buffer-queue.h"

namespace ns3
{
  namespace nnn
  {
    class PDUBuffer : public Object,
    protected ns3::nnn::nnnSIM::trie_with_policy<
      NNNAddress,
      ns3::nnn::nnnSIM::smart_pointer_payload_traits<PDUQueue>,
      ns3::nnn::nnnSIM::counting_policy_traits
    >
    {
    public:
      typedef ns3::nnn::nnnSIM::trie_with_policy<
	  NNNAddress,
	  ns3::nnn::nnnSIM::smart_pointer_payload_traits<PDUQueue>,
	  ns3::nnn::nnnSIM::counting_policy_traits
      > super;

      static TypeId GetTypeId (void);

      PDUBuffer ();
      virtual
      ~PDUBuffer ();

      void
      AddDestination (const NNNAddress &addr);

      void
      AddDestination (Ptr<NNNAddress> addr);

      void
      RemoveDestination (const NNNAddress &addr);

      void
      RemoveDestination (Ptr<NNNAddress> addr);

      bool
      DestinationExists (const NNNAddress &addr);

      bool
      DestinationExists (Ptr<NNNAddress> addr);

      void
      PushSO (const NNNAddress &addr, Ptr<const SO> so_p);

      void
      PushSO (Ptr<NNNAddress> addr, Ptr<const SO> so_p);

      void
      PushDO (const NNNAddress &addr, Ptr<const DO> do_p);

      void
      PushDO (Ptr<NNNAddress> addr, Ptr<const DO> do_p);

      void
      PushDU (const NNNAddress &addr, Ptr<const DU> du_p);

      void
      PushDU (Ptr<NNNAddress> addr, Ptr<const DU> du_p);

      std::queue<Ptr<Packet> >
      PopQueue (const NNNAddress &addr);

      std::queue<Ptr<Packet> >
      PopQueue (Ptr<NNNAddress> addr);

      uint
      QueueSize (const NNNAddress &addr);

      uint
      QueueSize (Ptr<NNNAddress> addr);
    };

    std::ostream& operator<< (std::ostream& os, const PDUBuffer &buffer);

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* PDU_BUFFER_H_ */
