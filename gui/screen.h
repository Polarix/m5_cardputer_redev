#ifndef _INCLUDE_CLASS_GUI_SCREEN_H_
#define _INCLUDE_CLASS_GUI_SCREEN_H_

#include "lvgl.h"

class screen
{
protected:
    lv_obj_t*       m_screen_handle;

public:
    explicit        screen(void);
    virtual         ~screen(void);
    void            create(void);
    void            load(void);
    lv_obj_t*&      raw(void);
}


#endif // _INCLUDE_CLASS_GUI_SCREEN_H_