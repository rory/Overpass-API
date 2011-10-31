#ifndef DE__OSM3S___OVERPASS_API__CORE__INDEX_COMPUTATIONS_H
#define DE__OSM3S___OVERPASS_API__CORE__INDEX_COMPUTATIONS_H

#include "basic_types.h"

#include <algorithm>
#include <vector>

using namespace std;

typedef int int32;
typedef unsigned int uint32;

inline uint32 ll_upper(uint32 ilat, int32 ilon);
inline uint32 upper_ilat(uint32 quadtile);
inline uint32 upper_ilon(uint32 quadtile);
inline uint32 calc_index(const vector< uint32 >& node_idxs);
inline vector< Uint32_Index > calc_node_children(const vector< uint32 >& way_rel_idxs);
inline vector< Uint31_Index > calc_children(const vector< uint32 >& way_rel_idxs);
inline vector< uint32 > calc_parents(const vector< uint32 >& node_idxs);
inline set< pair< Uint31_Index, Uint31_Index > > calc_parents
    (const set< pair< Uint31_Index, Uint31_Index > >& total_idxs);

/** ------------------------------------------------------------------------ */

struct Uint31_Compare
{
  bool operator()(uint32 a, uint32 b) const
  {
    return (a & 0x7fffffff) < (b & 0x7fffffff);
  }
};

inline uint32 ll_upper(uint32 ilat, int32 ilon)
{
  uint32 result = 0;
  
  for (uint32 i(0); i < 16; ++i)
  {
    result |= ((0x1<<(i+16))&ilat)>>(15-i);
    result |= ((0x1<<(i+16))&(uint32)ilon)>>(16-i);
  }
  
  return result;
}

inline uint32 upper_ilat(uint32 quadtile)
{
  uint32 result = 0;
  
  result |= (quadtile & 0x2)>>1;
  result |= (quadtile & 0x8)>>2;
  result |= (quadtile & 0x20)>>3;
  result |= (quadtile & 0x80)>>4;
  result |= (quadtile & 0x200)>>5;
  result |= (quadtile & 0x800)>>6;
  result |= (quadtile & 0x2000)>>7;
  result |= (quadtile & 0x8000)>>8;
  result |= (quadtile & 0x20000)>>9;
  result |= (quadtile & 0x80000)>>10;
  result |= (quadtile & 0x200000)>>11;
  result |= (quadtile & 0x800000)>>12;
  result |= (quadtile & 0x2000000)>>13;
  result |= (quadtile & 0x8000000)>>14;
  result |= (quadtile & 0x20000000)>>15;
  
  return result;
}

inline uint32 upper_ilon(uint32 quadtile)
{
  uint32 result = 0;
  
  result |= (quadtile & 0x1);
  result |= (quadtile & 0x4)>>1;
  result |= (quadtile & 0x10)>>2;
  result |= (quadtile & 0x40)>>3;
  result |= (quadtile & 0x100)>>4;
  result |= (quadtile & 0x400)>>5;
  result |= (quadtile & 0x1000)>>6;
  result |= (quadtile & 0x4000)>>7;
  result |= (quadtile & 0x10000)>>8;
  result |= (quadtile & 0x40000)>>9;
  result |= (quadtile & 0x100000)>>10;
  result |= (quadtile & 0x400000)>>11;
  result |= (quadtile & 0x1000000)>>12;
  result |= (quadtile & 0x4000000)>>13;
  result |= (quadtile & 0x10000000)>>14;
  result |= (quadtile & 0x40000000)>>15;
  
  return result;
}

