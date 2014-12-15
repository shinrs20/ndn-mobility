/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-packet.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-packet.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-packet.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_PACKET_H_
#define NNN_PACKET_H_

namespace ns3
{
  namespace nnn
  {
    /**
     * @brief enum for NNN PacketId for NNN Packets
     */
    enum NNN_PDU_TYPE { NULL_NNN, SO_NNN, DO_NNN, EN_NNN, AEN_NNN, REN_NNN, DEN_NNN, INF_NNN};

    /**
     * @brief enum for payload type the NNN data transmission packets are carrying
     */
    enum NNN_PDU_TRANS { NDN_NNN = 1 };

    /**
     * @brief enum for NNN version being used
     */
    enum NNN_VER { A_NNN = 0 };

    class NNNPacket
    {
    public:
      NNNPacket ();
      virtual
      ~NNNPacket ();

    protected:

    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_PACKET_H_ */
