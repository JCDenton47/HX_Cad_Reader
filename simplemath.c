#include "simplemath.h"

double absf(double input)
{
    if(input < 0)
        return -input;
    else
        return input;
}
double minf(double a, double b)
{
    if(a > b)
        return b;
    else
        return a;
}
double maxf(double a, double b)
{
    if(a > b)
        return a;
    else
        return b;
}


int mini(int a, int b)
{
    if(a > b)
        return b;
    else
        return a;
}
int maxi(int a, int b)
{
    if(a > b)
        return a;
    else
        return b;
}


int is_parallel(dwg_point_3d* dir1, dwg_point_3d* dir2)
{
    printf("%.3f, %.3f, %.3f, %.3f\n", dir1->x, dir1->y, dir2->x, dir2->y);
    if((dir1->x * dir2->y) - (dir1->y * dir2->x) < 0.001)
        return 1;
    return 0;
}
dwg_point_3d midpoint(dwg_point_3d* p1, dwg_point_3d* p2)
{
    dwg_point_3d tmp;
    tmp.x =  0.5 * (p2->x + p1->x);
    tmp.y =  0.5 * (p2->y + p1->y);
    return tmp;
}
double getdis(Dwg_Entity_LINE *line)
{
    int error, index;


    if(line == NULL)
    {
        printf("error: getdis get nullptr\n");
        return -1.0;
    }

    dwg_point_3d start, end;

    start.x = line->start.x;start.y = line->start.y;
    end.x   = line->end.x; end.y = line->end.y;

    return sqrt( (start.x - end.x) * (start.x - end.x) + (start.y - end.y) * (start.y - end.y) );

}
double getdis_4(double sx, double sy, double ex, double ey)
{
    double result = sqrt( (sx - ex) * (sx - ex) + (sy - ey) * (sy - ey) );

    return result;
}
