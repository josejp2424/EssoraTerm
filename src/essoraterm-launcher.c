#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define REAL_BINARY "/usr/lib/essoraterm/essoraterm-bin"
#define SETTINGS_BINARY "/usr/bin/essoraterm-settings"
#define MAX_XRM_OPTIONS 40

typedef struct {
    char background[16];
    char foreground[16];
    char cursor_color[16];
    char selection_background[16];
    char selection_foreground[16];
    char font_family[160];
    double font_size;
    int columns;
    int rows;
    int scrollback;
    int scrollbar_width;
    int cursor_shape;
    bool scrollbar;
    bool scrollbar_right;
    bool cursor_blink;
    bool visual_bell;
    bool scroll_on_key;
    bool scroll_on_output;
    bool allow_bold;
    unsigned present;
} Settings;

enum {
    P_BACKGROUND = 1u << 0,
    P_FOREGROUND = 1u << 1,
    P_CURSOR_COLOR = 1u << 2,
    P_SELECTION_BACKGROUND = 1u << 3,
    P_SELECTION_FOREGROUND = 1u << 4,
    P_FONT_FAMILY = 1u << 5,
    P_FONT_SIZE = 1u << 6,
    P_COLUMNS = 1u << 7,
    P_ROWS = 1u << 8,
    P_SCROLLBACK = 1u << 9,
    P_SCROLLBAR = 1u << 10,
    P_SCROLLBAR_RIGHT = 1u << 11,
    P_SCROLLBAR_WIDTH = 1u << 12,
    P_CURSOR_BLINK = 1u << 13,
    P_VISUAL_BELL = 1u << 14,
    P_SCROLL_ON_KEY = 1u << 15,
    P_SCROLL_ON_OUTPUT = 1u << 16,
    P_ALLOW_BOLD = 1u << 17,
    P_CURSOR_SHAPE = 1u << 18
};

static const char *home_directory(void)
{
    const char *home = getenv("HOME");
    struct passwd *pw;

    if (home != NULL && home[0] == '/')
        return home;

    pw = getpwuid(getuid());
    return (pw != NULL && pw->pw_dir != NULL) ? pw->pw_dir : NULL;
}

static char *trim(char *text)
{
    char *end;

    while (*text != '\0' && isspace((unsigned char)*text))
        ++text;

    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1]))
        --end;
    *end = '\0';
    return text;
}

static bool valid_color(const char *value)
{
    size_t i;

    if (value == NULL || value[0] != '#' || strlen(value) != 7)
        return false;
    for (i = 1; i < 7; ++i) {
        if (!isxdigit((unsigned char)value[i]))
            return false;
    }
    return true;
}

static bool valid_font_family(const char *value)
{
    const unsigned char *p = (const unsigned char *)value;
    size_t length = 0;

    if (value == NULL || *value == '\0')
        return false;
    while (*p != '\0') {
        if (*p == '\n' || *p == '\r' || *p == '=')
            return false;
        if (*p < 0x20 && *p != '\t')
            return false;
        ++p;
        if (++length >= 150)
            return false;
    }
    return true;
}

static bool parse_bool(const char *value, bool *result)
{
    if (strcasecmp(value, "true") == 0 || strcmp(value, "1") == 0 ||
        strcasecmp(value, "yes") == 0 || strcasecmp(value, "on") == 0) {
        *result = true;
        return true;
    }
    if (strcasecmp(value, "false") == 0 || strcmp(value, "0") == 0 ||
        strcasecmp(value, "no") == 0 || strcasecmp(value, "off") == 0) {
        *result = false;
        return true;
    }
    return false;
}

static bool parse_int_range(const char *value, int min, int max, int *result)
{
    char *end = NULL;
    long number;

    errno = 0;
    number = strtol(value, &end, 10);
    if (errno != 0 || end == value || *trim(end) != '\0' ||
        number < min || number > max)
        return false;
    *result = (int)number;
    return true;
}

static bool parse_double_range(const char *value, double min, double max,
                               double *result)
{
    char *end = NULL;
    double number;

    errno = 0;
    number = strtod(value, &end);
    if (errno != 0 || end == value || *trim(end) != '\0' ||
        number < min || number > max)
        return false;
    *result = number;
    return true;
}

static void copy_string(char *destination, size_t size, const char *value)
{
    snprintf(destination, size, "%s", value);
}

