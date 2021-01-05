#include <stdio.h>
#include <stdlib.h>
#include "libinclude/dwg.h"
#include "math.h"
#include "back_plate.h"

#define COLOR_INVISIBLE 0
#define COLOR_RED 1
#define COLOR_GREEN 3
#define COLOR_WHITE 7



double absf(double input)
{
    if(input < 0)
        return -input;
    else
        return input;

}

void print_acblock(Dwg_Data* input, int p_index)
{
    int i = 0;
    int line_counter = 0;
    Dwg_Entity_LINE *line;
    for (i = p_index; input->object[i].type != DWG_TYPE_ENDBLK; i++)
    {
//        printf("type: %d\n", input->object[i].type);
        switch (input->object[i].type)
		{
        case DWG_TYPE_LINE:
            line = input->object[i].tio.entity->tio.LINE;

            BITCODE_3BD start = line->start;
            BITCODE_BS color_i = input->object[i].tio.entity->color.raw;
            printf("line, %.3f, %.3f, color: %d \n", start.x, start.y, color_i );
            line_counter++;
            break;

		}
		if(line_counter > 10)
            break;

    }
    if(line_counter == 0)
    {
        for (; input->object[i].type != DWG_TYPE_INSERT; i++)
        {
            printf("type: %d\n", input->object[i].type);
            switch (input->object[i].type)
            {
            case DWG_TYPE_LINE:
                line = input->object[i].tio.entity->tio.LINE;

                BITCODE_3BD start = line->start;
                BITCODE_BS color_i = input->object[i].tio.entity->color.raw;
                printf("line, %.3f, %.3f, color: %d \n", start.x, start.y, color_i );
                line_counter++;
                break;

            }

        }


    }


}


