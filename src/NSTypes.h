/***************************************************************************
                          NSTypes.h  -  description
                             -------------------
    begin                : Sun May 2 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : Variable type definitions for Neurosis

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSTYPES_H
#define NSTYPES_H

namespace neurosis {


// char typedefs
// char max value = 256
typedef unsigned char		nsuchar, uchar, u8;
typedef signed char			nsschar, schar, s8;
typedef char				nschar;

// short typedefs
// short max value = 65536
typedef unsigned short		nsushort, ushort, u16;
typedef signed short		nsshort, sshort, s16;

// int typedefs
// int max value = 4294967296
typedef unsigned int		nsuint, uint, u32;
typedef signed int			nsint, sint, s32;

// float typedefs
// float max non-fractional value = 4294967296
typedef float				nsfloat, f32;

// double typedefs
// double max non-fractional value = 18446744073709551616
typedef double				nsdouble, f64;


}; // namespace neurosis

#endif // ifndef NSTYPES_H
