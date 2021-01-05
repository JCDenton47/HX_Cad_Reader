#ifndef PATHGEN_H
#define PATHGEN_H

#include "back_plate.h"

#define T_VERTICAL 16
#define T_HORIZONTAL 0
#define T_SPRAY 1


int is_vertical(point4w *input);




//给back_plate结构体，填充横竖直线, 由于排序的原因， x相等的线都会被排在一起在前面，所有找到下一个x不想等的位置
void fill_bp_tape( back_plate* const bp_ptr, const Dwg_Entity_LINE** input, const unsigned int len, const char dir);


//对红线坐标根据方向进行冒泡排序
void bubble_sort_RL( back_plate* const bp_ptr,  Dwg_Entity_LINE** const input, const unsigned int len, const char dir, const char sub_flag);

//排序胶带，并输出胶带信息
void output_TAPE(back_plate* bp_ptr);

//为胶带输出路径
void Generate_path(back_plate* const bp_ptr);



#endif // PATHGEN_H
