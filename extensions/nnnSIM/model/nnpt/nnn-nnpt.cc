/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Takahiro Miyamoto <mt3.mos@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nnpt.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nnpt.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nnpt.cc.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nnn-nnpt.h"

#include <ns3-dev/ns3/log.h>

NS_LOG_COMPONENT_DEFINE ("nnn.nnpt");

namespace ns3
{
  namespace nnn
  {
    NS_OBJECT_ENSURE_REGISTERED (NNPT);

    TypeId
    NNPT::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::nnn::NNPT") // cheating ns3 object system
	  .SetParent<Object> ()
	  .SetGroupName ("Nnn")
	  .AddConstructor<NNPT> ()
	  ;
      return tid;
    }

    NNPT::NNPT() {
    }

    NNPT::~NNPT() {
    }

    void
    NNPT::addEntry (Ptr<const NNNAddress> oldName, Ptr<const NNNAddress> newName, Time lease_expire)
    {
      NS_LOG_FUNCTION (this << *oldName << *newName << lease_expire);

      if (!foundOldName(oldName) && !foundOldName(newName))
        {
	  // We assume that the lease time gives us the absolute expiry time
	  // We need to calculate the relative time for the Schedule function
	  Time now = Simulator::Now ();
	  Time relativeExpireTime = lease_expire - now;

	  NS_LOG_INFO ("addEntry : Checking remaining lease time " << relativeExpireTime << " for (" << *newName << ") at " << now);

          // If the relative expire time is above 0, we can save it
          if (relativeExpireTime.IsStrictlyPositive())
            {
              NS_LOG_INFO ("addEntry : Adding entry for (" << *oldName << ") ->  (" << *newName  << ")");
              container.insert(nnpt::Entry(oldName, newName, lease_expire));
              Simulator::Schedule(relativeExpireTime, &NNPT::cleanExpired, this);
            }
        }
      else
        {
          NS_LOG_INFO("addEntry : Found either " << *oldName << " or " << *newName << " already in NNPT");
        }
    }

    void
    NNPT::deleteEntry (Ptr<const NNNAddress> oldName)
    {
      NS_LOG_FUNCTION (this);
      nnpt::Entry tmp = findEntry (oldName);
      container.erase(tmp);
    }

    void
    NNPT::deleteEntry (nnpt::Entry nnptEntry)
    {
      NS_LOG_FUNCTION (this);
      container.erase(nnptEntry);
    }

    bool
    NNPT::foundOldName (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_oldname& names_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = names_index.find(name);

      if (it == names_index.end())
	{
	  NS_LOG_INFO ("Didn't find old name (" << *name << ")");
	  return false;
	}
      else
	{
	  NS_LOG_INFO ("Found old name (" << *name << ")");
	  return true;
	}
    }

    bool
    NNPT::foundNewName (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << *name);
      pair_set_by_newname& names_index = container.get<newname> ();
      pair_set_by_newname::iterator it = names_index.find(name);

      nnpt::Entry tmp = *it;

      NS_LOG_INFO ("foundNewName : schedule errors " << tmp.m_lease_expire << " (" << tmp.m_oldName << ") -> (" << tmp.m_newName << ")" );

