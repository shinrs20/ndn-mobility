/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-oen.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-oen.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-oen.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_OEN_HEADER_H_
#define _NNN_OEN_HEADER_H_

#include "../nnn-pdu.h"
#include "../nnn-en-pdus.h"
#include "../../nnn-naming.h"

namespace ns3
{
  namespace nnn
  {

    class OEN : public ENPDU
    {
    public:
      OEN ();

      OEN (Ptr<NNNAddress> name);

      OEN (const NNNAddress &name);

      OEN (const OEN &oen_p);

      virtual
      ~OEN ();

      /**
       * \brief Get name
       *
       * Gets name used in the PDU
       **/
      const NNNAddress&
      GetName () const;

      /**
       * @brief Get smart pointer to the name (to avoid extra memory usage)
       */
      Ptr<const NNNAddress>
      GetNamePtr () const;

      /**
       * \brief Set name
       *
       * @param name smart pointer to name
       *
       **/
      void
      SetName (Ptr<NNNAddress> name);

      /**
       * \brief Another variant to set name
       *
       * @param name const reference to NNNAddress object
       *
       **/
      void
      SetName (const NNNAddress &name);

      /**
       * \brief Get lease time
       *
       * Gets lease time used in the PDU
       **/
      Time
      GetLeasetime() const;

      /**
       * \brief Set lease time
       *
       * @param lease lease time for NNNAddress
       *
       **/
      void
      SetLeasetime (Time lease);

      /**
       * @brief Print OEN in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      OEN &
      operator = (const OEN &other) { return *this; }

      Time m_lease;             ///< @brief Lease absolute time for NNN Address
      Ptr<NNNAddress> m_name;   ///< @brief Destination NNN Address handed
    };

    inline std::ostream &
    operator << (std::ostream &os, const OEN &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class OENException {};

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* _NNN_OEN_HEADER_H_ */
