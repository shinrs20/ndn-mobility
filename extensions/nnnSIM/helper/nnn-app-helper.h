/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *   Original template made for ndnSIM for University of California,
 *   Los Angeles by Ilya Moiseenko
 *
 *  nnn-app-helper.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-app-helper.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-app-helper.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_APP_HELPER_H
#define NNN_APP_HELPER_H

#include <ns3-dev/ns3/application-container.h>
#include <ns3-dev/ns3/attribute.h>
#include <ns3-dev/ns3/node-container.h>
#include <ns3-dev/ns3/object-factory.h>
#include <ns3-dev/ns3/ptr.h>

namespace ns3 {
  namespace nnn {

    /**
     * @ingroup nnn-helpers
     * \brief A helper to make it easier to instantiate an ns3::NnnConsumer Application
     * on a set of nodes.
     */
    class AppHelper
    {
    public:

      /**
       * \brief Create an NnnAppHelper to make it easier to work with Nnn apps
       *
       * \param app Class of the application
       */
      AppHelper (const std::string &prefix);

      /**
       * @brief Set the prefix consumer will be requesting
       */
      void
      SetPrefix (const std::string &prefix);

      /**
       * \brief Helper function used to set the underlying application attributes.
       *
       * \param name the name of the application attribute to set
       * \param value the value of the application attribute to set
       */
      void SetAttribute (std::string name, const AttributeValue &value);

      /**
       * Install an ns3::NdnConsumer on each node of the input container
       * configured with all the attributes set with SetAttribute.
       *
       * \param c NodeContainer of the set of nodes on which an NdnConsumer
       * will be installed.
       * \returns Container of Ptr to the applications installed.
       */
      ApplicationContainer Install (NodeContainer c);

      /**
       * Install an ns3::NnnConsumer on the node configured with all the
       * attributes set with SetAttribute.
       *
       * \param node The node on which an NnnConsumer will be installed.
       * \returns Container of Ptr to the applications installed.
       */
      ApplicationContainer Install (Ptr<Node> node);

      /**
       * Install an ns3::NnnConsumer on the node configured with all the
       * attributes set with SetAttribute.
       *
       * \param nodeName The node on which an NnnConsumer will be installed.
       * \returns Container of Ptr to the applications installed.
       */
      ApplicationContainer Install (std::string nodeName);

    private:
      /**
       * \internal
       * Install an ns3::NnnConsumer on the node configured with all the
       * attributes set with SetAttribute.
       *
       * \param node The node on which an NdnConsumer will be installed.
       * \returns Ptr to the application installed.
       */
      Ptr<Application> InstallPriv (Ptr<Node> node);
      ObjectFactory m_factory;
    };

  } // namespace nnn
} // namespace ns3

#endif // NNN_APP_HELPER_H
