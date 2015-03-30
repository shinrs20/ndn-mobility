/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-point2point-remote-channel.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-point2point-remote-channel.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-point2point-remote-channel.h.  If not, see <http://www.gnu.org/licenses/>.
 */

// This object connects two point-to-point net devices where at least one
// is not local to this simulator object.  It simply over-rides the transmit
// method and uses an MPI Send operation instead.

#ifndef NNN_POINT_TO_POINT_REMOTE_CHANNEL_H
#define NNN_POINT_TO_POINT_REMOTE_CHANNEL_H

#include "nnn-point2point-channel.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * \ingroup point-to-point
     */
    class FlexPointToPointRemoteChannel : public FlexPointToPointChannel
    {
    public:
      static TypeId GetTypeId (void);
      FlexPointToPointRemoteChannel ();
      ~FlexPointToPointRemoteChannel ();
      virtual bool TransmitStart (Ptr<Packet> p, Ptr<FlexPointToPointNetDevice> src, Time txTime);
    };
  }
}
#endif


