#include "screen.h"
#include <esp_err.h>
#include <esp_log.h>

explicit screen::screen(void)
 : m_screen_handle(nullptr)
{
    /* Do nothing. */
}

virtual screen::~screen(void)
{
    if(m_screen_handle)
    {
        lv_obj_del(m_screen_handle);
        m_screen_handle = nullptr;
    }
}

void screen::create(void)
{
    if(!m_screen_handle)
    {
        m_screen_handle = lv_obj_create(nullptr);
    }
}

void screen::load(void)
{
    if(m_screen_handle)
    {
        lv_scr_load(m_screen_handle);
    }
}

lv_obj_t*& screen::raw(void)
{
    return m_screen_handle;
}

