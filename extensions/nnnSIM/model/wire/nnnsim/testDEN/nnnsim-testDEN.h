/*
 * nnnsim-testDEN.h
 *
 *  Created on: Dec 16, 2014
 *      Author: jelfn
 */

#include "../nnnsim-common-hdr.h"
#include "../nnnsim-common.h"

#ifndef NNNSIM_TESTDEN_H_
#define NNNSIM_TESTDEN_H_


namespace ns3
{
  namespace nnn
  {
    namespace wire
    {
      namespace nnnSIM
      {

	class TestDEN : public CommonHeader<nnn::DEN>
	{
	public:
	  TestDEN ();

	  TestDEN (Ptr<nnn::DEN> t_p);

	  ~TestDEN ();

	  static Ptr<Packet>
	  ToWire (Ptr<const nnn::DEN> t_p);

	  static Ptr<nnn::DEN>
	  FromWire (Ptr<Packet> packet);

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

#endif /* NNNSIM_TESTDEN_H_ */