inline uint32 calc_index(const vector< uint32 >& node_idxs)
{
  if (node_idxs.empty())
    return 0;
  
  // Calculate the bounding box of the appearing indices.
  
  vector< uint32 >::const_iterator it = node_idxs.begin();
  uint32 lat_min = *it & 0x2aaaaaaa;
  uint32 lat_max = lat_min;
  uint32 lon_min = *it & 0x55555555;
  uint32 lon_max = lon_min;
  
  if (*it & 0x80000000)
  {
    if (*it & 0x00000001)
    {
      lat_min = *it & 0x2aaaaaa8;
      lon_min = *it & 0x55555554;
      lat_max = ll_upper((upper_ilat(lat_min) + 3)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 3)<<16);
    }
    else if (*it & 0x00000002)
    {
      lat_min = *it & 0x2aaaaa80;
      lon_min = *it & 0x55555540;
      lat_max = ll_upper((upper_ilat(lat_min) + 0xf)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 0xf)<<16);
    }
    else if (*it & 0x00000004)
    {
      lat_min = *it & 0x2aaaa800;
      lon_min = *it & 0x55555400;
      lat_max = ll_upper((upper_ilat(lat_min) + 0x3f)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 0x3f)<<16);
    }
    else if (*it & 0x00000008)
    {
      lat_min = *it & 0x2aaa8000;
      lon_min = *it & 0x55554000;
      lat_max = ll_upper((upper_ilat(lat_min) + 0xff)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 0xff)<<16);
    }
    else if (*it & 0x00000010)
    {
      lat_min = *it & 0x2aa80000;
      lon_min = *it & 0x55540000;
      lat_max = ll_upper((upper_ilat(lat_min) + 0x3ff)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 0x3ff)<<16);
    }
    else if (*it & 0x00000020)
    {
      lat_min = *it & 0x2a800000;
      lon_min = *it & 0x55400000;
      lat_max = ll_upper((upper_ilat(lat_min) + 0xfff)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 0xfff)<<16);
    }
    else if (*it & 0x00000040)
    {
      lat_min = *it & 0x28000000;
      lon_min = *it & 0x54000000;
      lat_max = ll_upper((upper_ilat(lat_min) + 0x3fff)<<16, 0);
      lon_max = ll_upper(0, (upper_ilon(lon_min) + 0x3fff)<<16);
    }
    else // *it == 0x80000080
      return 0x80000080;
  }
  
  for (++it; it != node_idxs.end(); ++it)
  {
    if (*it & 0x80000000)
    {
      uint32 lat = 0;
      uint32 lon = 0;      
      uint32 lat_u = 0;
      uint32 lon_u = 0;
      
      if (*it & 0x00000001)
      {
	lat = *it & 0x2aaaaaa8;
	lon = *it & 0x55555554;
	lat_u = ll_upper((upper_ilat(lat) + 3)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 3)<<16);
      }
      else if (*it & 0x00000002)
      {
	lat = *it & 0x2aaaaa80;
	lon = *it & 0x55555540;
	lat_u = ll_upper((upper_ilat(lat) + 0xf)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 0xf)<<16);
      }
      else if (*it & 0x00000004)
      {
	lat = *it & 0x2aaaa800;
	lon = *it & 0x55555400;
	lat_u = ll_upper((upper_ilat(lat) + 0x3f)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 0x3f)<<16);
      }
      else if (*it & 0x00000008)
      {
	lat = *it & 0x2aaa8000;
	lon = *it & 0x55554000;
	lat_u = ll_upper((upper_ilat(lat) + 0xff)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 0xff)<<16);
      }
      else if (*it & 0x00000010)
      {
	lat = *it & 0x2aa80000;
	lon = *it & 0x55540000;
	lat_u = ll_upper((upper_ilat(lat) + 0x3ff)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 0x3ff)<<16);
      }
      else if (*it & 0x00000020)
      {
	lat = *it & 0x2a800000;
	lon = *it & 0x55400000;
	lat_u = ll_upper((upper_ilat(lat) + 0xfff)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 0xfff)<<16);
      }
      else if (*it & 0x00000040)
      {
	lat = *it & 0x28000000;
	lon = *it & 0x54000000;
	lat_u = ll_upper((upper_ilat(lat) + 0x3fff)<<16, 0);
	lon_u = ll_upper(0, (upper_ilon(lon) + 0x3fff)<<16);
      }
      else // *it == 0x80000080
	return 0x80000080;
      
      if (lat < lat_min)
	lat_min = lat;
      if (lat_u > lat_max)
	lat_max = lat_u;
      if (lon < lon_min)
	lon_min = lon;
      if (lon_u > lon_max)
	lon_max = lon_u;
    }
    else
    {
      uint32 lat = *it & 0x2aaaaaaa;
      uint32 lon = *it & 0x55555555;
      
      if (lat < lat_min)
        lat_min = lat;
      else if (lat > lat_max)
	lat_max = lat;
      if (lon < lon_min)
	lon_min = lon;
      else if (lon > lon_max)
	lon_max = lon;
    }
  }
  

  // Evaluate the bounding box.

  if ((lat_max == lat_min) && (lon_max == lon_min))
    return node_idxs.front();
  
  uint32 ilat_min = upper_ilat(lat_min);
  uint32 ilat_max = upper_ilat(lat_max);
  uint32 ilon_min = upper_ilon(lon_min);
  uint32 ilon_max = upper_ilon(lon_max);
  
  if (((ilat_max & 0xfffe) - (ilat_min & 0xfffe) < 4) &&
      ((ilon_max & 0xfffe) - (ilon_min & 0xfffe) < 4))
    return (((lon_min | lat_min) & 0xfffffffc) | 0x80000001);
  
  if (((ilat_max & 0xfff8) - (ilat_min & 0xfff8) < 0x10) &&
      ((ilon_max & 0xfff8) - (ilon_min & 0xfff8) < 0x10))
    return (((lon_min | lat_min) & 0xffffffc0) | 0x80000002);
  
  if (((ilat_max & 0xffe0) - (ilat_min & 0xffe0) < 0x40) &&
      ((ilon_max & 0xffe0) - (ilon_min & 0xffe0) < 0x40))
    return (((lon_min | lat_min) & 0xfffffc00) | 0x80000004);
  
  if (((ilat_max & 0xff80) - (ilat_min & 0xff80) < 0x100) &&
      ((ilon_max & 0xff80) - (ilon_min & 0xff80) < 0x100))
    return (((lon_min | lat_min) & 0xffffc000) | 0x80000008);
  
  if (((ilat_max & 0xfe00) - (ilat_min & 0xfe00) < 0x400) &&
      ((ilon_max & 0xfe00) - (ilon_min & 0xfe00) < 0x400))
    return (((lon_min | lat_min) & 0xfffc0000) | 0x80000010);
  
  if (((ilat_max & 0xf800) - (ilat_min & 0xf800) < 0x1000) &&
      ((ilon_max & 0xf800) - (ilon_min & 0xf800) < 0x1000))
    return (((lon_min | lat_min) & 0xffc00000) | 0x80000020);
  
  if (((ilat_max & 0xe000) - (ilat_min & 0xe000) < 0x4000) &&
      ((ilon_max & 0xe000) - (ilon_min & 0xe000) < 0x4000))
    return (((lon_min | lat_min) & 0xfc000000) | 0x80000040);

  return 0x80000080;
}

