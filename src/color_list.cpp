#include <nanogui/color_list.h>

NAMESPACE_BEGIN(nanogui)

namespace {
static std::map<unsigned int, const Color*> _colors;

static const unsigned int hash_seed = 1315423911;

Color fromHex(int v) {
    return Color((int)(v>>16)&0xff,(int)(v>>8)&0xff,(int)(v&0xff),(int)(v>>24)&0xff);
}

unsigned int hash( const std::string& text )
{
  unsigned int nHash = 0;
  const char* key = text.c_str();
  if( key )
  {
    nHash = hash_seed;
    while(*key)
    {
      nHash ^= ((nHash << 5) + *key + (nHash >> 2));
      key++;
    }

    nHash &= 0x7FFFFFFF;
  }

  return nHash;
}

}

const Color& ColorList::find(const std::string& name)
{
  if (_colors.empty())
    _createColors();

  auto it = _colors.find(hash(name));
  if( it != _colors.end() )  {
    return *it->second;
  }
  else if (name.size()>2&&name[0]=='0'&&name[1]=='x') {
    int value = std::stoul(name.substr(2),nullptr,16);
    unsigned int nhash = hash(name);
    _colors[nhash] = (new Color(fromHex(value)));
    return *_colors[nhash];
  }

  return ColorList::clear;
}

#define __ASSIGN_COLOR(n,v) const Color ColorList::n = fromHex(v);
#define __ADD_COLOR(a) _colors[ hash(#a) ] = &ColorList::a;

void ColorList::_createColors()
{
__ADD_COLOR( clear           )
__ADD_COLOR( red             )
__ADD_COLOR( caesarRed       )
__ADD_COLOR( blue            )
__ADD_COLOR( green           )
__ADD_COLOR( show            )
__ADD_COLOR( ghost           )
__ADD_COLOR( whitesmoke      )
__ADD_COLOR( floralWhite     )
__ADD_COLOR( oldlace         )
__ADD_COLOR( linen           )
__ADD_COLOR( antiqueWhite    )
__ADD_COLOR( papayaWhip      )
__ADD_COLOR( blanchedAlmond  )
__ADD_COLOR( bisque          )
__ADD_COLOR( peachPuff       )
__ADD_COLOR( navajoWhite     )
__ADD_COLOR( moccasin        )
__ADD_COLOR( cornsilk        )
__ADD_COLOR( ivory           )
__ADD_COLOR( lemonChiffon    )
__ADD_COLOR( seashell        )
__ADD_COLOR( honeydew        )
__ADD_COLOR( mintCream       )
__ADD_COLOR( azure           )
__ADD_COLOR( aliceBlue       )
__ADD_COLOR( lavender        )
__ADD_COLOR( lavenderBlush   )
__ADD_COLOR( mistyRose       )
__ADD_COLOR( white           )
__ADD_COLOR( black           )
__ADD_COLOR( darkSlateGray   )
__ADD_COLOR( dimGrey         )
__ADD_COLOR( slateGrey       )
__ADD_COLOR( lightSlateGray  )
__ADD_COLOR( grey            )
__ADD_COLOR( lightGray       )
__ADD_COLOR( midnightBlue    )
__ADD_COLOR( navyBlue        )
__ADD_COLOR( cornflowerBlue  )
__ADD_COLOR( darkSlateBlue	 )
__ADD_COLOR( slateBlue       )
__ADD_COLOR( mediumSlateBlue )
__ADD_COLOR( lightSlateBlue	 )
__ADD_COLOR( mediumBlue	     )
__ADD_COLOR( royalBlue	     )
__ADD_COLOR( skyBlue         )
__ADD_COLOR( paleTurquoise	 )
__ADD_COLOR( cyan            )
__ADD_COLOR( mediumAquamarine)
__ADD_COLOR( darkGreen       )
__ADD_COLOR( darkSeaGreen    )
__ADD_COLOR( paleGreen	     )
__ADD_COLOR( limeGreen	     )
__ADD_COLOR( darkKhaki	     )
__ADD_COLOR( paleGoldenrod	 )
__ADD_COLOR( yellow          )
__ADD_COLOR( gold            )
__ADD_COLOR( rosyBrown	     )
__ADD_COLOR( indianRed	     )
__ADD_COLOR( sienna          )
__ADD_COLOR( peru            )
__ADD_COLOR( wheat           )
__ADD_COLOR( sandyBrown      )
__ADD_COLOR( tan             )
__ADD_COLOR( chocolate	     )
__ADD_COLOR( firebrick	     )
__ADD_COLOR( brown	         )
__ADD_COLOR( salmon	         )
__ADD_COLOR( orange	         )
__ADD_COLOR( darkOrange      )
__ADD_COLOR( coral	         )
__ADD_COLOR( tomato	         )
__ADD_COLOR( orangeRed	     )
__ADD_COLOR( hotPink	     )
__ADD_COLOR( deepPink	     )
__ADD_COLOR( pink            )
__ADD_COLOR( maroon          )
__ADD_COLOR( magenta         )
__ADD_COLOR( violet	         )
__ADD_COLOR( orchid	         )
__ADD_COLOR( purple	         )
__ADD_COLOR( snow            )
__ADD_COLOR( dodgerBlue      )
__ADD_COLOR( steelBlue       )
__ADD_COLOR( slateGray       )
__ADD_COLOR( aquamarine      )
__ADD_COLOR( seaGreen        )
__ADD_COLOR( springGreen     )
__ADD_COLOR( chartreuse      )
__ADD_COLOR( plum            )
__ADD_COLOR( thistle         )
}

