/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Alexander Afanasyev
 *
 *  nnn-app-face.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-app-face.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-app-face.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/log.h>

#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/simulator.h>

#include <ns3-dev/ns3/ndn-interest.h>
#include <ns3-dev/ns3/ndn-data.h>
#include <ns3-dev/ns3/ndnSIM/helper/ndn-header-helper.h>

#include "nnn-app-face.h"
#include "nnn-packets.h"
#include "../helper/nnn-header-helper.h"
#include "../apps/nnn-app.h"

NS_LOG_COMPONENT_DEFINE ("nnn.AppFace");

namespace ns3 {
  namespace nnn {

    NS_OBJECT_ENSURE_REGISTERED (AppFace);

    TypeId
    AppFace::GetTypeId ()
    {
      static TypeId tid = TypeId ("ns3::nnn::AppFace")
		.SetParent<Face> ()
		.SetGroupName ("Nnn")
		;
      return tid;
    }

    AppFace::AppFace (Ptr<App> app)
    : Face (app->GetNode ())
    , m_app (app)
    {
      NS_LOG_FUNCTION (this << app);

      NS_ASSERT (m_app != 0);
      SetFlags (Face::APPLICATION);
    }

    AppFace::~AppFace ()
    {
      NS_LOG_FUNCTION_NOARGS ();
    }

    AppFace::AppFace ()
    : Face (0)
    {
    }

    AppFace::AppFace (const AppFace &)
    : Face (0)
    {
    }

    AppFace&
    AppFace::operator= (const AppFace &)
    {
      return *((AppFace*)0);
    }

    bool
    AppFace::SendInterest (Ptr<const ndn::Interest> interest)
    {
      NS_LOG_FUNCTION (this << interest);

      if (!IsUp ())
	{
	  return false;
	}

      if (interest->GetNack () > 0)
	m_app->OnNack (interest);
      else
	m_app->OnInterest (interest);

      return true;
    }

    bool
    AppFace::SendData (Ptr<const ndn::Data> data)
    {
      NS_LOG_FUNCTION (this << data);

      if (!IsUp ())
	{
	  return false;
	}

      m_app->OnData (data);
      return true;
    }

    bool
    AppFace::SendSO (Ptr<const SO> soObject)
    {
      NS_LOG_FUNCTION (this << soObject);

      if (!IsUp ())
	{
	  return false;
	}
      return true;
    }

    bool
    AppFace::SendDO (Ptr<const DO> doObject)
    {
      NS_LOG_FUNCTION (this << doObject);

      if (!IsUp ())
	{
	  return false;
	}

      return true;
    }

    bool
    AppFace::SendDU (Ptr<const DU> duObject)
    {
      NS_LOG_FUNCTION (this << duObject);

      if (!IsUp ())
	{
	  return false;
	}

      return true;
    }

    std::ostream&
    AppFace::Print (std::ostream& os) const
    {
      os << "dev=local(" << GetId() << ")";
      return os;
    }
  } // namespace nnn
} // namespace ns3
