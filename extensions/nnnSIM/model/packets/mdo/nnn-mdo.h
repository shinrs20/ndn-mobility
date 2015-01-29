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
#include "../utils/nnn-addr-entry.h"
#include "../../naming/nnn-address.h"
#include "../../../utils/trie/trie.h"
#include "../../../utils/trie/counting-policy.h"
#include "../../../utils/trie/trie-with-policy.h"

namespace ns3
{
  class Packet;

  namespace nnn
  {

    class MDO : public NNNPacket, public SimpleRefCount<MDO>,
    protected ns3::nnn::nnnSIM::trie_with_policy<
        NNNAddress,
        ns3::nnn::nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
        ns3::nnn::nnnSIM::counting_policy_traits
    >
    {
    public:

      typedef ns3::nnn::nnnSIM::trie_with_policy<
	  NNNAddress,
	  ns3::nnn::nnnSIM::smart_pointer_payload_traits<NNNAddrEntry>,
	  ns3::nnn::nnnSIM::counting_policy_traits
      > super;

      MDO ();

      uint16_t
      GetNumDestinations (Ptr<NNNAddress> sector);

      uint16_t
      GetNumDistinctDestinations () const;

      uint16_t
      GetNumTotalDestinations () const;

      std::vector<Ptr<NNNAddress> >
      GetDestinations (Ptr<NNNAddress> sector);

      std::vector<Ptr<NNNAddress> >
      GetCompleteDestinations (Ptr<NNNAddress> sector);

      std::vector<Ptr<NNNAddress> >
      GetDistinctDestinations () const;

      std::vector<Ptr<NNNAddress> >
      GetTotalDestinations () const;

      void
      AddDestination (Ptr<NNNAddress> addr);

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

      Ptr<const NNNAddrEntry>
      Begin () const;

      Ptr<const NNNAddrEntry>
      End () const;

      Ptr<const NNNAddrEntry>
      Next (Ptr<const NNNAddrEntry> from) const;

    private:
      uint16_t m_PDUdatatype;
      Ptr<Packet> m_payload;
      uint16_t m_totaladdr;
      uint16_t m_totaldest;
      std::map<NNNAddress,uint16_t> m_sectorNum;
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
