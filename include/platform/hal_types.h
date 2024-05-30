// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0

/* As we want to support simple porting to further platform,
 * platform dependent types may be used here.
 */

#if ESP
    #include "platform/esp-idf/hal_types.h"
#elif unix
    #include "platform/posix/hal_types.h"
#endif