inline vector< Uint32_Index > calc_node_children(const vector< uint32 >& way_rel_idxs)
{
  vector< Uint32_Index > result;

  vector< pair< uint32, uint32 > > ranges;
  
  for (vector< uint32 >::const_iterator it = way_rel_idxs.begin();
      it != way_rel_idxs.end(); ++it)
  {
    if (*it & 0x80000000)
    {
      uint32 lat = 0;
      uint32 lon = 0;      
      uint32 lat_u = 0;
      uint32 lon_u = 0;
      uint32 offset = 0;
      
      if (*it & 0x00000001)
      {
	lat = upper_ilat(*it & 0x2aaaaaa8);
	lon = upper_ilon(*it & 0x55555554);
	offset = 2;
      }
      else if (*it & 0x00000002)
      {
	lat = upper_ilat(*it & 0x2aaaaa80);
	lon = upper_ilon(*it & 0x55555540);
	offset = 8;
      }
      else if (*it & 0x00000004)
      {
	lat = upper_ilat(*it & 0x2aaaa800);
	lon = upper_ilon(*it & 0x55555400);
	offset = 0x20;
      }
      else if (*it & 0x00000008)
      {
	lat = upper_ilat(*it & 0x2aaa8000);
	lon = upper_ilon(*it & 0x55554000);
	offset = 0x80;
      }
      else if (*it & 0x00000010)
      {
	lat = upper_ilat(*it & 0x2aa80000);
	lon = upper_ilon(*it & 0x55540000);
	offset = 0x200;
      }
      else if (*it & 0x00000020)
      {
	lat = upper_ilat(*it & 0x2a800000);
	lon = upper_ilon(*it & 0x55400000);
	offset = 0x800;
      }
      else if (*it & 0x00000040)
      {
	lat = upper_ilat(*it & 0x28000000);
	lon = upper_ilon(*it & 0x54000000);
	offset = 0x2000;
      }
      else // *it == 0x80000080
      {
	lat = 0;
	lon = 0;
	offset = 0x8000;
      }

      ranges.push_back(make_pair(ll_upper(lat<<16, lon<<16),
				 ll_upper((lat+offset-1)<<16, (lon+offset-1)<<16)+1));
      ranges.push_back(make_pair(ll_upper(lat<<16, (lon+offset)<<16),
				 ll_upper((lat+offset-1)<<16, (lon+2*offset-1)<<16)+1));
      ranges.push_back(make_pair(ll_upper((lat+offset)<<16, lon<<16),
				 ll_upper((lat+2*offset-1)<<16, (lon+offset-1)<<16)+1));
      ranges.push_back(make_pair(ll_upper((lat+offset)<<16, (lon+offset)<<16),
				 ll_upper((lat+2*offset-1)<<16, (lon+2*offset-1)<<16)+1));
      for (uint32 i = lat; i <= lat_u; ++i)
      {
	for (uint32 j = lon; j <= lon_u; ++j)
	  result.push_back(ll_upper(i<<16, j<<16));
      }
    }
    else
      ranges.push_back(make_pair(*it, (*it) + 1));
  }

  sort(ranges.begin(), ranges.end());
  uint32 pos = 0;
  for (vector< pair< uint32, uint32 > >::const_iterator it = ranges.begin();
      it != ranges.end(); ++it)
  {
    if (pos < it->first)
      pos = it->first;
    for (; pos < it->second; ++pos)
      result.push_back(pos);
  }
  return result;
}

