/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-do.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-do.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-do.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../nnnsim-common-hdr.h"
#include "../nnnsim-common.h"
NNN_NAMESPACE_BEGIN
/**
 * @brief Namespace encapsulating wire operations
 */
namespace wire {

  /**
   * @brief Namespace for nnnSIM wire format operations
   */
  namespace nnnSIM {

    /**
     * @brief Routines to serialize/deserialize DO packets in nnnSIM format
     */
    class DO : public CommonHeader<nnn::DO>
    {
    public:
      DO ();
      DO (Ptr<nnn::DO> do_p);

      static Ptr<Packet>
      ToWire (Ptr<const nnn::DO> do_p);

      static Ptr<nnn::DO>
      FromWire (Ptr<Packet> packet);

      // from Header
      static TypeId GetTypeId (void);
      TypeId GetInstanceTypeId (void) const;
      uint32_t GetSerializedSize (void) const;
      void Serialize (Buffer::Iterator start) const;
      uint32_t Deserialize (Buffer::Iterator start);
    };

  }
}

NNN_NAMESPACE_END
