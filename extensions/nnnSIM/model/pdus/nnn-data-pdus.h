/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-data-pdus.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-data-pdus.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-data-pdus.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_DATA_PDUS_H_
#define _NNN_DATA_PDUS_H_

#include "nnn-pdu.h"

namespace ns3
{
  namespace nnn
  {
    class DATAPDU : public virtual NNNPDU
    {
    public:
      DATAPDU ();
      virtual
      ~DATAPDU ();

      /**
       * @brief Gets the payload of the DO packet
       */
      Ptr<const Packet>
      GetPayload () const;

      /**
       * @brief Sets the payload of the DO packet
       */
      void
      SetPayload (Ptr<Packet> payload);

      /**
       * @brief Get the PDU type in DO
       */
      uint16_t
      GetPDUPayloadType () const;

      /**
       * @brief Set the PDU type held in DO
       *
       * @param pdu_type PDU type in DO
       */
      void
      SetPDUPayloadType (uint16_t pdu_type);

      virtual void
      Print (std::ostream &os) const;

    protected:
      uint16_t m_PDUdatatype;   ///< @brief Type of payload held in PDU
      Ptr<Packet> m_payload;    ///< @brief Payload
    };
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* _NNN_DATA_PDUS_H_ */
