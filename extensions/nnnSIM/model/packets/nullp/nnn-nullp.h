/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nullp.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nullp.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nullp.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _NNN_NULLP_HEADER_H_
#define _NNN_NULLP_HEADER_H_

#include "../nnn-packet.h"

namespace ns3 {

  class Packet;

  namespace nnn {

    /**
     * @ingroup nnn
     * @brief NNN Null packet (wire formats are defined in wire)
     **/
    class NULLp : public NNNPDU
    {
    public:
      /**
       * \brief Constructor
       *
       * Creates a NULL packet with no payload
       **/
      NULLp ();

      /**
       * \brief Constructor
       *
       * Creates a NULL packet with payload
       **/
      NULLp(Ptr<Packet> payload);

      /**
       * @brief Copy constructor
       */
      NULLp (const NULLp &nullp);

      /**
       * @brief Sets the payload of the NULL packet
       */
      void
      SetPayload (Ptr<Packet> payload);

      /**
       * @brief Gets the payload of the NULL packet
       */
      Ptr<const Packet>
      GetPayload () const;

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

      /**
       * @brief Print NULLp in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      NULLp &
      operator = (const NULLp &other) { return *this; }

    private:
      uint16_t m_PDUdatatype;   ///< @brief Type of payload held in NULLp
      Ptr<Packet> m_payload;    ///< @brief Payload

    };

    inline std::ostream &
    operator << (std::ostream &os, const NULLp &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class NULLpException {};

  } // namespace nnn
} // namespace ns3

#endif // _NNN_NULLP_HEADER_H_
