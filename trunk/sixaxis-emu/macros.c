
/* Sixaxis emulator

   Copyright (c) 2010 Mathieu Laurendeau

   License: GPLv3
*/

#include <pthread.h>
#include <SDL/SDL.h>
#include <limits.h>
#include "macros.h"
#include <unistd.h>
#include <stdio.h>

#ifdef WIN32
#define LINE_MAX 1024
#endif

/* This is the minimum amount of time between KEYDOWN and KEYUP. */
#define DEFAULT_DELAY 50

/* This is the name of the file that contains all macro files. */
#define MACRO_FILE "macros.txt"

#define MAX_ARG_LENGTH 32

typedef struct {
    char arg[MAX_ARG_LENGTH];
    SDLKey key;
} s_arg_key;

/*
 * This table gives a correspondence between a char string and a key code.
 */
s_arg_key arg_key_table[] = {
        {"UNKNOWN",         SDLK_UNKNOWN         },
        {"FIRST",             SDLK_FIRST             },
        {"BACKSPACE",         SDLK_BACKSPACE      },
        {"TAB",             SDLK_TAB             },
        {"CLEAR",             SDLK_CLEAR             },
        {"RETURN",             SDLK_RETURN         },
        {"PAUSE",             SDLK_PAUSE          },
        {"ESCAPE",             SDLK_ESCAPE         },
        {"SPACE",                SDLK_SPACE          },
        {"EXCLAIM",         SDLK_EXCLAIM         },
        {"QUOTEDBL",         SDLK_QUOTEDBL       },
        {"HASH",             SDLK_HASH           },
        {"DOLLAR",             SDLK_DOLLAR         },
        {"AMPERSAND",         SDLK_AMPERSAND      },
        {"QUOTE",             SDLK_QUOTE          },
        {"LEFTPAREN",         SDLK_AMPERSAND      },
        {"RIGHTPAREN",         SDLK_RIGHTPAREN     },
        {"ASTERISK",         SDLK_ASTERISK       },
        {"PLUS",             SDLK_PLUS           },
        {"COMMA",             SDLK_COMMA          },
        {"MINUS",             SDLK_MINUS          },
        {"PERIOD",             SDLK_PERIOD         },
        {"SLASH",             SDLK_SLASH          },
        {"0",                 SDLK_0                 },
        {"1",                 SDLK_1                 },
        {"2",                 SDLK_2                 },
        {"3",                 SDLK_3                 },
        {"4",                 SDLK_4                 },
        {"5",                 SDLK_5                 },
        {"6",                 SDLK_6                 },
        {"7",                 SDLK_7                 },
        {"8",                 SDLK_8                 },
        {"9",                 SDLK_9                 },
        {"COLON",             SDLK_COLON             },
        {"SEMICOLON",         SDLK_SEMICOLON      },
        {"LESS",             SDLK_LESS           },
        {"EQUALS",             SDLK_EQUALS         },
        {"GREATER",         SDLK_GREATER        },
        {"QUESTION",         SDLK_QUESTION       },
        {"AT",                 SDLK_AT             },
        /*
           Skip uppercase letters
         */
        {"LEFTBRACKET",        SDLK_LEFTBRACKET     },
        {"BACKSLASH",         SDLK_BACKSLASH         },
        {"RIGHTBRACKET",     SDLK_RIGHTBRACKET     },
        {"CARET",              SDLK_CARET             },
        {"UNDERSCORE",         SDLK_UNDERSCORE     },
        {"BACKQUOTE",         SDLK_BACKQUOTE         },
        {"a",                 SDLK_a                 },
        {"b",                 SDLK_b                 },
        {"c",                 SDLK_c                 },
        {"d",                 SDLK_d                 },
        {"e",                 SDLK_e                 },
        {"f",                 SDLK_f                 },
        {"g",                 SDLK_g                 },
        {"h",                 SDLK_h                 },
        {"i",                 SDLK_i                 },
        {"j",                 SDLK_j                 },
        {"k",                 SDLK_k                 },
        {"l",                 SDLK_l                 },
        {"m",                 SDLK_m                 },
        {"n",                 SDLK_n                 },
        {"o",                 SDLK_o                 },
        {"p",                 SDLK_p                 },
        {"q",                 SDLK_q                 },
        {"r",                 SDLK_r                 },
        {"s",                 SDLK_s                 },
        {"t",                 SDLK_t                 },
        {"u",                 SDLK_u                 },
        {"v",                 SDLK_v                 },
        {"w",                 SDLK_w                 },
        {"x",                 SDLK_x                 },
        {"y",                 SDLK_y                 },
        {"z",                 SDLK_z                 },
        {"DELETE",             SDLK_DELETE         },
        /* End of ASCII mapped keysyms */

        /* International keyboard syms */
        {"WORLD_0",         SDLK_WORLD_0         },
        {"WORLD_1",         SDLK_WORLD_1         },
        {"WORLD_2",         SDLK_WORLD_2         },
        {"WORLD_3",         SDLK_WORLD_3         },
        {"WORLD_4",         SDLK_WORLD_4         },
        {"WORLD_5",         SDLK_WORLD_5         },
        {"WORLD_6",         SDLK_WORLD_6         },
        {"WORLD_7",         SDLK_WORLD_7         },
        {"WORLD_8",         SDLK_WORLD_8         },
        {"WORLD_9",         SDLK_WORLD_9         },
        {"WORLD_10",         SDLK_WORLD_10         },
        {"WORLD_11",         SDLK_WORLD_11         },
        {"WORLD_12",         SDLK_WORLD_12         },
        {"WORLD_13",         SDLK_WORLD_13         },
        {"WORLD_14",         SDLK_WORLD_14         },
        {"WORLD_15",         SDLK_WORLD_15         },
        {"WORLD_16",         SDLK_WORLD_16         },
        {"WORLD_17",         SDLK_WORLD_17         },
        {"WORLD_18",         SDLK_WORLD_18         },
        {"WORLD_19",         SDLK_WORLD_19         },
        {"WORLD_20",         SDLK_WORLD_20         },
        {"WORLD_21",         SDLK_WORLD_21         },
        {"WORLD_22",         SDLK_WORLD_22         },
        {"WORLD_23",         SDLK_WORLD_23         },
        {"WORLD_24",         SDLK_WORLD_24         },
        {"WORLD_25",         SDLK_WORLD_25         },
        {"WORLD_26",         SDLK_WORLD_26         },
        {"WORLD_27",         SDLK_WORLD_27         },
        {"WORLD_28",         SDLK_WORLD_28         },
        {"WORLD_29",         SDLK_WORLD_29         },
        {"WORLD_30",         SDLK_WORLD_30         },
        {"WORLD_31",         SDLK_WORLD_31         },
        {"WORLD_32",         SDLK_WORLD_32         },
        {"WORLD_33",         SDLK_WORLD_33         },
        {"WORLD_34",         SDLK_WORLD_34         },
        {"WORLD_35",         SDLK_WORLD_35         },
        {"WORLD_36",         SDLK_WORLD_36         },
        {"WORLD_37",         SDLK_WORLD_37         },
        {"WORLD_38",         SDLK_WORLD_38         },
        {"WORLD_39",         SDLK_WORLD_39         },
        {"WORLD_40",         SDLK_WORLD_40         },
        {"WORLD_41",         SDLK_WORLD_41         },
        {"WORLD_42",         SDLK_WORLD_42         },
        {"WORLD_43",         SDLK_WORLD_43         },
        {"WORLD_44",         SDLK_WORLD_44         },
        {"WORLD_45",         SDLK_WORLD_45         },
        {"WORLD_46",         SDLK_WORLD_46         },
        {"WORLD_47",         SDLK_WORLD_47         },
        {"WORLD_48",         SDLK_WORLD_48         },
        {"WORLD_49",         SDLK_WORLD_49         },
        {"WORLD_50",         SDLK_WORLD_50         },
        {"WORLD_51",         SDLK_WORLD_51         },
        {"WORLD_52",         SDLK_WORLD_52         },
        {"WORLD_53",         SDLK_WORLD_53         },
        {"WORLD_54",         SDLK_WORLD_54         },
        {"WORLD_55",         SDLK_WORLD_55         },
        {"WORLD_56",         SDLK_WORLD_56         },
        {"WORLD_57",         SDLK_WORLD_57         },
        {"WORLD_58",         SDLK_WORLD_58         },
        {"WORLD_59",         SDLK_WORLD_59         },
        {"WORLD_60",         SDLK_WORLD_60         },
        {"WORLD_61",         SDLK_WORLD_61         },
        {"WORLD_62",         SDLK_WORLD_62         },
        {"WORLD_63",         SDLK_WORLD_63         },
        {"WORLD_64",         SDLK_WORLD_64         },
        {"WORLD_65",         SDLK_WORLD_65         },
        {"WORLD_66",         SDLK_WORLD_66         },
        {"WORLD_67",         SDLK_WORLD_67         },
        {"WORLD_68",         SDLK_WORLD_68         },
        {"WORLD_69",         SDLK_WORLD_69         },
        {"WORLD_70",         SDLK_WORLD_70         },
        {"WORLD_71",         SDLK_WORLD_71         },
        {"WORLD_72",         SDLK_WORLD_72         },
        {"WORLD_73",         SDLK_WORLD_73         },
        {"WORLD_74",         SDLK_WORLD_74         },
        {"WORLD_75",         SDLK_WORLD_75         },
        {"WORLD_76",         SDLK_WORLD_76         },
        {"WORLD_77",         SDLK_WORLD_77         },
        {"WORLD_78",         SDLK_WORLD_78         },
        {"WORLD_79",         SDLK_WORLD_79         },
        {"WORLD_80",         SDLK_WORLD_80         },
        {"WORLD_81",         SDLK_WORLD_81         },
        {"WORLD_82",         SDLK_WORLD_82         },
        {"WORLD_83",         SDLK_WORLD_83         },
        {"WORLD_84",         SDLK_WORLD_84         },
        {"WORLD_85",         SDLK_WORLD_85         },
        {"WORLD_86",         SDLK_WORLD_86         },
        {"WORLD_87",         SDLK_WORLD_87         },
        {"WORLD_88",         SDLK_WORLD_88         },
        {"WORLD_89",         SDLK_WORLD_89         },
        {"WORLD_90",         SDLK_WORLD_90         },
        {"WORLD_91",         SDLK_WORLD_91         },
        {"WORLD_92",         SDLK_WORLD_92         },
        {"WORLD_93",         SDLK_WORLD_93         },
        {"WORLD_94",         SDLK_WORLD_94         },
        {"WORLD_95",         SDLK_WORLD_95         },

        /* Numeric keypad */
        {"KP0",             SDLK_KP0             },
        {"KP1",             SDLK_KP1             },
        {"KP2",             SDLK_KP2             },
        {"KP3",             SDLK_KP3             },
        {"KP4",             SDLK_KP4             },
        {"KP5",             SDLK_KP5             },
        {"KP6",             SDLK_KP6             },
        {"KP7",             SDLK_KP7             },
        {"KP8",             SDLK_KP8             },
        {"KP9",             SDLK_KP9             },
        {"KP_PERIOD",         SDLK_KP_PERIOD        },
        {"KP_DIVIDE",         SDLK_KP_DIVIDE        },
        {"KP_MULTIPLY",        SDLK_KP_MULTIPLY    },
        {"KP_MINUS",         SDLK_KP_MINUS        },
        {"KP_PLUS",         SDLK_KP_PLUS        },
        {"KP_ENTER",         SDLK_KP_ENTER        },
        {"KP_EQUALS",         SDLK_KP_EQUALS        },

        /* Arrows + Home/End pad */
        {"UP",                 SDLK_UP             },
        {"DOWN",             SDLK_DOWN            },
        {"RIGHT",             SDLK_RIGHT             },
        {"LEFT",             SDLK_LEFT             },
        {"INSERT",             SDLK_INSERT            },
        {"HOME",             SDLK_HOME             },
        {"END",             SDLK_END             },
        {"PAGEUP",             SDLK_PAGEUP            },
        {"PAGEDOWN",         SDLK_PAGEDOWN        },

        /* Function keys */
        {"F1",                 SDLK_F1             },
        {"F2",                 SDLK_F2             },
        {"F3",                 SDLK_F3             },
        {"F4",                 SDLK_F4             },
        {"F5",                 SDLK_F5             },
        {"F6",                 SDLK_F6             },
        {"F7",                 SDLK_F7             },
        {"F8",                 SDLK_F8             },
        {"F9",                 SDLK_F9             },
        {"F10",             SDLK_F10             },
        {"F11",             SDLK_F11             },
        {"F12",             SDLK_F12             },
        {"F13",             SDLK_F13             },
        {"F14",             SDLK_F14             },
        {"F15",             SDLK_F15             },

        /* Key state modifier keys */
        {"NUMLOCK",         SDLK_NUMLOCK        },
        {"CAPSLOCK",         SDLK_CAPSLOCK        },
        {"SCROLLOCK",         SDLK_SCROLLOCK        },
        {"RSHIFT",             SDLK_RSHIFT            },
        {"LSHIFT",             SDLK_LSHIFT            },
        {"RCTRL",             SDLK_RCTRL             },
        {"LCTRL",             SDLK_LCTRL             },
        {"RALT",             SDLK_RALT             },
        {"LALT",             SDLK_LALT             },
        {"RMETA",             SDLK_RMETA             },
        {"LMETA",             SDLK_LMETA             },
        {"LSUPER",             SDLK_LSUPER         },        /* Left "Windows" key */
        {"RSUPER",             SDLK_RSUPER            },        /* Right "Windows" key */
        {"MODE",             SDLK_MODE           },        /* "Alt Gr" key */
        {"COMPOSE",         SDLK_COMPOSE        },        /* Multi-key compose key */

        /* Miscellaneous function keys */
        {"HELP",             SDLK_HELP             },
        {"PRINT",             SDLK_PRINT             },
        {"SYSREQ",             SDLK_SYSREQ            },
        {"BREAK",             SDLK_BREAK             },
        {"MENU",             SDLK_MENU             },
        {"POWER",             SDLK_POWER          },        /* Power Macintosh power key */
        {"EURO",             SDLK_EURO             },        /* Some european keyboards */
        {"UNDO",             SDLK_UNDO             },        /* Atari keyboard has Undo */

        /* Add any other keys here */

        {"LAST",            SDLK_LAST             },
};