int main(char** argv, int argc)
{
	BITCODE_BL i;
	int success;
	Dwg_Data dwg;

	const char* filename = "Drawing3.dwg";

	memset(&dwg, 0, sizeof(Dwg_Data));
	dwg.opts = 0;
	success = dwg_read_file(filename, &dwg);

    back_plate bp1;
    back_plate* bp_ptr = &bp1;

    bp1.RecLside = 0;
    bp1.RecSside = 0;




    BITCODE_2DPOINT L_side_origin = {0, 0};
    BITCODE_2DPOINT L_side_end = {0, 0};

    BITCODE_2DPOINT S_side_origin = {0, 0};
    BITCODE_2DPOINT S_side_end = {0, 0};

    unsigned int block_flag = 0;
    m_layer cur_layer = {0, 0};

    FILE *fp;
    fp = fopen("3_5.txt", "w");

	for (i = 0; i < dwg.num_objects; i++)
	{

	    Dwg_Object_Entity* entity_pt = dwg.object[i].tio.entity;
        Dwg_Object *       object_pt = dwg.object[i].tio.object;

		Dwg_Entity_LINE *line;
		Dwg_Entity_CIRCLE *circle;
		Dwg_Entity_TEXT *text;
		Dwg_Entity_INSERT *insertion;


        fprintf(fp, "type: %d, index: %d\n", dwg.object[i].type, i);


		switch (dwg.object[i].type)
		{
        case DWG_TYPE_BLOCK_HEADER:
            block_flag = 1;
            break;
        case DWG_TYPE_ENDBLK:
            block_flag = 0;
            break;

        case DWG_TYPE_LINE:
//            if(block_flag)
//                continue;

            line = dwg.object[i].tio.entity->tio.LINE;
            BITCODE_3BD s_pt = line->start;
            BITCODE_3BD e_pt = line->end;
            int direction;
            unsigned int color = object_pt->klass;

            double dis = 0;

            if(color != COLOR_WHITE)
                continue;


            if( abs( s_pt.y - e_pt.y ) == 0  ) //横线
            {
                direction = DIR_H;
                dis = absf( s_pt.x - e_pt.x ); //两点垂直距离
                if(dis < 300.0)
                    continue;
//                printf("line %d: start( x: %.3f, y: %.3f), end( x: %.3f, y: %.3f), dis%.3f\n",i ,s_pt.x, s_pt.y, e_pt.x, e_pt.y, dis);
                if(cur_layer.abs_ref != entity_pt->layer->absolute_ref)//层信息发生改变
                {
                    cur_layer.abs_ref = entity_pt->layer->absolute_ref;

                    if(cur_layer.size < 10)//意味着扫到了次级层
                    {

                        //清零存储的线

                        BITCODE_2DPOINT zero = {0, 0};
                        L_side_origin = zero;
                        L_side_end = zero;

                        S_side_origin = zero;
                        S_side_end = zero;

                    }

                    cur_layer.size = 1;
                }
                else
                {
                    ++cur_layer.size;

                }
//                printf("handle: %d\n", entity_pt->layer->absolute_ref);
                if(L_side_origin.y == 0 || L_side_origin.y < s_pt.y)
                {
                    L_side_origin.x = s_pt.x;
                    L_side_origin.y = s_pt.y;
                    L_side_end.x    = e_pt.x;
                    L_side_end.y    = e_pt.y;
                }

            }
            else if( abs( s_pt.x - e_pt.x ) == 0) //竖线
            {
                direction = DIR_V;
                dis = absf( s_pt.y - e_pt.y );
                if(dis < 300.0)
                    continue;
//                printf("line %d: start( x: %.3f, y: %.3f), end( x: %.3f, y: %.3f), dis%.3f\n",i ,s_pt.x, s_pt.y, e_pt.x, e_pt.y, dis);
//                printf("handle: %d\n", entity_pt->layer->absolute_ref);
                if(cur_layer.abs_ref != entity_pt->layer->absolute_ref)//层信息发生改变
                {
                    cur_layer.abs_ref = entity_pt->layer->absolute_ref;

                    if(cur_layer.size < 10)//次级层
                    {
                        //清零存储的线
                        BITCODE_2DPOINT zero = {0, 0};
                        L_side_origin = zero;
                        L_side_end = zero;

                        S_side_origin = zero;
                        S_side_end = zero;

                    }

                    cur_layer.size = 1;
                }
                else
                {
                    ++cur_layer.size;

                }
                if(S_side_origin.x == 0 || S_side_origin.x > s_pt.x)
                {
                    S_side_origin.x = s_pt.x;
                    S_side_origin.y = s_pt.y;
                    S_side_end.x    = e_pt.x;
                    S_side_end.y    = e_pt.y;
                }
            }
            else
                continue;
            break;
//		case DWG_TYPE_CIRCLE:
//			circle = dwg.object[i].tio.entity->tio.CIRCLE;
//			printf("//type:DWG_TYPE_CIRCLE, center (x:%.3f, y:%.3f), radius : %3.f\n", circle->center.x, circle->center.y, circle->radius);
//			//add_circle(circle->center.x, circle->center.y, circle->radius);
//			break;
//		case DWG_TYPE_TEXT:
//			text = dwg.object[i].tio.entity->tio.TEXT;
//			printf("//type:DWG_TYPE_TEXT");
//			/*add_text(text->insertion_pt.x, text->insertion_pt.y,
//				text->text_value);*/
//			break;

        case DWG_TYPE_INSERT:

            insertion = entity_pt->tio.INSERT;

            unsigned int parent_index = insertion->block_header->obj->index;

            BITCODE_BS color_i = dwg.object[parent_index + 2].tio.entity->color.raw;

            if(color_i != (COLOR_RED) && color_i != (COLOR_GREEN) )
                continue;


            print_acblock(&dwg, parent_index);

            BITCODE_3DPOINT insert_point = insertion->ins_pt;
            printf("insertion_x : %.3f, insertion_y: %.3f, parent_index: %ld \n", insert_point.x, insert_point.y,
                   insertion->block_header->obj->index);
//            printf("layer_id %d\n", entity_pt->layer->absolute_ref);
            printf("index: %d\n", i);
            printf("son's color : %d\n", color_i);




            break;



		default:

			break;
		}
	}
	BITCODE_2DPOINT origin = {S_side_origin.x, L_side_origin.y};
	double L_centerx = (L_side_origin.x + L_side_end.x) / 2 - origin.x;
	double S_centery = (S_side_origin.y + S_side_end.y) / 2 - origin.y;
	bp_ptr->origin = origin;
	bp_ptr->RecLside = absf(L_centerx * 2);
	bp_ptr->RecSside = absf(S_centery * 2);


	printf("final result\norigin :(%.3f, %.3f), L_side %.3f, S_side length %.3f\n",
           bp_ptr->origin.x, bp_ptr->origin.y , bp_ptr->RecLside, bp_ptr->RecSside);

    printf("L_side_line, start:(%.3f, %.3f), end(%.3f, %.3f),  dis:%.3f \n",
                    S_side_origin.x, L_side_origin.y, L_side_end.x, L_side_end.y,
                    absf( S_side_origin.x -  L_side_end.x));
    printf("S_side_line, start:(%.3f, %.3f), end(%.3f, %.3f),  dis:%.3f \n",
                    S_side_origin.x, S_side_origin.y, S_side_end.x, S_side_end.y,
                    absf( S_side_origin.y -  S_side_end.y));


	dwg_free(&dwg);

	fclose(fp);

	return success;

}
