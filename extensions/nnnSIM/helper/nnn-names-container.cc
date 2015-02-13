/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-names-container.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-names-container.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-names-container.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>

#include "nnn-names-container.h"

NS_LOG_COMPONENT_DEFINE ("nnn.NamesContainer");

namespace ns3 {
  namespace nnn {

    NamesContainer::NamesContainer ()
    {
    }

    NamesContainer::~NamesContainer ()
    {
    }

    void
    NamesContainer::addEntry (NamesContainerEntry nameEntry)
    {
      NS_LOG_FUNCTION (this);
      container.insert(nameEntry);

      Simulator::Schedule(nameEntry.m_lease_expire, &NamesContainer::cleanExpired, this);
    }

    void
    NamesContainer::addEntry (Ptr<NNNAddress> name, Time lease_expire)
    {
      NS_LOG_FUNCTION (this << name << lease_expire);
      container.insert(NamesContainerEntry(name, lease_expire));

      Simulator::Schedule(lease_expire, &NamesContainer::cleanExpired, this);
    }

    void
    NamesContainer::addEntry (Ptr<NNNAddress> name, Time lease_expire, Time renew)
    {
      NS_LOG_FUNCTION (this << name << lease_expire << renew);
      container.insert(NamesContainerEntry(name, lease_expire, renew));

      Simulator::Schedule(lease_expire, &NamesContainer::cleanExpired, this);
    }

    void
    NamesContainer::deleteEntry (NamesContainerEntry nameEntry)
    {
      NS_LOG_FUNCTION (this);
      container.erase(nameEntry);
    }

    void
    NamesContainer::deleteEntry (Ptr<NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << name);
      NamesContainerEntry tmp = findEntry (name);

      container.erase(tmp);
    }

    bool
    NamesContainer::foundName (Ptr<NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << name);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.find(name);

      if (it == names_index.end())
	return false;
      else
	return true;
    }

    NamesContainerEntry
    NamesContainer::findEntry (Ptr<NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << name);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.find(name);

      if (it != names_index.end())
	{
	  return *it;
	}
      else
	{
	  return NamesContainerEntry ();
	}
    }

    Ptr<NNNAddress>
    NamesContainer::findNewestName ()
    {
      NS_LOG_FUNCTION (this);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.end();

      it--;

      return it->m_name;
    }

    Time
    NamesContainer::findNameExpireTime (Ptr<NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << name);
      NamesContainerEntry tmp = findEntry(name);

      return tmp.m_lease_expire;
    }

    void
    NamesContainer::updateLeaseTime (Ptr<NNNAddress> name, Time lease_expire)
    {
      NS_LOG_FUNCTION (this << name << lease_expire);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.find(name);

      if (it != names_index.end())
	{
	  NamesContainerEntry tmp = *it;

	  tmp.m_lease_expire = lease_expire;
	  tmp.m_renew = lease_expire - Seconds (1);

	  if (names_index.replace(it, tmp))
	    {
	      Simulator::Schedule(lease_expire, &NamesContainer::cleanExpired, this);
	    }
	}
    }

    void
    NamesContainer::updateLeaseTime (Ptr<NNNAddress> name, Time lease_expire, Time renew)
    {
      NS_LOG_FUNCTION (this << name << lease_expire << renew);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.find(name);

      if (it != names_index.end())
	{
	  NamesContainerEntry tmp = *it;

	  tmp.m_lease_expire = lease_expire;
	  tmp.m_renew = renew;

	  if (names_index.replace(it, tmp))
	    {
	      Simulator::Schedule(lease_expire, &NamesContainer::cleanExpired, this);
	    }
	}
    }

    uint32_t
    NamesContainer::size()
    {
      return container.size();
    }

    bool
    NamesContainer::isEmpty()
    {
      return (container.size() == 0);
    }

    void
    NamesContainer::cleanExpired()
    {
      NS_LOG_FUNCTION (this);
      names_set_by_lease& lease_index = container.get<lease> ();
      Time now = Simulator::Now();

      //std::cout << "Deleting expired entries at " << now << std::endl;

      names_set_by_lease::iterator it = lease_index.begin();

      while (! isEmpty())
	{
	  if (it->m_lease_expire <= now)
	    {
	      deleteEntry(*it);
	      break;
	    }

	  ++it;
	}
    }

    void
    NamesContainer::printByAddress ()
    {
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.begin();

      std::cout << "NNN Address\t| Lease Expire\t| Renew" << std::endl;
      std::cout << "--------------------------------------------------" << std::endl;

      while (it != names_index.end())
	{
	  std::cout << *it;
	  ++it;
	}
    }

    void
    NamesContainer::printByLease ()
    {
      names_set_by_lease& lease_index = container.get<lease> ();
      names_set_by_lease::iterator it = lease_index.begin();

      std::cout << "NNN Address\t| Lease Expire\t| Renew" << std::endl;
      std::cout << "--------------------------------------------------" << std::endl;

      while (it != lease_index.end ())
	{
	  std::cout << *it;
	  ++it;
	}
    }

} /* namespace nnn */
} /* namespace ns3 */
