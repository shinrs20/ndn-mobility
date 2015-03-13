/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	         Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-en.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-en.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-en.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NNN_EN_HEADER_H_
#define _NNN_EN_HEADER_H_

#include <vector>

#include <ns3-dev/ns3/address.h>

#include "../nnn-pdu.h"
#include "../../naming/nnn-address.h"

namespace ns3 {

  class Packet;

  namespace nnn {

    /**
     * @ingroup nnn
     * @brief NNN EN packet (wire formats are defined in wire)
     **/
    class EN : public NNNPDU
    {
    public:
      /**
       * \brief Constructor
       *
       * Creates a EN packet
       **/
      EN ();

      /**
       * \brief Constructor
       *
       *
       * @param name Address signature creator
       **/
      EN (std::vector<Address> signature);

      /**
       * @brief Copy constructor
       */
      EN (const EN &en_p);

      uint16_t
      GetPoaType () const;

      void
      SetPoaType (uint16_t type);

      /**
       * \brief Get number of MN's Signatures
       *
       * @param  const reference to Name object
       *
       **/
      uint32_t
      GetNumPoa () const;

      /**
       * \brief Get Signatures of MN
       *
       **/
      std::vector<Address>
      GetPoas () const;

      Address
      GetOnePoa (uint32_t index) const;

      /**
       * \brief Add Signature(MAC)
       *
       * @param signature MAC vectors
       *
       **/
      void
      AddPoa (Address signature);

      void
      AddPoa (std::vector<Address> signatures);

      /**
       * @brief Print EN in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      EN &
      operator = (const EN &other) { return *this; }

    private:
      uint16_t m_poa_type;      ///< @brief Type of PoA in EN packet
      std::vector<Address> m_poas;  ///<@brief vector of Signatures

    };

    inline std::ostream &
    operator << (std::ostream &os, const EN &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class ENException {};

  } // namespace nnn
} // namespace ns3

#endif // _NNN_EN_HEADER_H_
