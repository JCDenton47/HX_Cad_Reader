#include "back_plate.h"
#include "simplemath.h"
#include "math.h"
#include "Windows.h"
#include "PathGen.h"
//int scan_dwg(const char* filename, back_plate* bp_ptr);

double line_normalized(Dwg_Entity_LINE* line)
{
    if((line->start.x - line->end.x) > 0.001)//如果起点在终点右边， 调转
    {
        printf("start.x:%.3f\n", line->start.x);
        double tmp = line->start.x;
        line->start.x = line->end.x;
        line->end.x = tmp;
        tmp = line->start.y;
        line->start.y = line->end.y;
        line->end.y = tmp;
        printf("start.x:%.3f\n", line->start.x);
    }
    if( (line->start.y - line->end.y) > 0.001)//如果终点在起点上方， 调转
    {
        printf("start.y:%.3f\n", line->start.y);
        double tmp = line->start.x;
        line->start.x = line->end.x;
        line->end.x = tmp;
        tmp = line->start.y;
        line->start.y = line->end.y;
        line->end.y = tmp;
        printf("start.y:%.3f\n", line->start.y);
    }
}
int print_color(const int color)
{
    switch(color)
    {
    case COLOR_RED:
        printf("\tline color  : red   ");
        break;
    case COLOR_GREEN:
//        printf("\tline color : green ");
        return 1;
        break;
    case COLOR_WHITE:
        printf("\tline color  : white ");
        break;
    case COLOR_YELLOW:
        return 1;
//        printf("\tline color : yellow ");
        break;
    default:
        printf("unknown color : %04d", color);
        break;
    }
    return 0;

}

