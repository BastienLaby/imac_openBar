#if !defined  HAVE_IS_PAREN_STRING_H__
#define       HAVE_IS_PAREN_STRING_H__
// This file is part of the FXT library.
// Copyright (C) 2012, 2013 Joerg Arndt
// License: GNU General Public License version 3 or later,
// see the file COPYING.txt in the main directory.

#include "fxttypes.h"


template <typename Type>
inline bool is_paren_string(const Type *str, ulong n2)
// Return whether parenthesis string is valid.
// Works for any pair of symbols.
{
    if ( n2==0 )  return true;  // nothing to do
    if ( n2 & 1UL )  return false;  // must have even length

    const Type c1 = str[0];
    const Type c0 = str[n2-1];
    long s = 0;  // running sum
    for (ulong js=0; js<n2; ++js)
    {
        const Type c = str[js];
        if ( c == c1 )
        {
            ++s;
        }
        else
        {
            if ( c != c0 )  return false;
            --s;
            if ( s < 0 )  return false;
        }
    }
    return (s==0);
}
// -------------------------

#endif  // !defined HAVE_IS_PAREN_STRING_H__
