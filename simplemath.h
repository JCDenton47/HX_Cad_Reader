#ifndef SIMPLEMATH_H
#define SIMPLEMATH_H

#include "libinclude/dwg.h"
#include "libinclude/dwg_api.h"

double absf(double input);

double minf(double a, double b);

double maxf(double a, double b);

int mini(int a, int b);
int maxi(int a, int b);


int is_parallel(dwg_point_3d* dir1, dwg_point_3d* dir2);

dwg_point_3d midpoint(dwg_point_3d* p1, dwg_point_3d* p2);

double getdis(Dwg_Entity_LINE *line);
double getdis_4(double sx, double sy, double ex, double ey);

#endif
