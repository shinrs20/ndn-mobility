
/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-du.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-du.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-du.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNNSIM_DU_H_
#define _NNNSIM_DU_H_

#include "../nnnsim-common-hdr.h"
#include "../nnnsim-common.h"

NNN_NAMESPACE_BEGIN

namespace wire
{
  namespace nnnSIM
  {

    class DU : public CommonHeader<nnn::DU>
    {
    public:
      DU ();
      DU (Ptr<nnn::DU> du_p);

      static Ptr<Packet>
      ToWire (Ptr<const nnn::DU> du_p);

      static Ptr<nnn::DU>
      FromWire (Ptr<Packet> packet);

      // from Header
      static TypeId GetTypeId (void);
      TypeId GetInstanceTypeId (void) const;
      uint32_t GetSerializedSize (void) const;
      void Serialize (Buffer::Iterator start) const;
      uint32_t Deserialize (Buffer::Iterator start);
    };
  } /* namespace nnnSIM */
} /* namespace wire */

#endif /* _WIRE_NNNSIM_DU_H_ */

NNN_NAMESPACE_END
