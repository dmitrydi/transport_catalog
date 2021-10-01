#pragma once

#include <cmath>
#include <limits>
#include <iostream>
#include "utils.h"

namespace Sphere {
  double ConvertDegreesToRadians(double degrees);

  struct Point {
    double latitude;
    double longitude;
    bool flag = false;

    static Point FromDegrees(double latitude, double longitude);

    bool operator==(const Point& other) const;
  };

  double Distance(Point lhs, Point rhs);

  std::ostream& operator<<(std::ostream& os, const Sphere::Point& point);
}



