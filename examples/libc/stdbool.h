#ifndef __STDBOOL_H_
#define __STDBOOL_H_

#ifndef __cplusplus

/**
* Only define these for C, since C++ has its own bool support
*/

#define true 1
#define false 0
typedef _Bool bool;

#endif

#endif //__STDBOOL_H_
