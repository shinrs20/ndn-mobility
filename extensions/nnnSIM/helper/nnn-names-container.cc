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

namespace ns3
{
  namespace nnn
  {
    NamesContainer::NamesContainer ()
    : renewName (MakeNullCallback <void> ())
    , hasNoName (MakeNullCallback <void> ())
    , defaultRenewal (Seconds(30))
    {
    }

    NamesContainer::~NamesContainer ()
    {
    }

    void
    NamesContainer::RegisterCallbacks (const Callback<void> renewal, const Callback<void> leaseagain)
    {
      renewName = renewal;
      hasNoName = leaseagain;
    }

    void
    NamesContainer::SetDefaultRenewal (Time renew)
    {
      defaultRenewal = renew;
    }

    Time
    NamesContainer::GetDefaultRenewal ()
    {
      return defaultRenewal;
    }

    void
    NamesContainer::addEntry (Ptr<const NNNAddress> name, Time lease_expire, bool fixed)
    {
      NS_LOG_FUNCTION (this << *name << lease_expire);

      // We assume that the lease time gives us the absolute expiry time
      // We need to calculate the relative time for the Schedule function
      Time now = Simulator::Now ();
      Time relativeExpireTime = lease_expire - now;

      NS_LOG_INFO ("Checking remaining lease time " << relativeExpireTime << " for (" << *name << ") at " << now);

      // If the relative expire time is above 0, we can save it
      if (relativeExpireTime.IsStrictlyPositive())
	{
	  NS_LOG_INFO ("Lease time is positive, add (" << *name << "), lease until " << lease_expire );

	  // We need to save the lease and renewal time in absolute time
	  container.insert(NamesContainerEntry(name, lease_expire, lease_expire - defaultRenewal, fixed));
	  if (!fixed)
	    {
	      // The Schedulers are in relative time
	      Simulator::Schedule((relativeExpireTime - defaultRenewal), &NamesContainer::willAttemptRenew, this);
	      Simulator::Schedule(relativeExpireTime, &NamesContainer::cleanExpired, this);
	    }
	}
    }

    void
    NamesContainer::deleteEntry (NamesContainerEntry nameEntry)
    {
      NS_LOG_FUNCTION (this);
      container.erase(nameEntry);
    }

    void
    NamesContainer::deleteEntry (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << *name);

      if (!isEmpty())
	{
	  NamesContainerEntry tmp = findEntry (name);

	  container.erase(tmp);
	}
    }

    bool
    NamesContainer::foundName (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << *name);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.find(name);

      if (it == names_index.end())
	return false;
      else
	return true;
    }

    NamesContainerEntry
    NamesContainer::findEntry (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << *name);
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

    Ptr<const NNNAddress>
    NamesContainer::findNewestName ()
    {
      NS_LOG_FUNCTION (this);
      names_set_by_lease& lease_index = container.get<lease> ();
      names_set_by_lease::iterator it = lease_index.begin();

      if (it != lease_index.end ())
	return it->m_name;
      else
	return Create<const NNNAddress> ();
    }

    Time
    NamesContainer::findNameExpireTime (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << *name);
      NamesContainerEntry tmp = findEntry(name);

      return tmp.m_lease_expire;
    }

    void
    NamesContainer::updateLeaseTime (Ptr<const NNNAddress> name, Time lease_expire)
    {
      NS_LOG_FUNCTION (this << *name << lease_expire);
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.find(name);

      Time absoluteLeaseTime = Simulator::Now () + lease_expire;
      bool change = false;

      if (it != names_index.end())
	{
	  NamesContainerEntry tmp = *it;

	  tmp.m_lease_expire = lease_expire;
	  tmp.m_renew_time = lease_expire -defaultRenewal;

	  if (names_index.replace(it, tmp))
	    {
	      Time relativeExpireTime = lease_expire - Simulator::Now ();

	      if (relativeExpireTime.IsStrictlyPositive())
		{
		  // Remember the scheduler is expressed in relative time
		  Simulator::Schedule(lease_expire, &NamesContainer::cleanExpired, this);
		}
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

    bool
    NamesContainer::isFixed (Ptr<const NNNAddress> name)
    {
      return findEntry (name).m_fixed;
    }

    void
    NamesContainer::cleanExpired()
    {
      NS_LOG_FUNCTION (this);
      names_set_by_lease& lease_index = container.get<lease> ();
      Time now = Simulator::Now();

      names_set_by_lease::iterator it = lease_index.begin();

      while (! isEmpty() && it != lease_index.end ())
	{
	  if (it->m_lease_expire <= now &&  !it->m_fixed)
	    {
	      deleteEntry(*it);
	      break;
	    }

	  ++it;
	}

      // The container is actually empty, callback
      if (isEmpty ())
	{
	  if (!hasNoName.IsNull())
	    hasNoName ();
	}
    }

    void
    NamesContainer::clear()
    {
      NS_LOG_FUNCTION (this);

      names_set_by_lease& lease_index = container.get<lease> ();
      names_set_by_lease::iterator it = lease_index.begin();

      while (! isEmpty())
	{
	  deleteEntry(*it);
	  ++it;
	}
    }

    void
    NamesContainer::willAttemptRenew ()
    {
      NS_LOG_FUNCTION (this);
      if (size () == 1)
	if (!renewName.IsNull())
	  renewName ();
    }

    void
    NamesContainer::Print (std::ostream &os) const
    {
      const names_set_by_lease& lease_index = container.get<lease> ();

      std::copy(lease_index.begin (), lease_index.end (), std::ostream_iterator<names_set::value_type> (os));
    }

    void
    NamesContainer::printByAddress ()
    {
      names_set_by_name& names_index = container.get<address> ();
      names_set_by_name::iterator it = names_index.begin();

      std::cout << "NNN Address\t| Lease Expire\t| Renew\t| Fixed" << std::endl;
      std::cout << "-----------------------------------------------------" << std::endl;

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

      std::cout << "NNN Address\t| Lease Expire\t| Renew\t| Fixed" << std::endl;
      std::cout << "-----------------------------------------------------" << std::endl;

      while (it != lease_index.end ())
	{
	  std::cout << *it;
	  ++it;
	}
    }

    std::ostream&
    operator<< (std::ostream& os, const NamesContainer &names)
    {
      names.Print (os);
      return os;
    }
  } /* namespace nnn */
} /* namespace ns3 */