inline vector< Uint31_Index > calc_children(const vector< uint32 >& way_rel_idxs)
{
  vector< Uint31_Index > result;
  
  for (vector< uint32 >::const_iterator it = way_rel_idxs.begin();
      it != way_rel_idxs.end(); ++it)
  {
    if (*it & 0x80000000)
    {
      int32 lat = 0;
      int32 lon = 0;      
      int32 lat_u = 0;
      int32 lon_u = 0;
      
      if (*it & 0x00000001)
      {
	lat = upper_ilat(*it & 0x2aaaaaa8);
	lon = upper_ilon(*it & 0x55555554);
	lat_u = lat + 3;
	lon_u = lon + 3;
      }
      else if (*it & 0x00000002)
      {
	lat = upper_ilat(*it & 0x2aaaaa80);
	lon = upper_ilon(*it & 0x55555540);
	lat_u = lat + 0xf;
	lon_u = lon + 0xf;
      }
      else if (*it & 0x00000004)
      {
	lat = upper_ilat(*it & 0x2aaaa800);
	lon = upper_ilon(*it & 0x55555400);
	lat_u = lat + 0x3f;
	lon_u = lon + 0x3f;
      }
      else if (*it & 0x00000008)
      {
	lat = upper_ilat(*it & 0x2aaa8000);
	lon = upper_ilon(*it & 0x55554000);
	lat_u = lat + 0xff;
	lon_u = lon + 0xff;
      }
      else if (*it & 0x00000010)
      {
	lat = upper_ilat(*it & 0x2aa80000);
	lon = upper_ilon(*it & 0x55540000);
	lat_u = lat + 0x3ff;
	lon_u = lon + 0x3ff;
      }
      else if (*it & 0x00000020)
      {
	lat = upper_ilat(*it & 0x2a800000);
	lon = upper_ilon(*it & 0x55400000);
	lat_u = lat + 0xfff;
	lon_u = lon + 0xfff;
      }
      else if (*it & 0x00000040)
      {
	lat = upper_ilat(*it & 0x28000000);
	lon = upper_ilon(*it & 0x54000000);
	lat_u = lat + 0x3fff;
	lon_u = lon + 0x3fff;
      }
      else // *it == 0x80000080
      {
	lat = 0;
	lon = 0;
	lat_u = 0x7fff;
	lon_u = 0xffff;
	result.push_back(0x80000080);
      }

      for (int32 i = lat; i <= (lat_u - 0x3fff); i += 0x2000)
      {
	for (int32 j = lon; j <= (lon_u - 0x3fff); j += 0x2000)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xfc000000) | 0x80000040);
      }
      for (int32 i = lat; i <= (lat_u - 0xfff); i += 0x800)
      {
	for (int32 j = lon; j <= (lon_u - 0xfff); j += 0x800)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xffc00000) | 0x80000020);
      }
      for (int32 i = lat; i <= (lat_u - 0x3ff); i += 0x200)
      {
	for (int32 j = lon; j <= (lon_u - 0x3ff); j += 0x200)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xfffc0000) | 0x80000010);
      }
      for (int32 i = lat; i <= (lat_u - 0xff); i += 0x80)
      {
	for (int32 j = lon; j <= (lon_u - 0xff); j += 0x80)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xffffc000) | 0x80000008);
      }
      for (int32 i = lat; i <= (lat_u - 0x3f); i += 0x20)
      {
	for (int32 j = lon; j <= (lon_u - 0x3f); j += 0x20)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xfffffc00) | 0x80000004);
      }
      for (int32 i = lat; i <= (lat_u - 0xf); i += 8)
      {
	for (int32 j = lon; j <= (lon_u - 0xf); j += 8)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xffffffc0) | 0x80000002);
      }
      for (int32 i = lat; i <= (lat_u - 3); i += 2)
      {
	for (int32 j = lon; j <= (lon_u - 3); j += 2)
	  result.push_back((ll_upper(i<<16, j<<16) & 0xfffffffc) | 0x80000001);
      }
      for (int32 i = lat; i <= lat_u; ++i)
      {
	for (int32 j = lon; j <= lon_u; ++j)
	  result.push_back(ll_upper(i<<16, j<<16));
      }
    }
    else
      result.push_back(*it);
  }
  
  sort(result.begin(), result.end());
  result.erase(unique(result.begin(), result.end()), result.end());
  return result;
}

