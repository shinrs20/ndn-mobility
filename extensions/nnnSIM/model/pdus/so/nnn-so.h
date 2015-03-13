/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-so.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-so.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-so.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _NNN_SO_HEADER_H_
#define _NNN_SO_HEADER_H_

#include "../nnn-pdu.h"
#include "../nnn-data-pdus.h"
#include "../../nnn-naming.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * @ingroup nnn
     * @brief NNN SO packet (wire formats are defined in wire)
     **/
    class SO : public DATAPDU
    {
    public:
      /**
       * \brief Constructor
       *
       * Creates a SO packet with no payload
       **/
      SO ();

      /**
       * \brief Constructor
       *
       * Creates a SO packet with payload
       *
       * @param name NNN Address Ptr
       * @param payload Packet Ptr
       **/
      SO(Ptr<NNNAddress> name, Ptr<Packet> payload);

      /**
       * \brief Constructor
       *
       * Creates a SO packet with payload
       *
       * @param name NNN Address
       * @param payload Packet Ptr
       **/
      SO(const NNNAddress &name, Ptr<Packet> payload);

      /**
       * @brief Copy constructor
       */
      SO (const SO &so_p);

      /**
       * \brief Get NNN name
       *
       * Gets name of the interest.
       **/
      const NNNAddress&
      GetName () const;

      /**
       * @brief Get smart pointer to the NNN name (to avoid extra memory usage)
       */
      Ptr<const NNNAddress>
      GetNamePtr () const;

      /**
       * \brief Set NNN name
       *
       * @param name smart pointer to Name
       *
       **/
      void
      SetName (Ptr<NNNAddress> name);

      /**
       * \brief Another variant to set NNN name
       *
       * @param name const reference to Name object
       *
       **/
      void
      SetName (const NNNAddress &name);

      /**
       * @brief Print SO in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      SO &
      operator = (const SO &other) { return *this; }

    private:
      Ptr<NNNAddress> m_name;   ///< @brief Source NNN Address used in the packet
    };

    inline std::ostream &
    operator << (std::ostream &os, const SO &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class SOException {};

  } // namespace nnn
} // namespace ns3

#endif // _NNN_SO_HEADER_H_
