/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	     Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-aen.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-aen.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-aen.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NNN_AEN_HEADER_H_
#define _NNN_AEN_HEADER_H_

#include "../nnn-pdu.h"
#include "../nnn-en-pdus.h"
#include "../../nnn-naming.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * @ingroup nnn
     * @brief NNN AEN packet (wire formats are defined in wire)
     **/
    class AEN : public ENPDU
    {
    public:
      /**
       * \brief Constructor
       *
       * Creates a AEN packet
       **/
      AEN ();

      /**
       * \brief Constructor
       *
       *
       * @param name NNN Address Ptr
       **/
      AEN(Ptr<NNNAddress> name);

      /**
       * \brief Constructor
       *
       * Creates a AEN packet with payload
       *
       * @param name NNN Address
       * @param payload Packet Ptr
       **/
      AEN(const NNNAddress &name);

      /**
       * @brief Copy constructor
       */
      AEN (const AEN &aen_p);

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

      Time
      GetLeasetime() const;

      void
      SetLeasetime (Time lease);

      /**
       * @brief Print AEN in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      AEN &
      operator = (const AEN &other) { return *this; }

    private:
      Time m_lease;             ///< @brief Lease time for NNN Address
      Ptr<NNNAddress> m_name;   ///< @brief Destination NNN Address handed

    };

    inline std::ostream &
    operator << (std::ostream &os, const AEN &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class AENException {};

  } // namespace nnn
} // namespace ns3

#endif // _NNN_AEN_HEADER_H_
