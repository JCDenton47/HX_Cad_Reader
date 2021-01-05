#include "PathGen.h"


int is_vertical(point4w *input)
{
    if(input->sx == input->ex)
        return true;
    else
        return false;
    return false;
}



void fill_bp_tape( back_plate* const bp_ptr, const Dwg_Entity_LINE** input, const unsigned int len, const char dir)
{
    int i = 0, j = 1, sublen = 0;

    dwg_point_3d dir_vec;//方向向量
    Dwg_Entity_LINE* line_SS = (input[i]) ;//line_sub_start
    Dwg_Entity_LINE* line_SE = (input[j]);//line_sub_end
    //找子组结束点
    while(j < len)
    {
        line_SS = (input[i]) ;//line_sub_start
        line_SE = (input[j]);//line_sub_end

        //如果方向为垂直，
        if(dir == DIR_V)
        {
            double result = fabs(line_SS->start.x - line_SE->start.x);
            if( result > 0.001)
            {//起点x不相等，说明已经定位到
                sublen = j - i;
                if(sublen == 1)
                {
                    //判断是否是无主孤魂
                    if(fabs(line_SS->start.y - line_SE->start.y) > 0.001)
                    {
                        //交换两个指针,调整substart
//                        input[j] = input[i];
                        i = j++;
                        continue;
                    }
                }

                for(int k = 0; k < sublen; ++k)
                {
                    Dwg_Entity_LINE* left = (input[i + k]);
                    Dwg_Entity_LINE* right = (input[j + k]);


                    tape* VT_pt = bp_ptr->vertical_tape + bp_ptr->tape_VT_index;

                    //算中点做喷胶路径
                    VT_pt->path_origin = midpoint( &(left->start), &(right->start) );
                    VT_pt->path_end    = midpoint( &(left->end), &(right->end) );

                    // //预防两条线之间的长度不一样
                    // VT_pt->path_origin.y = maxf(left->start.y, right->start.y);
                    // VT_pt->path_end.y    = minf(left->end.y, right->end.y);
                    //计算两条直线之间的距离
                    dir_vec.x = (VT_pt->path_end.x) - (VT_pt->path_origin.x);
                    dir_vec.y = (VT_pt->path_end.y) - (VT_pt->path_origin.y);
                    VT_pt->dir = dir_vec;

                    int g = (bp_ptr->tape_VT_index)++;

                    double sx = (bp_ptr->vertical_tape[g]).path_origin.x;
                    double sy = (bp_ptr->vertical_tape[g]).path_origin.y;
                    double ex = (bp_ptr->vertical_tape[g]).path_end.x;
                    double ey = (bp_ptr->vertical_tape[g]).path_end.y;
                    printf(" V(%.3f, %.3f), (%.3f, %.3f) \n", sx, sy, ex, ey);

                }
                i = j + sublen;
                //定位到下一个双组
                j = i + 1;
            }
            else
            //起点相等，再探
            {
                ++j;
            }


        }
        else// dir == DIR_H 水平线储存
        {
            /* code */
            double result = fabs(line_SS->start.y - line_SE->start.y);
            if( result > 0.001)
            //起点x不相等
            {
                sublen = j - i;

                int k = 0;
                for(; k < sublen; ++k)
                {
                    Dwg_Entity_LINE* up = (input[i + k]);
                    Dwg_Entity_LINE* down = (input[j + k]);

                    tape* HZ_pt = bp_ptr->horizontal_tape + bp_ptr->tape_HZ_index;
                    HZ_pt->path_origin = midpoint( &(up->start), &(down->start) );
                    HZ_pt->path_end    = midpoint( &(up->end), &(down->end) );

                    //预防两条直线不一致
                    HZ_pt->path_origin.x = minf(up->start.x, down->start.x);
                    HZ_pt->path_end.x    = maxf(up->end.x, down->end.x);

                    dir_vec.x = (HZ_pt->path_end.x) - (HZ_pt->path_origin.x);
                    dir_vec.y = (HZ_pt->path_end.y) - (HZ_pt->path_origin.y);
                    HZ_pt->dir = dir_vec;

                    int g = (bp_ptr->tape_HZ_index)++;

                    double sx = (bp_ptr->horizontal_tape[g]).path_origin.x;
                    double sy = (bp_ptr->horizontal_tape[g]).path_origin.y;
                    double ex = (bp_ptr->horizontal_tape[g]).path_end.x;
                    double ey = (bp_ptr->horizontal_tape[g]).path_end.y;
                    printf(" H(%.3f, %.3f), (%.3f, %.3f) \n", sx, sy, ex, ey);


                }
                i = j + sublen;
                //定位到下一个双组
                j = i + 1;
            }
            else
            //起点相等，再探
            {
                ++j;
            }
        }
    }





}

