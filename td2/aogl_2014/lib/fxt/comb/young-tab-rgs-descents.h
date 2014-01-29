#if !defined  HAVE_YOUNG_TAB_RGS_DESCENTS_H__
#define       HAVE_YOUNG_TAB_RGS_DESCENTS_H__
// This file is part of the FXT library.
// Copyright (C) 2013 Joerg Arndt
// License: GNU General Public License version 3 or later,
// see the file COPYING.txt in the main directory.


#include "fxttypes.h"

//#include "jjassert.h"

inline ulong young_tab_rgs_descent_set(const ulong *a, ulong n, ulong *d)
{
    if ( n <= 1 )  return 0;

    ulong k = 0;
    for (ulong i=0; i<n-1; ++i)
    {
        if ( a[i+1] > a[i] )  // descent
        {
            d[k] = i + 1;
            k += 1;
        }
    }

    return k;
}
// -------------------------


inline ulong young_tab_rgs_major_index(const ulong *a, ulong n)
{
    if ( n <= 1 )  return 0;

    ulong mi = 0;
    for (ulong i=0; i<n-1; ++i)
        if ( a[i+1] > a[i] )   mi += i + 1;

    return mi;
}
// -------------------------


inline ulong young_tab_rgs_descent_number(const ulong *a, ulong n)
{
    if ( n <= 1 )  return 0;

    ulong dn = 0;
    for (ulong i=0; i<n-1; ++i)
        if ( a[i+1] > a[i] )   dn += 1;

    return dn;
}
// -------------------------



#endif // !defined HAVE_YOUNG_TAB_RGS_DESCENTS_H__
