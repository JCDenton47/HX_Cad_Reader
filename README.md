# HX_Cad_Reader
A cad reader using libredwg

#前言
这是个简单的，利用libredwg进行cad图纸解析的例子程序，参考例程为库自带的dwg2svg.c
原库连接如下
#### https://github.com/LibreDWG/libredwg


# 运行指南
请下载codeblock并配置mingw64位编译器
然后打开.cbp即可编译该工程，建议使用VScode对该工程进行代码查看

# 注意事项
1. 由于源库编译失败，所以我将所有的源码都丢进去算了，以后估计会编译成一整个动态库，方便日后使用
2. 请勿使用MSDN（即vs自带的编译器)尝试运行，vs自带编译器不支持代码里面的C99标准.
