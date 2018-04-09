/**
 * \file config.h
 */

#ifndef ATK_MODELING_CONFIG_H
#define ATK_MODELING_CONFIG_H

#ifdef _MSC_VER
#pragma warning( disable : 4275 4251 )
# ifdef ATK_SHARED
#  ifdef BUILD_ATK_MODELING
#   define ATK_MODELING_EXPORT __declspec( dllexport )
#  else
#   define ATK_MODELING_EXPORT __declspec( dllimport )
#  endif
# else
#  define ATK_MODELING_EXPORT
# endif
#else
# define ATK_MODELING_EXPORT
#endif

#endif
