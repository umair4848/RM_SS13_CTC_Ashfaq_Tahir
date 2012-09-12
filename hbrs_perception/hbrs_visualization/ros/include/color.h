#ifndef COLOR_H
#define COLOR_H

#include <pcl/point_types.h>
#include <std_msgs/ColorRGBA.h>

namespace hbrs
{

namespace visualization
{

struct Color
{

  uint8_t r;
  uint8_t g;
  uint8_t b;

  Color(float nr, float ng, float nb) : r(nr * 255), g(ng * 255), b(nb * 255) { }

  Color(uint8_t nr, uint8_t ng, uint8_t nb) : r(nr), g(ng), b(nb) { }

  enum Name
  {
    SALMON,
    TEAL,
    DEEP_PINK,
  };

  Color(Name name)
  {
    switch (name)
    {
      case SALMON:        r = 0xFA, g = 0x80, b = 0x72; break;
      case TEAL:          r = 0x00, g = 0x80, b = 0x80; break;
      case DEEP_PINK:     r = 0xFF, g = 0x14, b = 0x93; break;
      default:            r = 0xFF, g = 0xFF, b = 0xFF; break;
    }
  }

  operator float()
  {
    pcl::PointXYZRGB point(r, g, b);
    return point.rgb;
  }

  operator std_msgs::ColorRGBA()
  {
    std_msgs::ColorRGBA color;
    color.r = r / 255.0;
    color.g = g / 255.0;
    color.b = b / 255.0;
    color.a = 1.0;
    return color;
  }

};

}

}

#endif /* COLOR_H */

