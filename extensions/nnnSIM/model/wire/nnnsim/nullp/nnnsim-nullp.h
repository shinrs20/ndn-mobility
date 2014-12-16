/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-nullp.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-nullp.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-null.h.  If not, see <http://www.gnu.org/licenses/>.
 */

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
     * @brief Routines to serialize/deserialize NULL packets in nnnSIM format
     */
    class NULLp : public Header
    {
    public:
      NULLp ();
      NULLp (Ptr<nnn::NULLp> null_p);

      Ptr<nnn::NULLp>
      GetNULLp ();

      static Ptr<Packet>
      ToWire (Ptr<const nnn::NULLp> null_p);

      static Ptr<nnn::NULLp>
      FromWire (Ptr<Packet> packet);

      // from Header
      static TypeId GetTypeId (void);
      virtual TypeId GetInstanceTypeId (void) const;
      virtual void Print (std::ostream &os) const;
      virtual uint32_t GetSerializedSize (void) const;
      virtual void Serialize (Buffer::Iterator start) const;
      virtual uint32_t Deserialize (Buffer::Iterator start);

    private:
      Ptr<nnn::NULLp> m_null_p;
    };
  }
}

NNN_NAMESPACE_END