/*
 * This function gives a key code from a char string.
 */
SDLKey get_key_from_buffer(const char* buffer) {
    int i;

    for (i = 0; i < SDLK_LAST; i++) {
        if (!strncmp(arg_key_table[i].arg, buffer, MAX_ARG_LENGTH)) {
            return arg_key_table[i].key;
        }
    }

    return SDLK_UNKNOWN;
}

/*
 * This function gives a char string from a key code.
 */
const char* get_chars_from_key(SDLKey key) {
    int i;

    for (i = 0; i < SDLK_LAST; i++) {
        if (arg_key_table[i].key == key) {
            return arg_key_table[i].arg;
        }
    }

    return arg_key_table[0].arg;
}

typedef struct {
    SDL_Event event;
    int delay;
    int size;
    char macro[MAX_ARG_LENGTH];
} s_macro_event_delay;

/*
 * This table is used to store all the macros that are read from script files at the initialization of the process.
 */
s_macro_event_delay* macro_table[SDLK_LAST];

/*
 * Initializes macro_table.
 */
void initialize_macro_table() {
    memset(macro_table, 0x00, sizeof(macro_table));
}

/*
 * Cleans macro_table.
 * Frees all allocated blocks pointed by macro_table.
 */
void free_macros() {
    s_macro_event_delay** pt;

    for (pt = macro_table; pt < macro_table + SDLK_LAST; pt++) {
        free(*pt);
    }
}