static void read_setting(Settings *settings, const char *key, const char *value)
{
    bool flag;
    int number;
    double decimal;

    if (strcmp(key, "background") == 0 && valid_color(value)) {
        copy_string(settings->background, sizeof(settings->background), value);
        settings->present |= P_BACKGROUND;
    } else if (strcmp(key, "foreground") == 0 && valid_color(value)) {
        copy_string(settings->foreground, sizeof(settings->foreground), value);
        settings->present |= P_FOREGROUND;
    } else if (strcmp(key, "cursor_color") == 0 && valid_color(value)) {
        copy_string(settings->cursor_color, sizeof(settings->cursor_color), value);
        settings->present |= P_CURSOR_COLOR;
    } else if (strcmp(key, "selection_background") == 0 && valid_color(value)) {
        copy_string(settings->selection_background,
                    sizeof(settings->selection_background), value);
        settings->present |= P_SELECTION_BACKGROUND;
    } else if (strcmp(key, "selection_foreground") == 0 && valid_color(value)) {
        copy_string(settings->selection_foreground,
                    sizeof(settings->selection_foreground), value);
        settings->present |= P_SELECTION_FOREGROUND;
    } else if (strcmp(key, "font_family") == 0 && valid_font_family(value)) {
        copy_string(settings->font_family, sizeof(settings->font_family), value);
        settings->present |= P_FONT_FAMILY;
    } else if (strcmp(key, "font_size") == 0 &&
               parse_double_range(value, 5.0, 48.0, &decimal)) {
        settings->font_size = decimal;
        settings->present |= P_FONT_SIZE;
    } else if (strcmp(key, "columns") == 0 &&
               parse_int_range(value, 40, 300, &number)) {
        settings->columns = number;
        settings->present |= P_COLUMNS;
    } else if (strcmp(key, "rows") == 0 &&
               parse_int_range(value, 10, 100, &number)) {
        settings->rows = number;
        settings->present |= P_ROWS;
    } else if (strcmp(key, "scrollback") == 0 &&
               parse_int_range(value, 0, 1000000, &number)) {
        settings->scrollback = number;
        settings->present |= P_SCROLLBACK;
    } else if (strcmp(key, "scrollbar_width") == 0 &&
               parse_int_range(value, 6, 40, &number)) {
        settings->scrollbar_width = number;
        settings->present |= P_SCROLLBAR_WIDTH;
    } else if (strcmp(key, "cursor_shape") == 0 &&
               parse_int_range(value, 0, 2, &number)) {
        settings->cursor_shape = number;
        settings->present |= P_CURSOR_SHAPE;
    } else if (strcmp(key, "scrollbar") == 0 && parse_bool(value, &flag)) {
        settings->scrollbar = flag;
        settings->present |= P_SCROLLBAR;
    } else if (strcmp(key, "scrollbar_right") == 0 && parse_bool(value, &flag)) {
        settings->scrollbar_right = flag;
        settings->present |= P_SCROLLBAR_RIGHT;
    } else if (strcmp(key, "cursor_blink") == 0 && parse_bool(value, &flag)) {
        settings->cursor_blink = flag;
        settings->present |= P_CURSOR_BLINK;
    } else if (strcmp(key, "visual_bell") == 0 && parse_bool(value, &flag)) {
        settings->visual_bell = flag;
        settings->present |= P_VISUAL_BELL;
    } else if (strcmp(key, "scroll_on_key") == 0 && parse_bool(value, &flag)) {
        settings->scroll_on_key = flag;
        settings->present |= P_SCROLL_ON_KEY;
    } else if (strcmp(key, "scroll_on_output") == 0 && parse_bool(value, &flag)) {
        settings->scroll_on_output = flag;
        settings->present |= P_SCROLL_ON_OUTPUT;
    } else if (strcmp(key, "allow_bold") == 0 && parse_bool(value, &flag)) {
        settings->allow_bold = flag;
        settings->present |= P_ALLOW_BOLD;
    }
}

static void load_settings(Settings *settings)
{
    const char *home = home_directory();
    char path[PATH_MAX];
    char line[512];
    FILE *file;

    memset(settings, 0, sizeof(*settings));
    if (home == NULL || getenv("ESSORATERM_NO_CONFIG") != NULL)
        return;

    if (snprintf(path, sizeof(path), "%s/.config/essoraterm/settings.conf", home)
        >= (int)sizeof(path))
        return;

    file = fopen(path, "r");
    if (file == NULL)
        return;

    while (fgets(line, sizeof(line), file) != NULL) {
        char *key;
        char *value;
        char *equal;

        key = trim(line);
        if (*key == '\0' || *key == '#' || *key == ';' || *key == '[')
            continue;
        equal = strchr(key, '=');
        if (equal == NULL)
            continue;
        *equal = '\0';
        value = trim(equal + 1);
        key = trim(key);
        read_setting(settings, key, value);
    }
    fclose(file);
}

static void add_xrm(char **arguments, int *position, const char *resource,
                    const char *value)
{
    char *entry = NULL;

    if (*position >= 1 + MAX_XRM_OPTIONS * 2)
        return;
    if (asprintf(&entry, "%s: %s", resource, value) < 0 || entry == NULL)
        return;
    arguments[(*position)++] = (char *)"-xrm";
    arguments[(*position)++] = entry;
}

static void add_bool_xrm(char **arguments, int *position, const char *resource,
                         bool value)
{
    add_xrm(arguments, position, resource, value ? "true" : "false");
}