inline vector< uint32 > calc_parents(const vector< uint32 >& node_idxs)
{
  vector< uint32 > result;
  result.push_back(0x80000080);
  
  for (vector< uint32 >::const_iterator it = node_idxs.begin();
  it != node_idxs.end(); ++it)
  {
    result.push_back(*it);
    
    uint32 lat = upper_ilat(*it & 0x2aaaaaa8) & 0xfffe;
    uint32 lon = upper_ilon(*it & 0x55555554) & 0xfffe;
    result.push_back(ll_upper((lat - 2)<<16, (lon - 2)<<16) | 0x80000001);
    result.push_back(ll_upper(lat<<16, (lon - 2)<<16) | 0x80000001);
    result.push_back(ll_upper((lat - 2)<<16, lon<<16) | 0x80000001);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000001);
    
    lat = lat & 0xfff8;
    lon = lon & 0xfff8;
    result.push_back(ll_upper((lat - 8)<<16, (lon - 8)<<16) | 0x80000002);
    result.push_back(ll_upper(lat<<16, (lon - 8)<<16) | 0x80000002);
    result.push_back(ll_upper((lat - 8)<<16, lon<<16) | 0x80000002);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000002);
    
    lat = lat & 0xffe0;
    lon = lon & 0xffe0;
    result.push_back(ll_upper((lat - 0x20)<<16, (lon - 0x20)<<16) | 0x80000004);
    result.push_back(ll_upper(lat<<16, (lon - 0x20)<<16) | 0x80000004);
    result.push_back(ll_upper((lat - 0x20)<<16, lon<<16) | 0x80000004);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000004);
    
    lat = lat & 0xff80;
    lon = lon & 0xff80;
    result.push_back(ll_upper((lat - 0x80)<<16, (lon - 0x80)<<16) | 0x80000008);
    result.push_back(ll_upper(lat<<16, (lon - 0x80)<<16) | 0x80000008);
    result.push_back(ll_upper((lat - 0x80)<<16, lon<<16) | 0x80000008);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000008);
    
    lat = lat & 0xfe00;
    lon = lon & 0xfe00;
    result.push_back(ll_upper((lat - 0x200)<<16, (lon - 0x200)<<16) | 0x80000010);
    result.push_back(ll_upper(lat<<16, (lon - 0x200)<<16) | 0x80000010);
    result.push_back(ll_upper((lat - 0x200)<<16, lon<<16) | 0x80000010);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000010);
    
    lat = lat & 0xf800;
    lon = lon & 0xf800;
    result.push_back(ll_upper((lat - 0x800)<<16, (lon - 0x800)<<16) | 0x80000020);
    result.push_back(ll_upper(lat<<16, (lon - 0x800)<<16) | 0x80000020);
    result.push_back(ll_upper((lat - 0x800)<<16, lon<<16) | 0x80000020);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000020);
    
    lat = lat & 0xe000;
    lon = lon & 0xe000;
    result.push_back(ll_upper((lat - 0x2000)<<16, (lon - 0x2000)<<16) | 0x80000040);
    result.push_back(ll_upper(lat<<16, (lon - 0x2000)<<16) | 0x80000040);
    result.push_back(ll_upper((lat - 0x2000)<<16, lon<<16) | 0x80000040);
    result.push_back(ll_upper(lat<<16, lon<<16) | 0x80000040);
  }
  
  sort(result.begin(), result.end(), Uint31_Compare());
  result.erase(unique(result.begin(), result.end()), result.end());
  
  return result;
}

