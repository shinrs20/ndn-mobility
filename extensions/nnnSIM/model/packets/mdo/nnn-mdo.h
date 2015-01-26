/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-mdo.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-mdo.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-mdo.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _NNN_MO_H_
#define _NNN_MO_H_

#include <ns3-dev/ns3/simple-ref-count.h>

#include "../nnn-packet.h"
#include "../../naming/nnn-address.h"
#include "../../../utils/trie/trie.h"
#include "../../../utils/trie/counting-policy.h"
#include "../../../utils/trie/trie-with-policy.h"

namespace ns3
{
  class Packet;

  namespace nnn
  {

    class MDO : public NNNPacket, public SimpleRefCount<MDO>
    {
    public:

      MDO ();

      void
      AddDestination (Ptr<NNNAddress> addr);

      void
      AddDestinations (std::vector<Ptr<NNNAddress > > addrs);

      /**
       * @brief Gets the payload of the MDO packet
       */
      Ptr<const Packet>
      GetPayload () const;

      /**
       * @brief Sets the payload of the MDO packet
       */
      void
      SetPayload (Ptr<Packet> payload);

      /**
       * @brief Get the PDU type in MDO
       */
      uint16_t
      GetPDUPayloadType () const;

      /**
       * @brief Set the PDU type held in MDO
       *
       * @param pdu_type PDU type in MDO
       */
      void
      SetPDUPayloadType (uint16_t pdu_type);

      /**
       * @brief Print MDO in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      std::vector<Ptr<NNNAddress> > sectors;
      std::vector<std::vector<Ptr<NNNAddress> > > dests;
      uint16_t m_PDUdatatype;
      Ptr<Packet> m_payload;
    };

    inline std::ostream &
    operator << (std::ostream &os, const MDO &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class MDOException {};

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* _NNN_MO_H_ */