void bubble_sort_RL( back_plate* const bp_ptr,  Dwg_Entity_LINE** const input, const unsigned int len, const char dir, const char sub_flag)
{
        int i, j;
        dwg_object* tmp;
        static char sub_sort_needed = 1;

        if(len == 0)
            return;

        for(i = 0; i < len - 1 ; ++i)
        {
            for(j = 0; j < len - 1 ; ++j)
            {
                if( DIR_V == dir)//如果是垂直的线，先排序x
                {
                    double x_1 = input[j]->start.x;
                    double x_2 = input[j + 1]->start.x;
                    if(x_1 > x_2)
                    {
                        tmp = input[j];
                        input[j] = input[j + 1];
                        input[j + 1] = tmp;
                    }

                }
                else if( DIR_H == dir )//如果是水平的线，先排序y
                {
                    double y_1 = (input[j])->start.y;
                    double y_2 = (input[j + 1])->start.y;
                    if(y_1 > y_2)
                    {
                        tmp = input[j];
                        input[j] = input[j + 1];
                        input[j + 1] = tmp;
                    }
                }
                else
                {
                    printf("fuck\n");
                }

            }
        }

        double buf[len];

        //使用一个浮点数数组存储所有的坐标,以实现一种stable sort,保持上面排序后的坐标不变，
        //只排序以一号坐标一致的次坐标组

        for(i = 0 ; i < len ; ++i)
        {
            if( DIR_V == dir )
            {
                buf[i] = (input[i])->start.x;
            }
            else if( DIR_H == dir )
            {
                buf[i] = (input[i])->start.y;
            }
        }
        //如果需要二号坐标排序
        if(sub_flag)
        {
            i = j = 0;


            while(j < len + 1)
            {
                // 当right_val不同，进行反应
                if( fabs(buf[j] - buf[i]) > 0.001)
                {
                    //进行相反方向的调用
                    bubble_sort_RL(bp_ptr, input + i, j - i, -dir, 0);

                    const double sublen = j - i;
                    if(sublen > 1)
                    {
                        i = j;
                        ++j;
                    }
                    else
                    {
                        //如果长度只是一
                        i = ++j;
                        j = i + 1;
                    }



                }
                else
                {
                    ++j;
                }

            }
            //未进行过排序
            if(i == 0)
            {
                bubble_sort_RL(bp_ptr, input + i, j - i, -dir, 0);
            }
            printf("test sort\n");
            for(i = 0; i < len ; ++i)
            {
                double sx = (input[i])->start.x;
                double sy = (input[i])->start.y;
                double ex = (input[i])->end.x;
                double ey = (input[i])->end.y;
                printf(" (%.3f, %.3f), (%.3f, %.3f) \n", sx, sy, ex, ey);
            }
            printf("dir: %s\n\n\n", dir > 0 ? "DIR_V" : "DIR_H");
            //排序完毕，进行结构体填充
            fill_bp_tape(bp_ptr, input, len, dir);
        }

}

void output_TAPE(back_plate* bp_ptr)
{
    //存入所有胶带之前，不要进行排序
    //先冒泡排竖红线
    bubble_sort_RL(bp_ptr, bp_ptr->VT_red_line, bp_ptr->RL_VT_index, DIR_V, 1);

    if(bp_ptr->RL_HZ_index)
        //再冒泡排序横红线
        bubble_sort_RL(bp_ptr, bp_ptr->HZ_red_line, bp_ptr->RL_HZ_index, DIR_H, 1);

}


