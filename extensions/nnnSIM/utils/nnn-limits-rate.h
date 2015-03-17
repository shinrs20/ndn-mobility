/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-limits-rate.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-limits-rate.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-limits-rate.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_LIMITS_RATE_H_
#define	_NNN_LIMITS_RATE_H_

#include "nnn-limits.h"
#include <ns3-dev/ns3/nstime.h>

namespace ns3 {
  namespace nnn {

    /**
     * \ingroup nnn-fw
     * \brief Structure to manage limits for outstanding interests
     */
    class LimitsRate : public Limits
    {
    public:
      typedef Limits super;

      static TypeId
      GetTypeId ();

      /**
       * \brief Constructor
       * \param prefix smart pointer to the prefix for the FIB entry
       */
      LimitsRate ()
      : m_isLeakScheduled (false)
      , m_bucketMax (0)
      , m_bucketLeak (1)
      , m_bucket (0)
      { }

      virtual
      ~LimitsRate () { }

      virtual void
      SetLimits (double rate, double delay);

      virtual
      double
      GetMaxLimit () const
      {
	return GetMaxRate ();
      }

      /**
       * @brief Check if Interest limit is reached (token bucket is not empty)
       */
      virtual bool
      IsBelowLimit ();

      /**
       * @brief Get token from the bucket
       */
      virtual void
      BorrowLimit ();

      /**
       * @brief Does nothing (token bucket leakage is time-dependent only)
       */
      virtual void
      ReturnLimit ();

      /**
       * @brief Update normalized amount that should be leaked every second (token bucket leak rate) and leak rate
       */
      virtual void
      UpdateCurrentLimit (double limit);

      /**
       * @brief Get normalized amount that should be leaked every second (token bucket leak rate)
       */
      virtual double
      GetCurrentLimit () const
      {
	return m_bucketLeak;
      }

      virtual double
      GetCurrentLimitRate () const
      {
	return m_bucketLeak;
      }

    protected:
      // from Node
      void
      NotifyNewAggregate ();

    private:
      /**
       * @brief Leak bucket, assuming `interval' seconds between leakages
       *
       * @param interval Time interval for leakage. Used to calculate size of the leak
       */
      void
      LeakBucket (double interval);
    private:
      bool m_isLeakScheduled;

      double m_bucketMax;   ///< \brief Maximum Interest allowance for this face (maximum tokens that can be issued at the same time)
      double m_bucketLeak;  ///< \brief Normalized amount that should be leaked every second (token bucket leak rate)
      double m_bucket;      ///< \brief Value representing current size of the Interest allowance for this face (current size of token bucket)

      Time m_leakRandomizationInteral;
    };
  } // namespace nnn
} // namespace ns3

#endif // _NNN_LIMITS_RATE_H_
