/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-do.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-do.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-do.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _NNN_DO_HEADER_H_
#define _NNN_DO_HEADER_H_

#include <ns3-dev/ns3/simple-ref-count.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>

#include "../nnn-packet.h"
#include "../../naming/nnn-address.h"

namespace ns3 {

  class Packet;

  namespace nnn {

    /**
     * @ingroup nnn
     * @brief NNN Null packet (wire formats are defined in wire)
     **/
    class DO : public NNNPacket, public SimpleRefCount<DO>
    {
    public:
      /**
       * \brief Constructor
       *
       * Creates a NULL packet with no payload
       **/
      DO ();

      /**
       * \brief Constructor
       *
       * Creates a DO packet with payload
       *
       * @param name NNN Address Ptr
       * @param payload Packet Ptr
       **/
      DO(Ptr<NNNAddress> name, Ptr<Packet> payload);

      /**
       * \brief Constructor
       *
       * Creates a DO packet with payload
       *
       * @param name NNN Address
       * @param payload Packet Ptr
       **/
      DO(const NNNAddress &name, Ptr<Packet> payload);

      /**
       * @brief Copy constructor
       */
      DO (const DO &do_p);

      /**
       * \brief Get interest name
       *
       * Gets name of the interest.
       **/
      const NNNAddress&
      GetName () const;

      /**
       * @brief Get smart pointer to the interest name (to avoid extra memory usage)
       */
      Ptr<const NNNAddress>
      GetNamePtr () const;

      /**
       * \brief Set interest name
       *
       * @param name smart pointer to Name
       *
       **/
      void
      SetName (Ptr<NNNAddress> name);

      /**
       * \brief Another variant to set interest name
       *
       * @param name const reference to Name object
       *
       **/
      void
      SetName (const NNNAddress &name);

      /**
       * @brief Gets the payload of the NULL packet
       */
      Ptr<const Packet>
      GetPayload () const;

      /**
       * @brief Sets the payload of the NULL packet
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

      /**
       * @brief Print DO in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      DO &
      operator = (const DO &other) { return *this; }

    private:
      Ptr<NNNAddress> m_name;   ///< @brief Destination NNN Address used in the packet
      uint16_t m_PDUdatatype;   ///< @brief Type of payload held in DO
      Ptr<Packet> m_payload;    ///< @brief Payload

    };

    inline std::ostream &
    operator << (std::ostream &os, const DO &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class DOException {};

  } // namespace nnn
} // namespace ns3

#endif // _NNN_DO_HEADER_H_