dwg_point_3d* fill_vertical_group(point4w* path, tape* tape_pt, int sublen, bool Need_Reverse)
{
    int i = 0;
    int j = 0;
    //如果不需要反向
    if(!Need_Reverse)
    {
        for( i = 0 ; i > -sublen ; --i)
        {
            path[j].ex = ((tape_pt + i)->path_origin.x) * 10;
            path[j].ey = ((tape_pt + i)->path_origin.y) * 10;
            path[j].sx = ((tape_pt + i)->path_end.x) * 10;
            path[j].sy = ((tape_pt + i)->path_end.y) * 10;
            ++j;
        }
        return &((tape_pt + i)->path_end);
    }

    //需要反向
    else
    {
        /* code */
        for( i = sublen - 1 ; i >= 0 ; --i)
        {
            path[j].sx = ((tape_pt - i)->path_origin.x) * 10;
            path[j].sy = ((tape_pt - i)->path_origin.y) * 10;
            path[j].ex = ((tape_pt - i)->path_end.x) * 10;
            path[j].ey = ((tape_pt - i)->path_end.y) * 10;

            ++j;
           //翻转顺序和起点，终点
        }
        return &((tape_pt + i)->path_end);
    }
}
//路径规划，实际路径生成
int fill_actual_path(point4w* path, back_plate* bp_pt)
{
    //横线组中，基本都是根据y分好组的，以y进行排序，次组中，x在逐次递增，所以一开始，先输出所有的底部横线组
    int P_i = -1, H_i = 0;
    tape* tape_pt = bp_pt->horizontal_tape;
    //如果存在横线？
    if(bp_pt->tape_HZ_index)
    {
        double c_y, n_y;
        //如果不止,尝试吃完一组同y的线
        do
        {
            ++P_i;
            path[P_i].sx = ((tape_pt + P_i)->path_origin.x) * 10;
            path[P_i].sy = ((tape_pt + P_i)->path_origin.y) * 10;
            path[P_i].ex = ((tape_pt + P_i)->path_end.x) * 10;
            path[P_i].ey = ((tape_pt + P_i)->path_end.y) * 10;
            if( (P_i + 1) == (bp_pt->tape_HZ_index) )
            {//以防整幅图纸只有一根横线
                ++P_i;
                break;
            }
            c_y = (tape_pt + P_i)->path_origin.y;
            n_y = (tape_pt + P_i + 1)->path_origin.y;

         } while (fabs(c_y - n_y) < 0.001);
        //补录最后一组线
        ++P_i;
        path[P_i].sx = ((tape_pt + P_i)->path_origin.x) * 10;
        path[P_i].sy = ((tape_pt + P_i)->path_origin.y) * 10;
        path[P_i].ex = ((tape_pt + P_i)->path_end.x) * 10;
        path[P_i].ey = ((tape_pt + P_i)->path_end.y) * 10;
    }
    //考虑如果不存在横线以及只有底部一条横线的情况（P_i == 0 or P_i == -1)
    if(P_i <= 0)
    {
        H_i = ++P_i;
        //存上横线的y坐标
    }
    else
        H_i = P_i;

    //开始不停的存竖线

    int V_i = bp_pt->tape_VT_index - 1;
    tape_pt = bp_pt->vertical_tape;

    dwg_point_3d* c_end = NULL, *n_start = NULL, *n_end = NULL;

    //如果成功存上了横直线组
    if(P_i > 1)
    {
        c_end   = &((bp_pt->horizontal_tape + P_i - 1)->path_end);//存入横线中最后一条，第一条竖线是跟最后一条横线比较的
        n_start = &((tape_pt + V_i)->path_origin);
        n_end   = &((tape_pt + V_i)->path_end);
    }
    //不成功的话，存上第一条竖线的终点，强行翻转直线
    else
    {
        c_end   = &((bp_pt->vertical_tape + V_i)->path_end);//存入横线中的终点，第一条竖线是跟最后一条横线比较的
        n_start = &((tape_pt + V_i)->path_origin);
        n_end   = &((tape_pt + V_i)->path_end);
    }

    //如果不成功，即没有存上任何横线


    bool Need_Reverse = false;
    while( V_i >= 0 )
    {

        size_t sublen = 1;

        //从竖线组尾部开始,我面对的，是一组同x的直线吗？
        //计算sublen
        double c_x, n_x;
        c_x = (tape_pt + V_i)->path_origin.x;
        n_x = (tape_pt + V_i - sublen)->path_origin.x;
        while( (V_i - sublen >= 0) && ( fabs( c_x - n_x ) < 0.001 ))
        {
            c_x = (tape_pt + V_i)->path_origin.x;
            n_x = (tape_pt + V_i - sublen)->path_origin.x;
            ++sublen;
        }

        if(sublen != 1)
            --sublen;

        if(n_start != NULL)
        {
            //我跟起点的位置近一点，还是终点的位置近一点？
//            double dis_end_start = getdis_4(c_end->x, c_end->y, n_start->x, n_start->y);
//            double dis_end_end   = getdis_4(c_end->x, c_end->y, n_end->x, n_end->y);
            //跟终点更近就翻转
            if(Need_Reverse == false)
            {
                Need_Reverse = true;
            }
            else
            {
                Need_Reverse = false;
            }

        }
        //根据是否需要reverse选择deque是正向输出还是反向输出，反向顺序，反向start 和 end
        c_end = fill_vertical_group(path + P_i, tape_pt + V_i, (int)sublen, Need_Reverse);
        P_i += sublen;
        V_i -= sublen;

        if(V_i < 0)
        {
            //说明已经读完所有横线组，跳出
            break;
        }
        n_start = &((tape_pt + V_i)->path_origin);
        n_end   = &((tape_pt + V_i)->path_end);

    }


    //存最后一条的横线
    tape_pt = bp_pt->horizontal_tape;
    //如果不存在上横线, 就不存
    if(H_i < bp_pt->tape_HZ_index)
    {
        //如果横线只有一条
        //如果不止,尝试吃完一组同y的线
        do
        {

            path[P_i].sx = ((tape_pt + H_i)->path_origin.x) * 10;
            path[P_i].sy = ((tape_pt + H_i)->path_origin.y) * 10;
            path[P_i].ex = ((tape_pt + H_i)->path_end.x) * 10;
            path[P_i].ey = ((tape_pt + H_i)->path_end.y) * 10;
            ++P_i;
            if( (H_i + 1) == (bp_pt->tape_HZ_index) )
            {
                break;
            }
            ++H_i;
        } while (fabs((tape_pt + P_i)->path_origin.y - (tape_pt + P_i + 1)->path_origin.y) < 0.001);

    }
    return P_i;

}