static void launch_preferences(void)
{
    execl(SETTINGS_BINARY, "essoraterm-settings", (char *)NULL);
    fprintf(stderr, "EssoraTerm: cannot start %s: %s\n",
            SETTINGS_BINARY, strerror(errno));
    exit(127);
}

int main(int argc, char **argv)
{
    Settings settings;
    char **arguments;
    const char *real_binary;
    int position = 1;
    int n;
    char buffer[256];

    if (argc >= 2 && (strcmp(argv[1], "--preferences") == 0 ||
                      strcmp(argv[1], "--settings") == 0))
        launch_preferences();

    load_settings(&settings);
    arguments = calloc((size_t)argc + MAX_XRM_OPTIONS * 2 + 2,
                       sizeof(*arguments));
    if (arguments == NULL) {
        perror("essoraterm: calloc");
        return 1;
    }

    arguments[0] = (char *)"essoraterm";

    if (settings.present & P_BACKGROUND)
        add_xrm(arguments, &position, "EssoraTerm*background", settings.background);
    if (settings.present & P_FOREGROUND)
        add_xrm(arguments, &position, "EssoraTerm*foreground", settings.foreground);
    if (settings.present & P_CURSOR_COLOR)
        add_xrm(arguments, &position, "EssoraTerm*cursorColor", settings.cursor_color);
    if (settings.present & P_SELECTION_BACKGROUND)
        add_xrm(arguments, &position, "EssoraTerm*highlightColor",
                settings.selection_background);
    if (settings.present & P_SELECTION_FOREGROUND)
        add_xrm(arguments, &position, "EssoraTerm*highlightTextColor",
                settings.selection_foreground);
    if (settings.present & P_FONT_FAMILY) {
        snprintf(buffer, sizeof(buffer),
                 "%s:style=Regular:antialias=true:autohint=true",
                 settings.font_family);
        add_xrm(arguments, &position, "EssoraTerm*faceName", buffer);
        add_bool_xrm(arguments, &position, "EssoraTerm*renderFont", true);
    }
    if (settings.present & P_FONT_SIZE) {
        snprintf(buffer, sizeof(buffer), "%.1f", settings.font_size);
        add_xrm(arguments, &position, "EssoraTerm*faceSize", buffer);
    }
    if ((settings.present & P_COLUMNS) && (settings.present & P_ROWS)) {
        snprintf(buffer, sizeof(buffer), "%dx%d", settings.columns, settings.rows);
        add_xrm(arguments, &position, "EssoraTerm*VT100.geometry", buffer);
    }
    if (settings.present & P_SCROLLBACK) {
        snprintf(buffer, sizeof(buffer), "%d", settings.scrollback);
        add_xrm(arguments, &position, "EssoraTerm*saveLines", buffer);
    }
    if (settings.present & P_SCROLLBAR)
        add_bool_xrm(arguments, &position, "EssoraTerm*VT100.scrollBar",
                     settings.scrollbar);
    if (settings.present & P_SCROLLBAR_RIGHT)
        add_bool_xrm(arguments, &position, "EssoraTerm*rightScrollBar",
                     settings.scrollbar_right);
    if (settings.present & P_SCROLLBAR_WIDTH) {
        snprintf(buffer, sizeof(buffer), "%d", settings.scrollbar_width);
        add_xrm(arguments, &position, "EssoraTerm*VT100*scrollbar.width", buffer);
    }
    if (settings.present & P_CURSOR_BLINK)
        add_bool_xrm(arguments, &position, "EssoraTerm*cursorBlink",
                     settings.cursor_blink);
    if (settings.present & P_VISUAL_BELL)
        add_bool_xrm(arguments, &position, "EssoraTerm*visualBell",
                     settings.visual_bell);
    if (settings.present & P_SCROLL_ON_KEY)
        add_bool_xrm(arguments, &position, "EssoraTerm*scrollKey",
                     settings.scroll_on_key);
    if (settings.present & P_SCROLL_ON_OUTPUT)
        add_bool_xrm(arguments, &position, "EssoraTerm*scrollTtyOutput",
                     settings.scroll_on_output);
    if (settings.present & P_ALLOW_BOLD)
        add_bool_xrm(arguments, &position, "EssoraTerm*allowBoldFonts",
                     settings.allow_bold);
    if (settings.present & P_CURSOR_SHAPE) {
        add_bool_xrm(arguments, &position, "EssoraTerm*cursorUnderLine",
                     settings.cursor_shape == 1);
        add_bool_xrm(arguments, &position, "EssoraTerm*cursorBar",
                     settings.cursor_shape == 2);
    }

    for (n = 1; n < argc; ++n)
        arguments[position++] = argv[n];
    arguments[position] = NULL;

    real_binary = getenv("ESSORATERM_BINARY");
    if (real_binary == NULL || *real_binary == '\0')
        real_binary = REAL_BINARY;

    execv(real_binary, arguments);
    fprintf(stderr, "EssoraTerm: cannot execute %s: %s\n",
            real_binary, strerror(errno));
    return 127;
}
