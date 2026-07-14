/*
 * EssoraTerm contextual-menu translations.
 * English is the fallback language.
 *
 * Copyright (c) 2026 josejp2424
 * SPDX-License-Identifier: MIT
 */
#ifndef ESSORATERM_I18N_H
#define ESSORATERM_I18N_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *lang;
    const char *copy;
    const char *paste;
    const char *select_all;
    const char *clear_terminal;
    const char *preferences;
} EssoraTermTranslation;

static const EssoraTermTranslation essoraterm_translations[] = {
    { "en", "Copy",       "Paste",       "Select All",          "Clear Terminal", "Preferences" },
    { "ar", "نسخ",        "لصق",         "تحديد الكل",          "مسح الطرفية", "التفضيلات" },
    { "es", "Copiar",     "Pegar",       "Seleccionar todo",     "Limpiar terminal", "Preferencias" },
    { "ca", "Copia",      "Enganxa",     "Selecciona-ho tot",    "Neteja el terminal", "Preferències" },
    { "de", "Kopieren",   "Einfügen",    "Alles auswählen",      "Terminal leeren", "Einstellungen" },
    { "fr", "Copier",     "Coller",      "Tout sélectionner",    "Effacer le terminal", "Préférences" },
    { "it", "Copia",      "Incolla",     "Seleziona tutto",      "Pulisci terminale", "Preferenze" },
    { "pt", "Copiar",     "Colar",       "Selecionar tudo",      "Limpar terminal", "Preferências" },
    { "hu", "Másolás",    "Beillesztés", "Összes kijelölése",    "Terminál törlése", "Beállítások" },
    { "ru", "Копировать", "Вставить",    "Выбрать всё",          "Очистить терминал", "Настройки" },
    { "ja", "コピー",      "貼り付け",      "すべて選択",            "端末をクリア", "設定" },
    { "zh", "复制",        "粘贴",         "全选",                  "清空终端", "设置" }
};

static void
essoraterm_trim_locale(char *value)
{
    char *start = value;
    char *end;

    while (*start != '\0' && isspace((unsigned char) *start))
        ++start;

    if (start != value)
        memmove(value, start, strlen(start) + 1);

    end = value + strlen(value);
    while (end > value && isspace((unsigned char) end[-1]))
        --end;
    *end = '\0';

    if ((value[0] == '"' || value[0] == '\'') && strlen(value) > 1) {
        size_t len = strlen(value);
        if (value[len - 1] == value[0]) {
            memmove(value, value + 1, len - 2);
            value[len - 2] = '\0';
        }
    }

    end = strchr(value, ':');
    if (end != NULL)
        *end = '\0';
}

static int
essoraterm_read_locale_file(const char *path, char *buffer, size_t length)
{
    FILE *fp;
    char line[256];

    fp = fopen(path, "r");
    if (fp == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *equal;
        char *name = line;
        char *value;

        while (*name != '\0' && isspace((unsigned char) *name))
            ++name;
        if (*name == '#' || *name == '\0')
            continue;

        equal = strchr(name, '=');
        if (equal == NULL)
            continue;
        *equal = '\0';
        value = equal + 1;
        essoraterm_trim_locale(name);

        if (strcmp(name, "LC_MESSAGES") == 0 ||
            strcmp(name, "LC_ALL") == 0 ||
            strcmp(name, "LANG") == 0) {
            essoraterm_trim_locale(value);
            if (*value != '\0') {
                strncpy(buffer, value, length - 1);
                buffer[length - 1] = '\0';
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}

static const char *
essoraterm_locale_name(void)
{
    static char file_locale[128];
    const char *value;

    value = getenv("LANGUAGE");
    if (value != NULL && *value != '\0')
        return value;

    value = getenv("LC_ALL");
    if (value != NULL && *value != '\0')
        return value;

    value = getenv("LC_MESSAGES");
    if (value != NULL && *value != '\0')
        return value;

    value = getenv("LANG");
    if (value != NULL && *value != '\0')
        return value;

    if (essoraterm_read_locale_file("/etc/locale.conf",
                                    file_locale, sizeof(file_locale)))
        return file_locale;

    if (essoraterm_read_locale_file("/etc/default/locale",
                                    file_locale, sizeof(file_locale)))
        return file_locale;

    return "en";
}

static void
essoraterm_language_code(char code[3])
{
    const char *locale = essoraterm_locale_name();

    code[0] = 'e';
    code[1] = 'n';
    code[2] = '\0';

    if (locale == NULL || locale[0] == '\0')
        return;

    if ((locale[0] == 'C' && (locale[1] == '\0' || locale[1] == '.')) ||
        strcmp(locale, "POSIX") == 0)
        return;

    if (isalpha((unsigned char) locale[0]) &&
        isalpha((unsigned char) locale[1])) {
        code[0] = (char) tolower((unsigned char) locale[0]);
        code[1] = (char) tolower((unsigned char) locale[1]);
    }
}

static const EssoraTermTranslation *
essoraterm_translation(void)
{
    static const EssoraTermTranslation *cached;
    char code[3];
    size_t n;

    if (cached != NULL)
        return cached;

    essoraterm_language_code(code);
    cached = &essoraterm_translations[0];

    for (n = 0;
         n < sizeof(essoraterm_translations) / sizeof(essoraterm_translations[0]);
         ++n) {
        if (strcmp(code, essoraterm_translations[n].lang) == 0) {
            cached = &essoraterm_translations[n];
            break;
        }
    }

    return cached;
}

static const char *
essoraterm_menu_label(const char *name)
{
    const EssoraTermTranslation *tr = essoraterm_translation();

    if (strcmp(name, "copy") == 0)
        return tr->copy;
    if (strcmp(name, "paste") == 0)
        return tr->paste;
    if (strcmp(name, "selectAll") == 0)
        return tr->select_all;
    if (strcmp(name, "clearTerminal") == 0)
        return tr->clear_terminal;
    if (strcmp(name, "preferences") == 0)
        return tr->preferences;

    return NULL;
}

#endif /* ESSORATERM_I18N_H */
