#ifndef BACK_PLATE_H
#define BACK_PLATE_H

#include "libinclude/dwg.h"
#include "libinclude/dwg_api.h"
#include "simplemath.h"
#include "string.h"

#define COLOR_INVISIBLE 0
#define COLOR_RED 1
#define COLOR_YELLOW 2
#define COLOR_GREEN 3
#define COLOR_WHITE 7

#define M_PI 3.1415926535

#define DIR_V 1
#define DIR_H -1


#define log_if_error(msg)                                                     \
  if (error)                                                                  \
    {                                                                         \
      fprintf (stderr, "ERROR: %s", msg);                                     \
      exit (1);                                                               \
    }
#define log_error(msg)                                                        \
  {                                                                           \
    fprintf (stderr, "ERROR: %s", msg);                                       \
    exit (1);                                                                 \
  }
#define dynget(obj, name, field, var)                                         \
  if (!dwg_dynapi_entity_value (obj, "" name, "" field, var, NULL))           \
    {                                                                         \
      fprintf (stderr, "ERROR: %s.%s", name, field);                          \
      exit (1);                                                               \
    }


typedef struct _tape
{
    dwg_point_3d dir;
    dwg_point_3d mid_point;
    dwg_point_3d path_origin;
    dwg_point_3d path_end;
}tape;

typedef struct _back_plate
{
    double RecLside; //矩形长边
    double RecSside; //矩形短边
    double minHeight;//最小高度
    double maxHeight;//最大高度
    double minWidth; //最小宽度
    double maxWidth; //最大宽度
    BITCODE_2DPOINT origin; // 图纸原点

    tape         vertical_tape[40]; //胶带组
    tape         horizontal_tape[40];
    unsigned int tape_VT_index;
    unsigned int tape_HZ_index;

    Dwg_Entity_LINE*  VT_red_line[40];
    Dwg_Entity_LINE*  HZ_red_line[40];
    unsigned int RL_VT_index;
    unsigned int RL_HZ_index;

    char* output_path;

}back_plate;


typedef struct _point
{
  int sx;
  int sy;
  int ex;
  int ey;
}point4w;

typedef struct _actual_path
{
  point4w real_path[50];
  unsigned int len;
  unsigned int width;
  unsigned int height;
  int rsx; // relative start.x
  int rsy;  // relative start,y
}actual_path;



//将所有向量归一化为从左到右，从上到下的向量
double line_normalized(Dwg_Entity_LINE* line);

//输出块头
static void output_BLOCK_HEADER (dwg_data* dwg, dwg_object_ref *ref, back_plate* bp_ptr);

//将color_code翻译为实际color
int print_color(const int color);


//带记录的直线输出函数，将所获取的直线放置到自己的结构体中
static void output_LINE_Rec (Dwg_Entity_LINE *line, back_plate* bp_ptr);

//不带记录的纯输出直线，功能几乎相同，只是没多态只能这样写
static void output_LINE(dwg_object *obj);

//输出实心胶带， 慎用， 因为会出现十分奇异的问题，坐标识别不精确
static void output_HATCH(dwg_object *obj, back_plate* bp_ptr);

//输出插入点，插入点代表块参照，可以通过里面的属性找到原来的块进行偏移后输出
static void output_INSERT (const dwg_data* dwg, Dwg_Object *obj, back_plate* bp_ptr);

//根据obj类型进行输出，其实就输出块参照和直线信息
static void output_object (const dwg_data* dwg, dwg_object *obj, back_plate* bp_ptr);


//输出BLOCK_HEADER, 好像该需求已经废置
static void output_BLOCK_HEADER (dwg_data* dwg, dwg_object_ref *ref, back_plate* bp_ptr);





//释放堆上的内存
static void free_back_plate(back_plate* bp_ptr);

//输出dwg
static void output_dwg(dwg_data *dwg, back_plate* bp_ptr);


//扫描dwg, 作为最外侧的接口
int scan_dwg(const char* filename, back_plate* bp_ptr);



#endif // BACK_PLATE_H





//好像这个函数已经废置使用
// static void output_Brec(dwg_object_ref *ref)
// {
//     dwg_object *hdr, *obj;
//     dwg_obj_block_header *_hdr;
//     int error;
//     BITCODE_RL abs_ref;
//     char *name;

//     if (!ref)
//         {
//         fprintf (stderr,
//                 "Empty BLOCK."
//                 " Could not output an SVG symbol for this BLOCK_HEADER\n");
//         return;
//         }
//     hdr = dwg_ref_get_object (ref, &error);//从Obj参照中获取obj
//     if (!hdr || error)
//         return;
//     abs_ref = dwg_ref_get_absref (ref, &error);//从Obj参照中获取绝对地址

//     _hdr = dwg_object_to_BLOCK_HEADER (hdr);//转换obj为block_header_obj
//     if (_hdr)//输出BH自身的信息
//         {
//         dynget (_hdr, "BLOCK_HEADER", "name", &name);
//         // name = dwg_obj_block_header_get_name (_hdr, &error);
//         printf ("\tid=\"symbol-%X\"\nname:%s\n", abs_ref ? abs_ref : 0,
//                 name ? name : "");
//         if (name != NULL && name != _hdr->name
//             && hdr->parent->header.version >= R_2007)
//             free (name);
//         }
//     else
//         printf ("\tid=\"symbol-%X\" name:unknown\n", abs_ref ? abs_ref : 0);
//         //获取block中的元素
//     obj = get_first_owned_entity (hdr);
//     while (obj)
//     {
//         if (dwg_object_get_type (obj) == DWG_TYPE_LINE)
//         {

//             output_LINE(obj);

//         }
//         obj = get_next_owned_entity (hdr, obj);
//     }
// }
