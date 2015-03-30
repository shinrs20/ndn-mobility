/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-ppp-header.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   nnn-ppp-header.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-ppp-header.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NNN_PPP_HEADER_H_
#define NNN_PPP_HEADER_H_

#include <map>
#include <ns3-dev/ns3/header.h>

namespace ns3
{
  namespace nnn
  {
    class FlexPppHeader : public ns3::Header
    {
    public:
      FlexPppHeader ();

      virtual
      ~FlexPppHeader ();

      static TypeId
      GetTypeId (void);

      virtual TypeId
      GetInstanceTypeId (void) const;

      virtual void
      Print (std::ostream &os) const;
      virtual void
      Serialize (Buffer::Iterator start) const;
      virtual uint32_t
      Deserialize (Buffer::Iterator start);
      virtual uint32_t
      GetSerializedSize (void) const;

      void
      SetProtocol (uint16_t protocol);

      uint16_t
      GetProtocol (void);

      void
      AddProtocol (uint16_t protocol, std::string descrip);

    private:
      uint16_t m_protocol;
      std::map<uint16_t, std::string> m_supportedProtocols;
    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_PPP_HEADER_H_ */
