/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-so.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-so.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-so.h.  If not, see <http://www.gnu.org/licenses/>.
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
     * @brief Routines to serialize/deserialize SO packets in nnnSIM format
     */
    class SO : public Header
    {
    public:
      SO ();
      SO (Ptr<nnn::SO> so);

      Ptr<nnn::SO>
      GetSO ();

      static Ptr<Packet>
      ToWire (Ptr<const nnn::SO> so);

      static Ptr<nnn::SO>
      FromWire (Ptr<Packet> packet);

      // from Header
      static TypeId GetTypeId (void);
      virtual TypeId GetInstanceTypeId (void) const;
      virtual void Print (std::ostream &os) const;
      virtual uint32_t GetSerializedSize (void) const;
      virtual void Serialize (Buffer::Iterator start) const;
      virtual uint32_t Deserialize (Buffer::Iterator start);

    private:
      Ptr<nnn::SO> m_so_p;
    };

  }
}

NNN_NAMESPACE_END
