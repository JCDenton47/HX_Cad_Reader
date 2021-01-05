#include <stdio.h>
#include <stdlib.h>
#include "libinclude/dwg.h"
#include "math.h"
#include "back_plate.h"








int main(int argc, char** argv)
{
	BITCODE_BL i;
	int success;

	back_plate bp1 = {0, 0, 0, 0, 0, 0};

	back_plate* bp_ptr = &bp1;
	memset(bp_ptr, 0, sizeof(back_plate));

    bp_ptr->output_path = argv[1]; // 保存文件路径

    scan_dwg(argv[1], bp_ptr);
//    scan_dwg("4.dwg", bp_ptr);
	return success;

}