      if (it == names_index.end())
	{
	  NS_LOG_INFO ("Didn't find new name (" << *name << ")");
	  return false;
	}
      else
	{
	  NS_LOG_INFO ("Found new name (" << *name << ")");
	  return true;
	}
    }

    const NNNAddress&
    NNPT::findPairedName (Ptr<const NNNAddress> oldName)
    {
      NS_LOG_FUNCTION (this << *oldName);

      return *findPairedNamePtr(oldName);
    }

    const NNNAddress&
    NNPT::findPairedOldName (Ptr<const NNNAddress> newName)
    {
      NS_LOG_FUNCTION (this << *newName);

      return *findPairedOldNamePtr(newName);
    }

    Ptr<const NNNAddress>
    NNPT::findPairedNamePtr (Ptr<const NNNAddress> oldName)
    {
      NS_LOG_FUNCTION (this << *oldName);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(oldName);

      if (it != pair_index.end())
	{
	  nnpt::Entry tmp;
	  // Check if there is a newer entry
	  while (true)
	    {
	      tmp = *it;
	      it = pair_index.find(tmp.m_newName);
	      if (it == pair_index.end())
		break;
	    }
	  return tmp.m_newName;
	}
      else
	{
	  return oldName;
	}
    }

    Ptr<const NNNAddress>
    NNPT::findPairedOldNamePtr (Ptr<const NNNAddress> newName)
    {
      NS_LOG_FUNCTION (this << *newName);
      pair_set_by_newname& pair_index = container.get<newname> ();
      pair_set_by_newname::iterator it = pair_index.find(newName);

      if (it != pair_index.end ())
	{
	  nnpt::Entry tmp;
	  // Check if there is a newer entry
	  while (true)
	    {
	      tmp = *it;
	      it = pair_index.find (tmp.m_oldName);
	      if (it == pair_index.end ())
		break;
	    }
	  return tmp.m_oldName;
	}
      else
	{
	  return newName;
	}
    }

    nnpt::Entry
    NNPT::findEntry (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this << *name);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(name);

      if (it != pair_index.end())
	{
	  return *it;
	}
      else
	{
	  return nnpt::Entry ();
	}
    }

    void
    NNPT::updateLeaseTime (Ptr<const NNNAddress> oldName, Time lease_expire)
    {
      NS_LOG_FUNCTION (this << *oldName << lease_expire);
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.find(oldName);

      Time relativeExpireTime = lease_expire - Simulator::Now ();

      // If the relative expire time is above 0, schedule the next clean
      if (relativeExpireTime.IsStrictlyPositive())
	{
	  if (it != pair_index.end())
	    {
	      nnpt::Entry tmp = *it;

	      tmp.m_lease_expire = lease_expire;

	      if (pair_index.replace(it, tmp))
		{
		  Simulator::Schedule(relativeExpireTime, &NNPT::cleanExpired, this);
		}
	    }
	}
    }

    uint32_t
    NNPT::size ()
    {
      NS_LOG_FUNCTION (this);
      return container.size();
    }

    bool
    NNPT::isEmpty ()
    {
      NS_LOG_FUNCTION (this);
      return (container.size() == 0);
    }

    Time
    NNPT::findNameExpireTime (Ptr<const NNNAddress> name)
    {
      NS_LOG_FUNCTION (this);
      nnpt::Entry tmp = findEntry(name);

      return tmp.m_lease_expire;
    }

    Time
    NNPT::findNameExpireTime (nnpt::Entry nnptEntry)
    {
      NS_LOG_FUNCTION (this);
      return nnptEntry.m_lease_expire;
    }

    void
    NNPT::cleanExpired ()
    {
      NS_LOG_FUNCTION (this);
      pair_set_by_lease& lease_index = container.get<st_lease> ();
      Time now = Simulator::Now ();

      pair_set_by_lease::iterator it = lease_index.begin();

      while (! isEmpty())
	{
	  if (it->m_lease_expire <= now)
	    {
	      NS_LOG_INFO ("cleanExpired : removing (" << *it->m_oldName << ") -> (" << *it->m_newName << ")");
	      deleteEntry(*it);
	      break;
	    }
	  ++it;
	}
    }

    void
    NNPT::Print (std::ostream &os) const
    {
      const pair_set_by_oldname& pair_index = container.get<oldname> ();

      std::copy(pair_index.begin (), pair_index.end (), std::ostream_iterator<pair_set::value_type> (os));
    }

    void
    NNPT::printByAddress ()
    {
      pair_set_by_oldname& pair_index = container.get<oldname> ();
      pair_set_by_oldname::iterator it = pair_index.begin();

      std::cout << "Old Address\t| New Address\t| Lease Expire" << std::endl;
      std::cout << "-------------------------------------------------" << std::endl;

      while (it != pair_index.end())
	{
	  std::cout << *it;
	  ++it;
	}
    }

    void
    NNPT::printByLease ()
    {
      pair_set_by_lease& lease_index = container.get<st_lease> ();
      pair_set_by_lease::iterator it = lease_index.begin();

      std::cout << "NNN Address\t| New Address\t| Lease Expire" << std::endl;
      std::cout << "-------------------------------------------------" << std::endl;

      while (it != lease_index.end ())
	{
	  std::cout << *it;
	  ++it;
	}
    }

    std::ostream&
    operator<< (std::ostream& os, const NNPT &nnpt)
    {
      nnpt.Print (os);
      return os;
    }

  } /* namespace nnn */
} /* namespace ns3 */
