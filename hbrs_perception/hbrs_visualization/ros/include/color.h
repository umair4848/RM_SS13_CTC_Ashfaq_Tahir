#ifndef COLOR_H
#define COLOR_H

namespace hbrs
{

namespace visualization
{

struct Color
{

  Color(float nr, float ng, float nb) : r(nr), g(ng), b(nb) { }

  float r;
  float g;
  float b;

  enum Name
  {
    SALMON,
  };

  Color(Name name)
  {
    switch (name)
    {
      case SALMON: r = 0.98, g = 0.50, b = 0.45; break;
    }
  }

};

}

}

#endif /* COLOR_H */