void fill_empty_path(point4w* epath, int len, point4w* apath)
{



    for(int i = 0 ; i < len - 1 ; ++i)
    {
        //判断第一根线的终点跟第二根线的起点距离更近还是终点更近
        epath[i].sx = apath[i].ex;
        epath[i].sy = apath[i].ey;
        epath[i].ex = apath[i + 1].sx;
        epath[i].ey = apath[i + 1].sy;

    }
    epath[len - 1].sx = apath[len - 1].ex;
    epath[len - 1].sy = apath[len - 1].ey;
    epath[len - 1].ex = apath[0].sx;
    epath[len - 1].ey = apath[0].sy;

}

// void Get_minmax_xy(actual_path *apath, point4w* tmp)
// {
//     int min_x, min_y, max_x, max_y;

//     min_x =tmp[0].sx;
//     min_y =tmp[0].sy;
//     max_x =tmp[0].sx;
//     max_y =tmp[0].sy;


//     for(int i = 1 ; i < apath->len ; i++)
//     {
//         int t_x = tmp[i].sx;
//         int t_y = tmp[i].sy;

//         if(t_x < min_x)
//             min_x = t_x;

//         if(t_x < min_y)
//             min_y = t_x;

//         if(t_x > max_x)
//             max_x = t_x;

//         if(t_y > max_y)
//             max_y = t_y;
//     }

//     apath->rsx = min_x;
//     apath->rsy = min_y;
//     apath->length = max_x - min_x;
//     apath->width  = max_y - min_y;

// }


