/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev and Saran Tarnoi
 *
 *  nnn-link-control-helper.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-link-control-helper.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-link-control-helper.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_LINK_CONTROL_HELPER_H_
#define NNN_LINK_CONTROL_HELPER_H_

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/node.h>

namespace ns3
{
  namespace nnn
  {

    class LinkControlHelper
    {
    public:
      static void
      FailLink (Ptr<Node> node1, Ptr<Node> node2);

      static void
      FailLinkByName (const std::string &node1, const std::string &node2);

      static void
      UpLink (Ptr<Node> node1, Ptr<Node> node2);

      static void
      UpLinkByName (const std::string &node1, const std::string &node2);
    };

  } /* namespace nnn */
} /* namespace ns3 */

#endif /* NNN_LINK_CONTROL_HELPER_H_ */
