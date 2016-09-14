# daily-code
        主要完成C语言的预处理功能
        C语言预处理主要针对如下关键字进行特定的动作
        主要完成对源文件的扩展，#include #if 递归包含

        预处理支持的关键字及功能
        |   command        |    explain                             |
        | ---------------- | -------------------------------------  |
        |   #define        |    宏替换                              |
        |   #error         |    用于调试，输出错误信息              |
        |   #include       |    包含其他文件（递归包含）            |
        |   #if            |    条件编译                            |
        |   #else          |                                        |
        |   #elif          |                                        |
        |   #endif         |                                        |
        |   #ifdef         |    条件编译，能和#else #endif共用      |
        |   #ifndef        |    未定义宏则包含后面的代码            |
        |   #undef         |    取消宏定义                          |
        |   #line          |    改变预先定义的__LINE__, __FILE__    |
        |   #pragma        |    向编译器发送状态，设定编译器        |
        +------------------+----------------------------------------+

        词法分析过程:
            预定义4类字符：
                #define C_WS 1      空格字符，典型的如tab, space
                #define C_ALPH 2    数组字符 a-z,A-Z
                #define C_NUM  3    数字0-9
                #define C_OTHER 4   其他字符