static void output_LINE_Rec (Dwg_Entity_LINE *line, back_plate* bp_ptr)
{
    int error, index;
    static char flag_FF = 0;
    dwg_point_3d start, end;




//    if (!line)
//    log_error ("dwg_object_to_LINE");

    int color = line->parent->color.raw;



    start.x = line->start.x; start.y = line->start.y;
    end.x = line->end.x; end.y = line->end.y;

    double sy = start.y;
    double ey = end.y;
    double sx = start.x;
    double ex = end.x;


    double dis = getdis(line);

    switch(color)
    {
    case COLOR_RED:
    //如果当前红线太短，复位红线


        if(dis < 20)
        {
            flag_FF = 0;
            break;
        }
        printf("\tline color  : red   ");
        printf (" \n\"start.x: %.3f, start.y: %.3f  "
            " end.x: %.3f, end.y: %.3f\""
            "index : %d\n",
                (sx), (sy),
            (ex),  (ey));

        //存打竖的红线
        Dwg_Entity_LINE** cur_RL;//当前红线的二级指针

        if(fabs(sx - ex) < 0.001)//竖线
        {
            printf("vertical\n");
            line_normalized(line);
            cur_RL = &(bp_ptr->VT_red_line[bp_ptr->RL_VT_index]);//当前竖直红线位置

            Dwg_Entity_LINE* pt = (Dwg_Entity_LINE*)malloc(sizeof(Dwg_Entity_LINE));//分一个空间出来
            if(pt == NULL)
            {
                printf("no mem!\n");
                return;
            }
            pt->start = line->start;
            pt->end   = line->end;
            pt->parent = line->parent;

            *cur_RL = pt;
            ++(bp_ptr->RL_VT_index);
        }
        else if(fabs(sy - ey) < 0.001)//横线
        {
            printf("horizontal\n");
            line_normalized(line);
            cur_RL = &(bp_ptr->HZ_red_line[bp_ptr->RL_HZ_index]);//当前打横红线位置

            Dwg_Entity_LINE* pt = (Dwg_Entity_LINE*)malloc(sizeof(Dwg_Entity_LINE));
            if(pt == NULL)
            {
                printf("no mem!\n");
                return;
            }
            pt->start = line->start;
            pt->end   = line->end;
            pt->parent = line->parent;

            *cur_RL = pt;
            ++(bp_ptr->RL_HZ_index);
        }

        break;
    case COLOR_GREEN:
        break;
    case COLOR_WHITE:

       if(line->parent->ownerhandle != NULL)
           break;

    //    printf("\tline color  : white ");
       double x_min, x_max, y_min, y_max;
       x_min = minf(start.x, end.x);
       x_max = maxf(start.x, end.x);
       y_min = minf(start.y, end.y);
       y_max = maxf(start.y, end.y);

       if(x_min < bp_ptr->minWidth || bp_ptr->minWidth == 0)
           bp_ptr->minWidth = x_min;
       if(x_max > bp_ptr->maxWidth || bp_ptr->maxWidth == 0)
           bp_ptr->maxWidth = x_max;
       if(y_min < bp_ptr->minHeight|| bp_ptr->minHeight == 0)
           bp_ptr->minHeight = y_min;
       if(y_max > bp_ptr->maxHeight|| bp_ptr->maxHeight == 0)
           bp_ptr->maxHeight = y_max;

        break;
    case COLOR_YELLOW:
        break;
    default:
        printf("unknown color : %04d", color);
        break;
    }
}
static void output_LINE(dwg_object *obj)
{
    int error, index;
    Dwg_Entity_LINE *line;
    dwg_point_3d start, end;

    index = dwg_object_get_index (obj, &error);
    log_if_error ("object_get_index");
    line = dwg_object_to_LINE (obj);
    if (!line)
        log_error ("dwg_object_to_LINE");

//    int color = line->parent->color.raw;
//    int ret_flag = print_color(color);


    if (!dwg_get_LINE (line, "start", &start))
        log_error ("LINE.start");
    if (!dwg_get_LINE (line, "end", &end))
        log_error ("LINE.end");



    printf (" d=\"start.x: %.3f, start.y: %.3f  "
            " end.x: %.3f, end.y: %.3f\""
            "index : %d\n",
                (start.x), (start.y),
            (end.x),  (end.y), index);

}
static void output_HATCH(dwg_object *obj, back_plate* bp_ptr)
{
    int i = 0, total_line;
    int error, index;


    Dwg_Entity_HATCH *hatch;
    hatch = dwg_object_to_HATCH(obj);
    index = dwg_object_get_index (obj, &error);

    dwg_object* line_obj;

    total_line = hatch->num_boundary_handles;

    printf("\nhatch here!\n");

    for( ; i < total_line ; ++i)
    {

        line_obj = (*(hatch->boundary_handles + i) )->obj;

        if(dwg_object_get_type(line_obj) != DWG_TYPE_LINE)
        {
            printf("error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            return;
        }
        if(line_obj->tio.entity->color.raw != COLOR_RED)
        {
            printf("hatch color dismatch!");
            return;
        }
        output_LINE_Rec(line_obj, bp_ptr);
    }
    printf("hatch->num_boundary_handles:%d \n\n", hatch->num_boundary_handles);

}

static void output_INSERT (const dwg_data* dwg, Dwg_Object *obj, back_plate* bp_ptr)
{
    int index, error;
    BITCODE_RL abs_ref;
    double rotation;
    dwg_ent_insert *insert;
    dwg_point_3d ins_pt, _scale;
    dwg_handle *obj_handle, *ins_handle;

    insert = dwg_object_to_INSERT (obj);//转为entity
    if (!insert)
    log_error ("dwg_object_to_INSERT");
    index = dwg_object_get_index (obj, &error);//获取当前插入点在整个大modelspace中的index
    log_if_error ("object_get_index");
    dynget (insert, "INSERT", "rotation", &rotation);//获取插入中的旋转参数
    dynget (insert, "INSERT", "ins_pt", &ins_pt);//获取插入中的插入点
    dynget (insert, "INSERT", "scale", &_scale);//获取插入中的放缩比例，实际块与参照的放缩比例
    obj_handle = dwg_object_get_handle (obj, &error);
    log_if_error ("get_handle");
    if (!insert->block_header)
    log_error ("insert->block_header");
    abs_ref = insert->block_header->absolute_ref;//获取insert_ent的绝对参照地址，即块本身位置

    if (insert->block_header->handleref.code == 5)//为什么是5？，源程序就是5，5说明读对了
    {
        printf ("\tobj-insert: %d \"translate(%lf %lf) "
                "rotate(%f) scale(%f %f)\" xlink:href=\"symbol-%X\" "
                "block_header->handleref: " FORMAT_H "\n",
                index,  (ins_pt.x),  (ins_pt.y),
                (180.0 / M_PI) * rotation, _scale.x, _scale.y, abs_ref,
                ARGS_H (*obj_handle));
        Dwg_Object* hdr = dwg_absref_get_object(dwg, abs_ref);//精髓所在，根据abs_ref,获取块本身位置的obj地址

        if(dwg_object_get_type(hdr) == DWG_TYPE_BLOCK_HEADER)//判断是否为Block HEADER
        {
            printf("block_header found\n");

            Dwg_Object* ref_obj;
            Dwg_Entity_LINE* ref_line;
            ref_obj = get_first_owned_entity (hdr);//读里面的参数
            while (ref_obj)
            {
                ref_line = dwg_object_to_LINE(ref_obj);

                if (dwg_object_get_type (ref_obj) == DWG_TYPE_LINE)
                {
                    Dwg_Object tmp_obj;
                    Dwg_Entity_LINE tmp_line;

                    // tmp_obj = *ref_obj;//这句话不能用, 一用会导致块本源信息丢失，暂时不知道原因

                    tmp_line.start.x = ins_pt.x + _scale.x * ref_line->start.x ;
                    tmp_line.end.x   = ins_pt.x + _scale.x * ref_line->end.x;

                    tmp_line.start.y = ins_pt.y + _scale.y * ref_line->start.y;
                    tmp_line.end.y   = ins_pt.y + _scale.y * ref_line->end.y;

                    tmp_line.parent = ref_line->parent;

                    // tmp_obj.tio.entity->tio.LINE = &tmp_line;//这句话不能用

                    if(tmp_line.parent->color.raw == COLOR_GREEN)
                       break;

                    // printf("line: s(%.3f, %.3f), e(%.3f, %.3f)\n",
                    //        tmp_line.start.x, tmp_line.start.y, tmp_line.end.x, tmp_line.end.y);

                   output_LINE_Rec(&tmp_line, bp_ptr);

                }
                ref_obj = get_next_owned_entity (hdr, ref_obj);

            }
        }

    }
    else
    {
        printf ("\n\n<!-- WRONG INSERT(" FORMAT_H ") -->\n",
                ARGS_H (*obj_handle));
    }
}


static void output_object (const dwg_data* dwg, dwg_object *obj, back_plate* bp_ptr)
{
  if (!obj)
    {
      fprintf (stderr, "object is NULL\n");
      return;
    }

  if (dwg_object_get_type (obj) == DWG_TYPE_INSERT)
    {
      output_INSERT (dwg, obj, bp_ptr);
    }

  if (dwg_object_get_type (obj) == DWG_TYPE_LINE)
    {
        Dwg_Entity_LINE* line = dwg_object_to_LINE(obj);
        output_LINE_Rec(line, bp_ptr);

    }

}

static void output_BLOCK_HEADER (dwg_data* dwg, dwg_object_ref *ref, back_plate* bp_ptr)
{
  dwg_object *hdr, *obj;
  dwg_obj_block_header *_hdr;
  int error;
  BITCODE_RL abs_ref;
  char *name;

  if (!ref)
    {
      fprintf (stderr,
               "Empty BLOCK."
               " Could not output an SVG symbol for this BLOCK_HEADER\n");
      return;
    }
  hdr = dwg_ref_get_object (ref, &error);//从Obj参照中获取obj
  if (!hdr || error)
    return;
  abs_ref = dwg_ref_get_absref (ref, &error);//从Obj参照中获取绝对地址

  _hdr = dwg_object_to_BLOCK_HEADER (hdr);//转换obj为block_header_obj
  if (_hdr)//输出BH自身的信息
    {
      dynget (_hdr, "BLOCK_HEADER", "name", &name);
      // name = dwg_obj_block_header_get_name (_hdr, &error);
      printf ("\tid=\"symbol-%X\"\nname:%s\n", abs_ref ? abs_ref : 0,
              name ? name : "");
      if (name != NULL && name != _hdr->name
          && hdr->parent->header.version >= R_2007)
        free (name);
    }
  else
    printf ("\tid=\"symbol-%X\" name:unknown\n", abs_ref ? abs_ref : 0);
    //获取block中的元素
  obj = get_first_owned_entity (hdr);
  while (obj)
    {
      output_object (dwg, obj, bp_ptr);
      obj = get_next_owned_entity (hdr, obj);
    }

}


//////////////////////////////////胶带组////////////////////////////////////////////////////


static void free_back_plate(back_plate* bp_ptr)
{
    for(int i = 0 ; i < bp_ptr->tape_VT_index ; ++i)
    {
        if(bp_ptr->VT_red_line[i] != NULL)
            free(bp_ptr->VT_red_line[i]);
    }
    for(int i = 0; i < bp_ptr->tape_HZ_index; ++i)
    {
        if(bp_ptr->HZ_red_line[i] != NULL)
            free(bp_ptr->HZ_red_line[i]);
    }
}

static void output_dwg(dwg_data *dwg, back_plate* bp_ptr)
{
    unsigned int i, num_hdr_objs, j = 0;
    int error;
    dwg_obj_block_control *_ctrl;
    dwg_object_ref **hdr_refs;

    _ctrl = dwg_block_control (dwg);
    hdr_refs = dwg_obj_block_control_get_block_headers (_ctrl, &error);
    log_if_error ("block_control_get_block_headers");
    num_hdr_objs = dwg_obj_block_control_get_num_entries (_ctrl, &error);
    log_if_error ("block_control_get_num_entries");

    printf("paper: x_min: %.3f,  y_max: %.3f,  "
            "y_min: %.3f, y_max: %.3f\n"
            , dwg_model_x_min(dwg), dwg_model_x_max(dwg)
            , dwg_model_y_min(dwg), dwg_model_y_max(dwg) );


    printf("all the dwg_model_space elements are listed as follow\n");
    output_BLOCK_HEADER (dwg, dwg_model_space_ref (dwg), bp_ptr);



    bp_ptr->origin.x = bp_ptr->minWidth;
	bp_ptr->origin.y = bp_ptr->maxHeight;
    bp_ptr->RecLside = fabs(bp_ptr->maxWidth - bp_ptr->minWidth);
    bp_ptr->RecSside = fabs(bp_ptr->maxHeight - bp_ptr->minHeight);



	printf("final result\norigin :(%.3f, %.3f), L_side %.3f, S_side length %.3f\n",
           bp_ptr->origin.x, bp_ptr->origin.y , bp_ptr->RecLside, bp_ptr->RecSside);
    printf("model space output complete!\n");




    output_TAPE(bp_ptr);

    Generate_path(bp_ptr);

    free_back_plate(bp_ptr);






}


int scan_dwg(const char* filename, back_plate* bp_ptr)
{
    BITCODE_BL i;
	int fail = 0;
	Dwg_Data dwg;


	memset(&dwg, 0, sizeof(Dwg_Data));
	dwg.opts = 0;
	fail = dwg_read_file(filename, &dwg);
    if(fail < DWG_ERR_CRITICAL)
        output_dwg(&dwg, bp_ptr);

    dwg_free(&dwg);
	return fail;


}
