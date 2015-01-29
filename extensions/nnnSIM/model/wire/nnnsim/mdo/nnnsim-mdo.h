/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnnsim-mdo.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnnsim-mdo.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnnsim-mdo.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NNNSIM_MDO_H_
#define NNNSIM_MDO_H_

#include "../nnnsim-common-hdr.h"
#include "../nnnsim-common.h"

namespace ns3
{
  namespace nnn
  {
    namespace wire
    {
      namespace nnnSIM
      {
	class MDO : public CommonHeader<nnn::MDO>
	{
	public:
	  MDO ();
	  MDO (Ptr<nnn::MDO> mdo_p);

	  static Ptr<Packet>
	  ToWire (Ptr<const nnn::MDO> mdo_p);

	  static Ptr<nnn::MDO>
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
  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNNSIM_MDO_H_ */
