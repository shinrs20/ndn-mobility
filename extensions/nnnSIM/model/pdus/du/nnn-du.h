/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-du.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-du.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-du.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_DU_HEADER_H_
#define _NNN_DU_HEADER_H_

#include "../nnn-pdu.h"
#include "../nnn-data-pdus.h"
#include "../../nnn-naming.h"

namespace ns3
{
  namespace nnn
  {
    class DU : public DATAPDU
    {
    public:
      DU ();

      DU(Ptr<NNNAddress> src, Ptr<NNNAddress> dst, Ptr<Packet> payload);

      /**
       * @brief Copy constructor
       */
      DU (const DU &du_p);

      DU(const NNNAddress &src, const NNNAddress &dst, Ptr<Packet> payload);

      const NNNAddress&
      GetSrcName () const;

      const NNNAddress&
      GetDstName () const;

      Ptr<const NNNAddress>
      GetSrcNamePtr () const;

      Ptr<const NNNAddress>
      GetDstNamePtr () const;

      void
      SetSrcName (Ptr<NNNAddress> src);

      void
      SetSrcName (const NNNAddress &src);

      void
      SetDstName (Ptr<NNNAddress> dst);

      void
      SetDstName (const NNNAddress &dst);

      /**
       * @brief Print DU in plain-text to the specified output stream
       */
      void
      Print (std::ostream &os) const;

    private:
      Ptr<NNNAddress> m_src;
      Ptr<NNNAddress> m_dst;
    };

    inline std::ostream &
    operator << (std::ostream &os, const DU &i)
    {
      i.Print (os);
      return os;
    }

    /**
     * @brief Class for Interest parsing exception
     */
    class DUException {};

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* _NNN_DU_HEADER_H_ */
