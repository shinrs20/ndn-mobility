/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	             Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-den.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-den.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-den.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NNN_DEN_HEADER_H_
#define _NNN_DEN_HEADER_H_

#include <vector>

#include "../nnn-packet.h"
#include "../../naming/nnn-address.h"

namespace ns3 {

  class Packet;

  namespace nnn {

    /**
     * @ingroup nnn
     * @brief NNN DEN packet (wire formats are defined in wire)
     **/
    class DEN : public NNNPDU
    {
    public:
      /**
       * \brief Constructor
       *
       * Creates a REN packet
       **/
      DEN ();

      /**
       * \brief Constructor
       *
       *
       * @param name NNN Address Ptr
       **/
      DEN(Ptr<NNNAddress> name);

      /**
       * \brief Constructor
       *
       * Creates a DEN packet with payload
       *
       * @param name NNN Address
       * @param payload Packet Ptr
       **/
      DEN(const NNNAddress &name);

      /**
       * @brief Copy constructor
       */
      DEN (const DEN &den_p);

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
       * @brief Print DEN in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      DEN &
      operator = (const DEN &other) { return *this; }

    private:
      Ptr<NNNAddress> m_name;   ///< @brief NNN Address used in the packet
      uint16_t m_poa_type;      ///< @brief Type of PoA in DEN packet
      std::vector<Address> m_poas;  ///<@brief vector of Signatures

    };

    inline std::ostream &
    operator << (std::ostream &os, const DEN &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class DENException {};
  } //namespace nnn
} //namespace ns3

#endif // _NNN_DEN_HEADER_H_
