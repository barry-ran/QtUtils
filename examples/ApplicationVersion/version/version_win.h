#ifndef VERSION_H
#define VERSION_H

#define _TOSTR(x)   #x
#define TOSTR(x)  _TOSTR(x)

// 字符串形式版本号，用于版本显示
/* the following are compile time version */
/* C++11 requires a space between literal and identifier */
#define VERSION_STR TOSTR(VERSION_MAJOR) "." TOSTR(VERSION_MINOR) "." TOSTR(VERSION_PATCH)

// rc形式版本号，用于windos rc文件中指定版本号
#define VERSION_RES VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH

#endif // VERSION_H