inline pair< Uint31_Index, Uint31_Index > make_interval(uint32 idx)
{
  return make_pair(Uint31_Index(idx), Uint31_Index((idx + 1) & 0x7fffffff)); 
}

inline void blur_index(uint32 distance, uint32 bitmask, uint32 lower_idx, uint32 upper_idx,
		       vector< pair< Uint31_Index, Uint31_Index > >& result)
{
  uint32 source_bitmask = 0x10000 - distance;
  uint32 min_lat = upper_ilat(lower_idx) & source_bitmask;
  uint32 min_lon = upper_ilon(lower_idx) & source_bitmask;
  uint32 max_lat = upper_ilat(upper_idx) & source_bitmask;
  uint32 max_lon = upper_ilon(upper_idx) & source_bitmask;
  for (uint32 lat = min_lat - distance; lat <= max_lat; lat += distance)
    result.push_back(make_interval(ll_upper(lat<<16, (min_lon - distance)<<16) | bitmask));
  for (uint32 lon = min_lon; lon <= max_lon; lon += distance)
    result.push_back(make_interval(ll_upper((min_lat - distance)<<16, lon<<16) | bitmask));
  result.push_back(make_interval(ll_upper(max_lat<<16, max_lon<<16) | bitmask));
}

inline void add_decomp_range(const pair< Uint32_Index, Uint32_Index >& range,
			     vector< pair< Uint32_Index, Uint32_Index > >& node_decomp)
{
  uint32 lower = range.first.val();
  while (lower < range.second.val())
  {
    uint32 upper = 0;
    if ((lower & 0x3) != 0 || lower + 4 > range.second.val())
      upper =  lower + 1;
    else if ((lower & 0xf) != 0 || lower + 0x10 > range.second.val())
      upper =  lower + 4;
    else if ((lower & 0x3f) != 0 || lower + 0x40 > range.second.val())
      upper =  lower + 0x10;
    else if ((lower & 0xff) != 0 || lower + 0x100 > range.second.val())
      upper =  lower + 0x40;
    else if ((lower & 0x3ff) != 0 || lower + 0x400 > range.second.val())
      upper =  lower + 0x100;
    else if ((lower & 0xfff) != 0 || lower + 0x1000 > range.second.val())
      upper =  lower + 0x400;
    else if ((lower & 0x3fff) != 0 || lower + 0x4000 > range.second.val())
      upper =  lower + 0x1000;
    else if ((lower & 0xffff) != 0 || lower + 0x10000 > range.second.val())
      upper =  lower + 0x4000;
    else if ((lower & 0x3ffff) != 0 || lower + 0x40000 > range.second.val())
      upper =  lower + 0x10000;
    else if ((lower & 0xfffff) != 0 || lower + 0x100000 > range.second.val())
      upper =  lower + 0x40000;
    else if ((lower & 0x3fffff) != 0 || lower + 0x400000 > range.second.val())
      upper =  lower + 0x100000;
    else if ((lower & 0xffffff) != 0 || lower + 0x1000000 > range.second.val())
      upper =  lower + 0x400000;
    else if ((lower & 0x3ffffff) != 0 || lower + 0x4000000 > range.second.val())
      upper =  lower + 0x1000000;
    else if ((lower & 0xfffffff) != 0 || lower + 0x10000000 > range.second.val())
      upper =  lower + 0x4000000;
    else if ((lower & 0x3fffffff) != 0 || lower + 0x40000000 > range.second.val())
      upper =  lower + 0x10000000;
    else
      upper =  lower + 0x40000000;
    node_decomp.push_back(make_pair(lower, upper));
    lower = upper;
  }
}

