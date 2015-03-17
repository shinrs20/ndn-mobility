/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2015 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-limits-window.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-limits-window.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-limits-window.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _NNN_LIMITS_WINDOW_H_
#define	_NNN_LIMITS_WINDOW_H_

#include "nnn-limits.h"

namespace ns3
{
  namespace nnn
  {
    /**
     * \ingroup nnn-fw
     * \brief Structure to manage limits for outstanding interests (window-based limiting)
     */
    class LimitsWindow : public Limits
    {
    public:
      typedef Limits super;

      static TypeId
      GetTypeId ();

      /**
       * @brief Default Constructor
       */
      LimitsWindow ()
      : m_outstanding (0)
      { }

      /**
       * @brief Virtual destructor
       */
      virtual
      ~LimitsWindow () { }

      // from ndn::Limits

      virtual void
      SetLimits (double rate, double delay)
      {
	super::SetLimits (rate, delay);

	m_curMaxLimit = GetMaxRate () * GetMaxDelay ();
      }

      virtual
      double
      GetMaxLimit () const
      {
	return GetMaxRate () * GetMaxDelay ();
      }

      virtual void
      UpdateCurrentLimit (double limit);

      virtual double
      GetCurrentLimit () const
      {
	return m_curMaxLimit;
      }

      virtual double
      GetCurrentLimitRate () const
      {
	return m_curMaxLimit / GetMaxDelay ();
      }

      /**
       * @brief Check if current interest window (number of pending interests) if less than maximum
       */
      virtual bool
      IsBelowLimit ();

      /**
       * @brief Increase current window of outstanding interests
       */
      virtual void
      BorrowLimit ();

      /**
       * @brief Decrease current window of outstanding interests
       */
      virtual void
      ReturnLimit ();

    private:
      TracedValue< double > m_curMaxLimit;
      TracedValue< double > m_outstanding;
    };


  } // namespace nnn
} // namespace ns3

#endif // _NNN_LIMITS_WINDOW_H_