/*
 * Allocates an element and initializes it to 0.
 */
void allocate_element(s_macro_event_delay** pt) {
    (*pt) = realloc((*pt), sizeof(s_macro_event_delay) * (++((*pt)->size)));
    memset((*pt) + (*pt)->size - 1, 0x00, sizeof(s_macro_event_delay));
}

/*
 * Processes a line of a script file (macro definition or command).
 * Returns the macro index in macro_table.
 * If line is a macro definition and the macro is already defined, returns SDLK_UNKNOWN.
 * If macro is SDLK_UNKNOWN and if line is not a macro definition, nothing is done.
 */
SDLKey process_line(const char* line, SDLKey macro) {
    char command[LINE_MAX];
    char argument[LINE_MAX];
    SDLKey key;
    int delay;
    s_macro_event_delay** pt;
    int ret;

    if (macro != SDLK_UNKNOWN) {
        pt = macro_table + macro;
    }

    ret = sscanf(line, "%s %s", command, argument);

    if(ret < 2)
    {
        /* invalid line */
        return macro;
    }

    if (!strncmp(command, "MACRO", strlen("MACRO"))) {
        macro = get_key_from_buffer(argument);
        pt = macro_table + macro;

        if (!*pt) {
            (*pt) = calloc(1, sizeof(s_macro_event_delay));
            (*pt)->size = 1;
            strncpy((*pt)->macro, argument, MAX_ARG_LENGTH);
        } else {
            printf("Macro %s defined twice!\n", argument);
            macro = SDLK_UNKNOWN;
        }
    } else if (!strncmp(command, "KEYDOWN", strlen("KEYDOWN"))) {
        if ((*pt)) {
            key = get_key_from_buffer(argument);

            allocate_element(pt);

            (*pt)[(*pt)->size - 1].event.type = SDL_KEYDOWN;
            (*pt)[(*pt)->size - 1].event.key.state = SDL_KEYDOWN;
            (*pt)[(*pt)->size - 1].event.key.keysym.sym = key;
            (*pt)[(*pt)->size - 1].delay = 0;
        }
    } else if (!strncmp(command, "KEYUP", strlen("KEYUP"))) {
        if ((*pt)) {
            key = get_key_from_buffer(argument);

            allocate_element(pt);

            (*pt)[(*pt)->size - 1].event.type = SDL_KEYUP;
            (*pt)[(*pt)->size - 1].event.key.state = SDL_KEYUP;
            (*pt)[(*pt)->size - 1].event.key.keysym.sym = key;
            (*pt)[(*pt)->size - 1].delay = 0;
        }
    } else if (!strncmp(command, "KEY", strlen("KEY"))) {
        if ((*pt)) {
            key = get_key_from_buffer(argument);

            allocate_element(pt);

            (*pt)[(*pt)->size - 1].event.type = SDL_KEYDOWN;
            (*pt)[(*pt)->size - 1].event.key.state = SDL_KEYDOWN;
            (*pt)[(*pt)->size - 1].event.key.keysym.sym = key;
            (*pt)[(*pt)->size - 1].delay = DEFAULT_DELAY;

            allocate_element(pt);

            (*pt)[(*pt)->size - 1].event.type = SDL_KEYUP;
            (*pt)[(*pt)->size - 1].event.key.state = SDL_KEYUP;
            (*pt)[(*pt)->size - 1].event.key.keysym.sym = key;
        }
    } else if (!strncmp(command, "DELAY", strlen("DELAY"))) {
        if ((*pt)) {
            delay = atoi(argument);

            allocate_element(pt);

            (*pt)[(*pt)->size - 1].delay = delay;
        }
    }

    return macro;
}