inline set< pair< Uint31_Index, Uint31_Index > > calc_parents
    (const set< pair< Uint32_Index, Uint32_Index > >& node_idxs)
{
  vector< pair< Uint32_Index, Uint32_Index > > node_decomp;
  for (set< pair< Uint32_Index, Uint32_Index > >::const_iterator
      it = node_idxs.begin(); it != node_idxs.end(); ++it)
    add_decomp_range(*it, node_decomp);
  
  vector< pair< Uint31_Index, Uint31_Index > > result;
  result.push_back(make_pair(0x80000080, 0x80000081));
  
  for (vector< pair< Uint32_Index, Uint32_Index > >::const_iterator
      it = node_decomp.begin(); it != node_decomp.end(); ++it)
  {
    result.push_back(make_pair(it->first.val(), it->second.val()));
    
    uint32 lower_idx = it->first.val() & 0x7ffffffc;
    uint32 upper_idx = (it->second.val() - 1) & 0x7ffffffc;
    blur_index(2, 0x80000001, lower_idx, upper_idx, result);

    lower_idx = it->first.val() & 0x7fffffc0;
    upper_idx = (it->second.val() - 1) & 0x7fffffc0;
    blur_index(8, 0x80000002, lower_idx, upper_idx, result);
    
    lower_idx = it->first.val() & 0x7ffffc00;
    upper_idx = (it->second.val() - 1) & 0x7ffffc00;
    blur_index(0x20, 0x80000004, lower_idx, upper_idx, result);
    
    lower_idx = it->first.val() & 0x7fffc000;
    upper_idx = (it->second.val() - 1) & 0x7fffc000;
    blur_index(0x80, 0x80000008, lower_idx, upper_idx, result);
    
    lower_idx = it->first.val() & 0x7ffc0000;
    upper_idx = (it->second.val() - 1) & 0x7ffc0000;
    blur_index(0x200, 0x80000010, lower_idx, upper_idx, result);
    
    lower_idx = it->first.val() & 0x7fc00000;
    upper_idx = (it->second.val() - 1) & 0x7fc00000;
    blur_index(0x800, 0x80000020, lower_idx, upper_idx, result);
    
    lower_idx = it->first.val() & 0x7c000000;
    upper_idx = (it->second.val() - 1) & 0x7c000000;
    blur_index(0x2000, 0x80000040, lower_idx, upper_idx, result);
  }

  sort(result.begin(), result.end());
  
  set< pair< Uint31_Index, Uint31_Index > > result_set;
  Uint31_Index last_first = result[0].first;
  Uint31_Index last_second = result[0].second;
  for (vector< pair< Uint31_Index, Uint31_Index > >::size_type i = 1;
      i < result.size(); ++i)
  {
    if (last_second < result[i].first)
    {
      result_set.insert(make_pair(last_first, last_second));
      last_first = result[i].first;
    }
    if (last_second < result[i].second)
      last_second = result[i].second;
  }
  result_set.insert(make_pair(last_first, last_second));
    
  return result_set;
}

#endif
