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

#include "../nnn-pdu.h"
#include "../nnn-en-pdus.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * @ingroup nnn
     * @brief NNN EN packet (wire formats are defined in wire)
     **/
    class EN : public ENPDU
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
       * @param poas vector of PoA names
       **/
      EN (std::vector<Address> poas);

      /**
       * @brief Copy constructor
       */
      EN (const EN &en_p);

      /**
       * @brief Print EN in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      // NO_ASSIGN
      EN &
      operator = (const EN &other) { return *this; }
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