/*
 * Displays macro_table.
 */
void dump_scripts() {
    s_macro_event_delay** p_table;
    s_macro_event_delay* p_element;

    for (p_table = macro_table; p_table < macro_table + SDLK_LAST; p_table++) {
        if (*p_table) {
            printf("MACRO %s\n\n", (*p_table)->macro);
            for (p_element = *p_table; p_element && p_element < *p_table
                    + (*p_table)->size; p_element++) {
                if (p_element->event.type == SDL_KEYDOWN) {
                    printf("KEYDOWN %s\n", get_chars_from_key(
                            p_element->event.key.keysym.sym));
                } else if (p_element->event.type == SDL_KEYUP) {
                    printf("KEYUP %s\n", get_chars_from_key(
                            p_element->event.key.keysym.sym));
                }
                if (p_element->delay) {
                    printf("DELAY %d\n", p_element->delay);
                }
            }
            printf("\n");
        }
    }
}

/*
 * Reads macros from script files.
 */
void read_macros() {

    SDLKey macro = SDLK_UNKNOWN;

    char file[LINE_MAX];
    char line[LINE_MAX];
    FILE* fp;
    FILE* fplist = fopen(MACRO_FILE, "r");
    if (!fplist) {
        fprintf(stderr, "Can not find %s\n", MACRO_FILE);
    } else {
        while (fgets(file, LINE_MAX, fplist)) {
            if(file[strlen(file) - 1] == '\n')
            {
                file[strlen(file) - 1] = 0;
            }
            fp = fopen(file, "r");
            if (!fp) {
                fprintf(stderr, "Can not find '%s'\n", file);
            } else {
                while (fgets(line, LINE_MAX, fp)) {
                    if (line[0] != '#') {
                        macro = process_line(line, macro);
                    }
                }
                fclose(fp);
            }
        }
        fclose(fplist);
    }
}

/*
 * Initializes macro_table and reads macros from macro files.
 */
void initialize_macros() {
    initialize_macro_table();
    read_macros();
}

/*
 * This code is run by each thread that executes a macro.
 */
void macro(s_macro_event_delay* p_table) {

    s_macro_event_delay* p_element;

    for (p_element = p_table; p_element < p_table + p_table->size; p_element++) {
        if (p_element->event.type != SDL_NOEVENT) {
            SDL_PushEvent(&(p_element->event));
        }
        if (p_element->delay > 0) {
            usleep(p_element->delay*1000);
        }
    }
}

/*
 * Launches a thread if there is a macro for that key.
 */
void macro_lookup(SDLKey key)
{
    pthread_t thread;
    pthread_attr_t thread_attr;

    if(macro_table[key]) {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create( &thread, &thread_attr, (void*)macro, (void*) macro_table[key]);
    }
}
