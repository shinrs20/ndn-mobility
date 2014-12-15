/*
 * nnn-packet.h
 *
 *  Created on: Dec 15, 2014
 *      Author: jelfn
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