__ASSIGN_COLOR( clear,          0x00000000 )
__ASSIGN_COLOR( red,            0xffff0000 )
__ASSIGN_COLOR( caesarRed,      0xffA00000 )
__ASSIGN_COLOR( blue,           0xff0000ff )
__ASSIGN_COLOR( green,          0xff00ff00 )
__ASSIGN_COLOR( show,           0xfffffafa )
__ASSIGN_COLOR( ghost,          0xffF8F8FF )
__ASSIGN_COLOR( whitesmoke,     0xffF5F5F5 )
__ASSIGN_COLOR( floralWhite,    0xffFFFAF0 )
__ASSIGN_COLOR( oldlace,        0xffFDF5E6 )
__ASSIGN_COLOR( linen,          0xffFAF0E6 )
__ASSIGN_COLOR( antiqueWhite,   0xffFAEBD7 )
__ASSIGN_COLOR( papayaWhip,     0xffFFEFD5 )
__ASSIGN_COLOR( blanchedAlmond, 0xffFFEBCD )
__ASSIGN_COLOR( bisque,         0xffFFE4C4 )
__ASSIGN_COLOR( peachPuff,      0xffFFDAB9 )
__ASSIGN_COLOR( navajoWhite,    0xffFFDEAD )
__ASSIGN_COLOR( moccasin,       0xffFFE4B5 )
__ASSIGN_COLOR( cornsilk,       0xffFFF8DC )
__ASSIGN_COLOR( ivory,          0xffFFFFF0 )
__ASSIGN_COLOR( lemonChiffon,   0xffFFFACD )
__ASSIGN_COLOR( seashell,       0xffFFF5EE )
__ASSIGN_COLOR( honeydew,       0xffF0FFF0 )
__ASSIGN_COLOR( mintCream,      0xffF5FFFA )
__ASSIGN_COLOR( azure,          0xffF0FFFF )
__ASSIGN_COLOR( aliceBlue,      0xffF0F8FF )
__ASSIGN_COLOR( lavender,       0xffE6E6FA )
__ASSIGN_COLOR( lavenderBlush,  0xffFFF0F5 )
__ASSIGN_COLOR( mistyRose,      0xffFFE4E1 )
__ASSIGN_COLOR( white,          0xffFFFFFF )
__ASSIGN_COLOR( black,          0xff000000 )
__ASSIGN_COLOR( darkSlateGray,  0xff2F4F4F )
__ASSIGN_COLOR( dimGrey,        0xff696969 )
__ASSIGN_COLOR( slateGrey,      0xff708090 )
__ASSIGN_COLOR( lightSlateGray,	0xff778899 )
__ASSIGN_COLOR( grey,           0xffBEBEBE )
__ASSIGN_COLOR( lightGray,      0xffD3D3D3 )
__ASSIGN_COLOR( midnightBlue,   0xff191970 )
__ASSIGN_COLOR( navyBlue,       0xff000080 )
__ASSIGN_COLOR( cornflowerBlue,	0xff6495ED )
__ASSIGN_COLOR( darkSlateBlue,	0xff483D8B )
__ASSIGN_COLOR( slateBlue,      0xff6A5ACD )
__ASSIGN_COLOR( mediumSlateBlue,0xff7B68EE )
__ASSIGN_COLOR( lightSlateBlue,	0xff8470FF )
__ASSIGN_COLOR( mediumBlue,	    0xff0000CD )
__ASSIGN_COLOR( royalBlue,	    0xff4169E1 )
__ASSIGN_COLOR( skyBlue,        0xff87CEEB )
__ASSIGN_COLOR( paleTurquoise,	0xffAFEEEE )
__ASSIGN_COLOR( cyan,           0xff00FFFF )
__ASSIGN_COLOR( mediumAquamarine,0xff66CDAA)
__ASSIGN_COLOR( darkGreen,      0xff006400 )
__ASSIGN_COLOR( darkSeaGreen,   0xff8FBC8F )
__ASSIGN_COLOR( paleGreen,	    0xff98FB98 )
__ASSIGN_COLOR( limeGreen,	    0xff32CD32 )
__ASSIGN_COLOR( darkKhaki,	    0xffBDB76B )
__ASSIGN_COLOR( paleGoldenrod,	0xffEEE8AA )
__ASSIGN_COLOR( yellow,         0xffFFFF00 )
__ASSIGN_COLOR( gold,           0xffFFD700 )
__ASSIGN_COLOR( rosyBrown,	    0xffBC8F8F )
__ASSIGN_COLOR( indianRed,	    0xffCD5C5C )
__ASSIGN_COLOR( sienna,         0xffA0522D )
__ASSIGN_COLOR( peru,           0xffCD853F )
__ASSIGN_COLOR( wheat,          0xffF5DEB3 )
__ASSIGN_COLOR( sandyBrown,     0xffF4A460 )
__ASSIGN_COLOR( tan,            0xffD2B48C )
__ASSIGN_COLOR( chocolate,	    0xffD2691E )
__ASSIGN_COLOR( firebrick,	    0xffB22222 )
__ASSIGN_COLOR( brown,	        0xffA52A2A )
__ASSIGN_COLOR( salmon,	        0xffFA8072 )
__ASSIGN_COLOR( orange,	        0xffFFA500 )
__ASSIGN_COLOR( darkOrange,     0xffFF8C00 )
__ASSIGN_COLOR( coral,	        0xffFF7F50 )
__ASSIGN_COLOR( tomato,	        0xffFF6347 )
__ASSIGN_COLOR( orangeRed,	    0xffFF4500 )
__ASSIGN_COLOR( hotPink,	    0xffFF69B4 )
__ASSIGN_COLOR( deepPink,	    0xffFF1493 )
__ASSIGN_COLOR( pink,           0xffFFC0CB )
__ASSIGN_COLOR( maroon,         0xffB03060 )
__ASSIGN_COLOR( magenta,        0xffFF00FF )
__ASSIGN_COLOR( violet,	        0xffEE82EE )
__ASSIGN_COLOR( orchid,	        0xffDA70D6 )
__ASSIGN_COLOR( purple,	        0xffA020F0 )
__ASSIGN_COLOR( snow,           0xffFFFAFA )
__ASSIGN_COLOR( dodgerBlue,     0xff1E90FF )
__ASSIGN_COLOR( steelBlue,      0xff63B8FF )
__ASSIGN_COLOR( slateGray,      0xffC6E2FF )
__ASSIGN_COLOR( aquamarine,     0xff7FFFD4 )
__ASSIGN_COLOR( seaGreen,       0xff54FF9F )
__ASSIGN_COLOR( springGreen,    0xff00FF7F )
__ASSIGN_COLOR( chartreuse,     0xff7FFF00 )
__ASSIGN_COLOR( plum,           0xffFFBBFF )
__ASSIGN_COLOR( thistle,        0xffFFE1FF )

NAMESPACE_END(nanogui)
