#include "keypad_code.h"

const key_char_t keypad_map[] = 
{    /* char */               /* shift char */        /* caps char */         /* fn char */
{ 0, KEY_CTRL_CODE_NULL,      KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
/* Row 1 */
{ 1, '`',                     '~',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_ESC},
{ 2, '1',                     '!',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 3, '2',                     '@',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 4, '3',                     '#',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 5, '4',                     '$',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 6, '5',                     '%',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 7, '6',                     '^',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 8, '7',                     '&',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{ 9, '8',                     '*',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{10, '9',                     '(',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{11, '0',                     ')',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{12, '-',                     '_',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{13, '=',                     '+',                    KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{14, KEY_CTRL_CODE_BACKSPACE, KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_DEL},
/* Row 2 */
{15, KEY_CTRL_CODE_TAB,       KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{16, 'q',                     'Q',                    'Q',                    KEY_CTRL_CODE_NULL},
{17, 'w',                     'W',                    'W',                    KEY_CTRL_CODE_NULL},
{18, 'e',                     'E',                    'E',                    KEY_CTRL_CODE_NULL},
{19, 'r',                     'R',                    'R',                    KEY_CTRL_CODE_NULL},
{20, 't',                     'T',                    'T',                    KEY_CTRL_CODE_NULL},
{21, 'y',                     'Y',                    'Y',                    KEY_CTRL_CODE_NULL},
{22, 'u',                     'U',                    'U',                    KEY_CTRL_CODE_NULL},
{23, 'i',                     'I',                    'I',                    KEY_CTRL_CODE_NULL},
{24, 'o',                     'O',                    'O',                    KEY_CTRL_CODE_NULL},
{25, 'p',                     'P',                    'P',                    KEY_CTRL_CODE_NULL},
{26, '[',                     '{',                    '[',                    KEY_CTRL_CODE_NULL},
{27, ']',                     '}',                    ']',                    KEY_CTRL_CODE_NULL},
{28, '\\',                    '|',                    '\\',                   KEY_CTRL_CODE_NULL},
/* Row 3 */
{29, KEY_CTRL_CODE_FN,        KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{30, KEY_CTRL_CODE_CAPS,      KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{31, 'a',                     'A',                    'A',                    KEY_CTRL_CODE_NULL},
{32, 's',                     'S',                    'S',                    KEY_CTRL_CODE_NULL},
{33, 'd',                     'D',                    'D',                    KEY_CTRL_CODE_NULL},
{34, 'f',                     'F',                    'F',                    KEY_CTRL_CODE_NULL},
{35, 'g',                     'G',                    'G',                    KEY_CTRL_CODE_NULL},
{36, 'h',                     'H',                    'H',                    KEY_CTRL_CODE_NULL},
{37, 'j',                     'J',                    'J',                    KEY_CTRL_CODE_NULL},
{38, 'k',                     'K',                    'K',                    KEY_CTRL_CODE_NULL},
{39, 'l',                     'L',                    'L',                    KEY_CTRL_CODE_NULL},
{40, ';',                     ':',                    ';',                    KEY_CTRL_CODE_UP},
{41, '\'',                    '"',                    '\'',                   KEY_CTRL_CODE_NULL},
{42, KEY_CTRL_CODE_ENTER,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
/* Row 4 */
{43, KEY_CTRL_CODE_CTRL,      KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{44, KEY_CTRL_CODE_OPT,       KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{45, KEY_CTRL_CODE_ALT,       KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL,     KEY_CTRL_CODE_NULL},
{46, 'z',                     'Z',                    'Z',                    KEY_CTRL_CODE_NULL},
{47, 'x',                     'X',                    'X',                    KEY_CTRL_CODE_NULL},
{48, 'c',                     'C',                    'C',                    KEY_CTRL_CODE_NULL},
{49, 'v',                     'V',                    'V',                    KEY_CTRL_CODE_NULL},
{50, 'b',                     'B',                    'B',                    KEY_CTRL_CODE_NULL},
{51, 'n',                     'N',                    'N',                    KEY_CTRL_CODE_NULL},
{52, 'm',                     'M',                    'M',                    KEY_CTRL_CODE_NULL},
{53, ',',                     '<',                    ',',                    KEY_CTRL_CODE_NULL},
{54, '.',                     '>',                    '.',                    KEY_CTRL_CODE_NULL},
{55, '/',                     '?',                    '/',                    KEY_CTRL_CODE_NULL},
{56, KEY_CTRL_CODE_SPACE,     KEY_CTRL_CODE_SPACE,    KEY_CTRL_CODE_SPACE,    KEY_CTRL_CODE_NULL},
};

const key_char_t* keypad_get_char_map(int key_code)
{
    int key_char_map_index = (key_code>56)?0:key_code;
    return &keypad_map[key_char_map_index];
}

char keypad_get_char(int key_code, bool shift, bool caps, bool fn)
{
    char key_char = KEY_CTRL_CODE_NULL;
    const key_char_t* key_map = keypad_get_char_map(key_code);
    if(key_map)
    {
        if(fn)
        {
            key_char = key_map->key_fn_char;
        }
        else if(shift)
        {
            key_char = key_map->key_shift_char;
        }
    }
    return key_char;
}