void Generate_path(back_plate* const bp_ptr)
{


    actual_path Apath;

    memset(&Apath, 0, sizeof(Apath));

    point4w ac_path[50];
    point4w empty_path[50];
    //先读实际路径

    //取横第一段

    int i = 0, j = 1;
    tape* HT_ptr = bp_ptr->horizontal_tape;
    tape* VT_ptr = bp_ptr->vertical_tape;


    Apath.len = 0;
    Apath.len += fill_actual_path(ac_path, bp_ptr);

    printf("total actual path %d\n", Apath.len);

    for(int i = 0 ; i < Apath.len ; ++i)
    {
        printf("sx: %d, sy: %d, ex: %d, ey: %d\n", ac_path[i].sx, ac_path[i].sy, ac_path[i].ex, ac_path[i].ey);
    }
    printf("\n\n\n");

    fill_empty_path(empty_path, Apath.len, ac_path);
    for(int i = 0 ; i < Apath.len ; ++i)
    {
        printf("sx: %d, sy: %d, ex: %d, ey: %d\n", empty_path[i].sx, empty_path[i].sy, empty_path[i].ex, empty_path[i].ey);
    }
    printf("\n\n\n");

    FILE* fpTXT;
    fpTXT = fopen("path.txt", "w+");
    if(fpTXT == NULL)
    {
        printf("error writing data to txt\n");
    }
    //暂时的策略，
    // Get_minmax_xy(&Apath, ac_path);

    //修改此处位置为以左上角为原点

    Apath.rsx = bp_ptr->minWidth * 10;
    // Apath.rsy = bp_ptr->minHeight * 10;
    Apath.rsy    = bp_ptr->minHeight * 10;
    Apath.height = (bp_ptr->maxHeight - bp_ptr->minHeight) * 10;
    Apath.width  = (bp_ptr->maxWidth  - bp_ptr->minWidth) * 10;

    const double fileRsx = bp_ptr->minWidth * 10;
    const double fileRsy = bp_ptr->maxHeight * 10;

    j = 0;

    //i == 0
    //第一个空行程不在数组里，所以我自己打出来
    //初始空行
    fprintf(fpTXT, "%d %d %d\n",
                ac_path[0].sx - fileRsx,
                abs(ac_path[0].sy - fileRsy),
                is_vertical(ac_path) ? (T_VERTICAL) : (T_HORIZONTAL)
                );//不喷,根据后面有功路径决定是否喷垂直还是水平
    for(int i = 0; i < Apath.len; ++i)
    {
        //结构体保存raw图纸数据
        Apath.real_path[j++] = ac_path[i];
        Apath.real_path[j++] = empty_path[i];
        //文件输出处理后的，以左下角为原点的图纸数据
        point4w tmp_ap = {
                            ac_path[i].sx - fileRsx,
                            abs(ac_path[i].sy - fileRsy),
                            ac_path[i].ex - fileRsx,
                            abs(ac_path[i].ey - fileRsy),
                            };
        point4w tmp_ep = {
                            empty_path[i].sx - fileRsx,
                            abs(empty_path[i].sy - fileRsy),
                            empty_path[i].ex - fileRsx,
                            abs(empty_path[i].ey - fileRsy),
                            };

        // fprintf(fpTXT, "%d %d %d %d 1\n", tmp_ap.sx, tmp_ap.sy, tmp_ap.ex, tmp_ap.ey);
        // fprintf(fpTXT, "%d %d %d %d 0\n", tmp_ep.sx, tmp_ep.sy, tmp_ep.ex, tmp_ep.ey);


        //实行
        fprintf(fpTXT, "%d %d %d\n", tmp_ap.ex, tmp_ap.ey, T_SPRAY);
         //空行
        fprintf(fpTXT, "%d %d %d\n", tmp_ep.ex, tmp_ep.ey, is_vertical(ac_path + i + 1) ? (T_VERTICAL) : (T_HORIZONTAL));//不喷,根据后面有功路径决定是否喷垂直还是水平
    }
    fclose(fpTXT);


    Apath.len *= 2;

    FILE* fp;
    fp = fopen("path.cof", "wb+");//cad_out_file , 实际就是要写一个结构体进去
    if(fp == NULL)
    {
        printf("error open path.cof\n");
    }
    fwrite((&Apath), sizeof(Apath), 1, fp);

    fclose(fp);

    // fp = fopen("path.cof", "r+") why write a binary file without b option...
    //尝试读取path结构体
    fp = fopen("path.cof", "rb+");
    actual_path a2;
    fread(&a2, sizeof(a2), 1, fp);
    fclose(fp);

    for(int i = 0; i < Apath.len; ++i)
    {
        printf("%s: sx :%d, sy :%d, ex :%d, ey :%d\n",(i % 2) ? ("empty"):("real"), a2.real_path[i].sx, a2.real_path[i].sy, a2.real_path[i].ex, a2.real_path[i].ey);
    }
    printf("width %d, height %d\n", Apath.width, Apath.height);

}
