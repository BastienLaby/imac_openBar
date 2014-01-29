#if !defined  HAVE_SUBSET_LEX_COMPARE_H__
#define       HAVE_SUBSET_LEX_COMPARE_H__
// This file is part of the FXT library.
// Copyright (C) 2013 Joerg Arndt
// License: GNU General Public License version 3 or later,
// see the file COPYING.txt in the main directory.

#include "fxttypes.h"


template <typename Type>
inline int subset_lex_compare(const Type *a, ulong na, const Type *b, ulong nb)
// Compare a[] and b[] subset-lexicographically.
// Return +1 if a[] >> b[], -1 if a[] << b[], 0 if a[]==b[].
{
    ulong ea = na;  // last index such that a[ea-1] != 0
    while ( ea-- )  if ( a[ea] != 0 )  break;
    ++ea;

    ulong eb = nb;  // last index such that b[eb-1] != 0
    while ( eb-- )  if ( b[eb] != 0 )  break;
    ++eb;

    const ulong m = (ea < eb ? ea : eb);  // min(ea, eb)
    for (ulong j=0; j<m; ++j)
    {
        ulong aj = a[j],  bj = b[j];
        if ( aj != bj )
        {
            if ( aj < bj )
            {
                if ( j == ea-1 )  return -1;
                else              return +1;
            }
            else  // bj < aj
            {
                if ( j == eb-1 )  return +1;
                else              return -1;
            }
        }
    }

    if ( ea == eb )  return 0;
    return ( ea > eb ? +1 : -1 );
}
// -------------------------


#endif  // !defined HAVE_SUBSET_LEX_COMPARE_H__
