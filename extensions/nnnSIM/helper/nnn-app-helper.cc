/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Ilya Moiseenko
 *
 *  nnn-app-helper.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-app-helper.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-app-helper.cc.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/string.h>
#include <ns3-dev/ns3/names.h>

//#include <ns3-dev/ns3/nnn-app.h>

#ifdef NS3_MPI
#include "ns3/mpi-interface.h"
#endif

#include "nnn-app-helper.h"

NS_LOG_COMPONENT_DEFINE ("nnn.AppHelper");

namespace ns3 {
  namespace nnn {

    AppHelper::AppHelper (const std::string &app)
    {
      m_factory.SetTypeId (app);
    }

    void
    AppHelper::SetPrefix (const std::string &prefix)
    {
      m_factory.Set ("Prefix", StringValue(prefix));
    }

    void
    AppHelper::SetAttribute (std::string name, const AttributeValue &value)
    {
      m_factory.Set (name, value);
    }

    ApplicationContainer
    AppHelper::Install (Ptr<Node> node)
    {
      ApplicationContainer apps;
      Ptr<Application> app = InstallPriv (node);
      if (app != 0)
	apps.Add (app);

      return apps;
    }

    ApplicationContainer
    AppHelper::Install (std::string nodeName)
    {
      Ptr<Node> node = Names::Find<Node> (nodeName);
      return Install (node);
    }

    ApplicationContainer
    AppHelper::Install (NodeContainer c)
    {
      ApplicationContainer apps;
      for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
	{
	  Ptr<Application> app = InstallPriv (*i);
	  if (app != 0)
	    apps.Add (app);
	}

      return apps;
    }

    Ptr<Application>
    AppHelper::InstallPriv (Ptr<Node> node)
    {
#ifdef NS3_MPI
      if (MpiInterface::IsEnabled () &&
	  node->GetSystemId () != MpiInterface::GetSystemId ())
	{
	  // don't create an app if MPI is enabled and node is not in the correct partition
	  return 0;
	}
#endif

      Ptr<Application> app = m_factory.Create<Application> ();
      node->AddApplication (app);

      return app;
    }

  } // namespace nnn
} // namespace ns3
