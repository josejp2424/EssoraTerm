#define _POSIX_C_SOURCE 200809L

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <errno.h>
#include <locale.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    const char *lang;
    const char *title;
    const char *appearance;
    const char *terminal;
    const char *behavior;
    const char *font;
    const char *background;
    const char *foreground;
    const char *cursor_color;
    const char *selection_background;
    const char *selection_foreground;
    const char *preview;
    const char *columns;
    const char *rows;
    const char *scrollback;
    const char *scrollbar;
    const char *scrollbar_right;
    const char *scrollbar_width;
    const char *cursor_shape;
    const char *cursor_block;
    const char *cursor_underline;
    const char *cursor_bar;
    const char *cursor_blink;
    const char *visual_bell;
    const char *scroll_on_key;
    const char *scroll_on_output;
    const char *allow_bold;
    const char *restore;
    const char *test;
    const char *save;
    const char *close;
    const char *saved_title;
    const char *saved_message;
    const char *restore_title;
    const char *restore_message;
    const char *cancel;
    const char *error_title;
} Translation;

static const Translation translations[] = {
    { "en", "EssoraTerm Preferences", "Appearance", "Terminal", "Behavior",
      "Font", "Background", "Text", "Cursor", "Selection background",
      "Selection text", "EssoraTerm preview\nuser@essora:~$ echo Hello",
      "Columns", "Rows", "Scrollback lines", "Show scrollbar",
      "Scrollbar on the right", "Scrollbar width", "Cursor shape", "Block",
      "Underline", "Bar", "Blinking cursor", "Visual bell",
      "Scroll to bottom on key press", "Scroll to bottom on terminal output",
      "Allow bold fonts", "Restore defaults", "Open test terminal", "Save",
      "Close", "Preferences saved",
      "The changes will be used by new EssoraTerm windows.",
      "Restore defaults?", "The personal EssoraTerm configuration will be removed.",
      "Cancel", "EssoraTerm error" },
    { "es", "Preferencias de EssoraTerm", "Apariencia", "Terminal", "Comportamiento",
      "Fuente", "Fondo", "Texto", "Cursor", "Fondo de selección",
      "Texto seleccionado", "Vista previa de EssoraTerm\nusuario@essora:~$ echo Hola",
      "Columnas", "Filas", "Líneas del historial", "Mostrar barra de desplazamiento",
      "Barra a la derecha", "Ancho de la barra", "Forma del cursor", "Bloque",
      "Subrayado", "Barra", "Cursor intermitente", "Campana visual",
      "Bajar al escribir", "Bajar cuando hay salida en la terminal",
      "Permitir fuente en negrita", "Restaurar valores", "Abrir terminal de prueba",
      "Guardar", "Cerrar", "Preferencias guardadas",
      "Los cambios se usarán en las nuevas ventanas de EssoraTerm.",
      "¿Restaurar valores?", "Se eliminará la configuración personal de EssoraTerm.",
      "Cancelar", "Error de EssoraTerm" },
    { "ca", "Preferències d'EssoraTerm", "Aparença", "Terminal", "Comportament",
      "Tipus de lletra", "Fons", "Text", "Cursor", "Fons de selecció",
      "Text seleccionat", "Vista prèvia d'EssoraTerm\nusuari@essora:~$ echo Hola",
      "Columnes", "Files", "Línies de l'historial", "Mostra la barra de desplaçament",
      "Barra a la dreta", "Amplada de la barra", "Forma del cursor", "Bloc",
      "Subratllat", "Barra", "Cursor intermitent", "Avís visual",
      "Baixa en prémer una tecla", "Baixa amb la sortida del terminal",
      "Permet negreta", "Restaura els valors", "Obre un terminal de prova",
      "Desa", "Tanca", "Preferències desades",
      "Els canvis s'utilitzaran a les finestres noves d'EssoraTerm.",
      "Vols restaurar els valors?", "S'eliminarà la configuració personal d'EssoraTerm.",
      "Cancel·la", "Error d'EssoraTerm" },
    { "de", "EssoraTerm-Einstellungen", "Darstellung", "Terminal", "Verhalten",
      "Schrift", "Hintergrund", "Text", "Cursor", "Auswahlhintergrund",
      "Auswahltext", "EssoraTerm-Vorschau\nbenutzer@essora:~$ echo Hallo",
      "Spalten", "Zeilen", "Verlaufszeilen", "Bildlaufleiste anzeigen",
      "Bildlaufleiste rechts", "Breite der Bildlaufleiste", "Cursorform", "Block",
      "Unterstrich", "Balken", "Blinkender Cursor", "Visuelles Signal",
      "Bei Tastendruck nach unten", "Bei Terminalausgabe nach unten",
      "Fettschrift erlauben", "Standard wiederherstellen", "Testterminal öffnen",
      "Speichern", "Schließen", "Einstellungen gespeichert",
      "Die Änderungen gelten für neue EssoraTerm-Fenster.",
      "Standard wiederherstellen?", "Die persönliche EssoraTerm-Konfiguration wird entfernt.",
      "Abbrechen", "EssoraTerm-Fehler" },
    { "fr", "Préférences d’EssoraTerm", "Apparence", "Terminal", "Comportement",
      "Police", "Arrière-plan", "Texte", "Curseur", "Fond de sélection",
      "Texte sélectionné", "Aperçu d’EssoraTerm\nutilisateur@essora:~$ echo Bonjour",
      "Colonnes", "Lignes", "Lignes d’historique", "Afficher la barre de défilement",
      "Barre à droite", "Largeur de la barre", "Forme du curseur", "Bloc",
      "Souligné", "Barre", "Curseur clignotant", "Alerte visuelle",
      "Descendre lors de la saisie", "Descendre lors d’une sortie du terminal",
      "Autoriser le gras", "Restaurer les valeurs", "Ouvrir un terminal de test",
      "Enregistrer", "Fermer", "Préférences enregistrées",
      "Les changements seront utilisés par les nouvelles fenêtres EssoraTerm.",
      "Restaurer les valeurs ?", "La configuration personnelle d’EssoraTerm sera supprimée.",
      "Annuler", "Erreur EssoraTerm" },
    { "it", "Preferenze di EssoraTerm", "Aspetto", "Terminale", "Comportamento",
      "Carattere", "Sfondo", "Testo", "Cursore", "Sfondo selezione",
      "Testo selezionato", "Anteprima EssoraTerm\nutente@essora:~$ echo Ciao",
      "Colonne", "Righe", "Righe cronologia", "Mostra barra di scorrimento",
      "Barra a destra", "Larghezza barra", "Forma cursore", "Blocco",
      "Sottolineato", "Barra", "Cursore lampeggiante", "Avviso visivo",
      "Scorri in basso alla pressione", "Scorri in basso con l’output",
      "Consenti grassetto", "Ripristina valori", "Apri terminale di prova",
      "Salva", "Chiudi", "Preferenze salvate",
      "Le modifiche saranno usate dalle nuove finestre EssoraTerm.",
      "Ripristinare i valori?", "La configurazione personale di EssoraTerm sarà rimossa.",
      "Annulla", "Errore EssoraTerm" },
    { "pt", "Preferências do EssoraTerm", "Aparência", "Terminal", "Comportamento",
      "Fonte", "Fundo", "Texto", "Cursor", "Fundo da seleção",
      "Texto selecionado", "Pré-visualização do EssoraTerm\nutilizador@essora:~$ echo Olá",
      "Colunas", "Linhas", "Linhas do histórico", "Mostrar barra de rolagem",
      "Barra à direita", "Largura da barra", "Forma do cursor", "Bloco",
      "Sublinhado", "Barra", "Cursor intermitente", "Aviso visual",
      "Ir ao fim ao digitar", "Ir ao fim com a saída do terminal",
      "Permitir negrito", "Restaurar valores", "Abrir terminal de teste",
      "Guardar", "Fechar", "Preferências guardadas",
      "As alterações serão usadas em novas janelas do EssoraTerm.",
      "Restaurar valores?", "A configuração pessoal do EssoraTerm será removida.",
      "Cancelar", "Erro do EssoraTerm" },
    { "hu", "EssoraTerm beállítások", "Megjelenés", "Terminál", "Viselkedés",
      "Betűtípus", "Háttér", "Szöveg", "Kurzor", "Kijelölés háttere",
      "Kijelölt szöveg", "EssoraTerm előnézet\nfelhasznalo@essora:~$ echo Helló",
      "Oszlopok", "Sorok", "Előzménysorok", "Görgetősáv megjelenítése",
      "Görgetősáv jobbra", "Görgetősáv szélessége", "Kurzor alakja", "Blokk",
      "Aláhúzás", "Sáv", "Villogó kurzor", "Vizuális csengő",
      "Alulra görgetés billentyűre", "Alulra görgetés kimenetkor",
      "Félkövér engedélyezése", "Alapértékek", "Tesztterminál megnyitása",
      "Mentés", "Bezárás", "Beállítások mentve",
      "A változások az új EssoraTerm ablakokban jelennek meg.",
      "Alapértékek visszaállítása?", "A személyes EssoraTerm beállítás törlődik.",
      "Mégse", "EssoraTerm hiba" },
    { "ru", "Настройки EssoraTerm", "Внешний вид", "Терминал", "Поведение",
      "Шрифт", "Фон", "Текст", "Курсор", "Фон выделения",
      "Текст выделения", "Предпросмотр EssoraTerm\nuser@essora:~$ echo Привет",
      "Столбцы", "Строки", "Строки истории", "Показывать полосу прокрутки",
      "Полоса справа", "Ширина полосы", "Форма курсора", "Блок",
      "Подчёркивание", "Полоса", "Мигающий курсор", "Визуальный сигнал",
      "Прокручивать при вводе", "Прокручивать при выводе терминала",
      "Разрешить жирный шрифт", "Восстановить", "Открыть тестовый терминал",
      "Сохранить", "Закрыть", "Настройки сохранены",
      "Изменения будут применены к новым окнам EssoraTerm.",
      "Восстановить значения?", "Личная конфигурация EssoraTerm будет удалена.",
      "Отмена", "Ошибка EssoraTerm" },
    { "ja", "EssoraTerm の設定", "外観", "端末", "動作",
      "フォント", "背景", "文字", "カーソル", "選択範囲の背景",
      "選択範囲の文字", "EssoraTerm プレビュー\nuser@essora:~$ echo こんにちは",
      "列", "行", "履歴行数", "スクロールバーを表示",
      "右側に表示", "バーの幅", "カーソルの形", "ブロック",
      "下線", "バー", "カーソルを点滅", "視覚ベル",
      "キー入力時に最下部へ", "出力時に最下部へ",
      "太字を許可", "初期値に戻す", "テスト端末を開く",
      "保存", "閉じる", "設定を保存しました",
      "変更は新しい EssoraTerm ウィンドウで使用されます。",
      "初期値に戻しますか？", "個人用 EssoraTerm 設定を削除します。",
      "キャンセル", "EssoraTerm エラー" },
    { "zh", "EssoraTerm 设置", "外观", "终端", "行为",
      "字体", "背景", "文字", "光标", "选择背景",
      "选择文字", "EssoraTerm 预览\nuser@essora:~$ echo 你好",
      "列数", "行数", "历史行数", "显示滚动条",
      "滚动条在右侧", "滚动条宽度", "光标形状", "方块",
      "下划线", "竖线", "光标闪烁", "视觉提示",
      "按键时滚动到底部", "终端输出时滚动到底部",
      "允许粗体", "恢复默认值", "打开测试终端",
      "保存", "关闭", "设置已保存",
      "更改将在新的 EssoraTerm 窗口中使用。",
      "恢复默认值？", "将删除个人 EssoraTerm 配置。",
      "取消", "EssoraTerm 错误" },
    { "ar", "تفضيلات EssoraTerm", "المظهر", "الطرفية", "السلوك",
      "الخط", "الخلفية", "النص", "المؤشر", "خلفية التحديد",
      "نص التحديد", "معاينة EssoraTerm\nuser@essora:~$ echo مرحباً",
      "الأعمدة", "الصفوف", "أسطر السجل", "إظهار شريط التمرير",
      "الشريط على اليمين", "عرض الشريط", "شكل المؤشر", "كتلة",
      "تسطير", "شريط", "مؤشر وامض", "تنبيه مرئي",
      "التمرير للأسفل عند الكتابة", "التمرير للأسفل عند خرج الطرفية",
      "السماح بالخط العريض", "استعادة الافتراضي", "فتح طرفية اختبار",
      "حفظ", "إغلاق", "تم حفظ التفضيلات",
      "ستُستخدم التغييرات في نوافذ EssoraTerm الجديدة.",
      "استعادة القيم الافتراضية؟", "سيتم حذف إعداد EssoraTerm الشخصي.",
      "إلغاء", "خطأ EssoraTerm" }
};

typedef struct {
    GtkWidget *window;
    GtkWidget *font_button;
    GtkWidget *background;
    GtkWidget *foreground;
    GtkWidget *cursor_color;
    GtkWidget *selection_background;
    GtkWidget *selection_foreground;
    GtkWidget *preview;
    GtkWidget *columns;
    GtkWidget *rows;
    GtkWidget *scrollback;
    GtkWidget *scrollbar;
    GtkWidget *scrollbar_right;
    GtkWidget *scrollbar_width;
    GtkWidget *cursor_shape;
    GtkWidget *cursor_blink;
    GtkWidget *visual_bell;
    GtkWidget *scroll_on_key;
    GtkWidget *scroll_on_output;
    GtkWidget *allow_bold;
    char *config_path;
    const Translation *tr;
} App;

static const Translation *translation_for_locale(void)
{
    const char *locale = getenv("LANGUAGE");
    char code[3] = "en";
    size_t i;

    if (locale == NULL || *locale == '\0')
        locale = getenv("LC_ALL");
    if (locale == NULL || *locale == '\0')
        locale = getenv("LC_MESSAGES");
    if (locale == NULL || *locale == '\0')
        locale = getenv("LANG");

    if (locale != NULL && g_ascii_isalpha(locale[0]) && g_ascii_isalpha(locale[1])) {
        code[0] = g_ascii_tolower(locale[0]);
        code[1] = g_ascii_tolower(locale[1]);
    }

    for (i = 0; i < G_N_ELEMENTS(translations); ++i) {
        if (strcmp(code, translations[i].lang) == 0)
            return &translations[i];
    }
    return &translations[0];
}

static const char *home_directory(void)
{
    const char *home = g_get_home_dir();
    struct passwd *pw;

    if (home != NULL && *home != '\0')
        return home;
    pw = getpwuid(getuid());
    return (pw != NULL) ? pw->pw_dir : NULL;
}

static void set_color(GtkWidget *button, const char *value)
{
    GdkRGBA color;
    if (!gdk_rgba_parse(&color, value))
        gdk_rgba_parse(&color, "#000000");
    gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(button), &color);
}

static char *color_string(GtkWidget *button)
{
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), &color);
    return g_strdup_printf("#%02X%02X%02X",
                           (int)(color.red * 255.0 + 0.5),
                           (int)(color.green * 255.0 + 0.5),
                           (int)(color.blue * 255.0 + 0.5));
}

static void update_preview(App *app)
{
    char *background = color_string(app->background);
    char *foreground = color_string(app->foreground);
    PangoFontDescription *description;
    const char *family;
    double size;
    char *css;
    GtkCssProvider *provider;

    description = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(app->font_button));
    family = pango_font_description_get_family(description);
    size = (double)pango_font_description_get_size(description) / PANGO_SCALE;
    if (size <= 0.0)
        size = 10.0;

    css = g_strdup_printf(
        "#essoraterm-preview { background-color: %s; color: %s; "
        "font-family: '%s'; font-size: %.1fpt; padding: 16px; }",
        background, foreground, family != NULL ? family : "Monospace", size);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(app->preview),
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    g_free(css);
    g_free(background);
    g_free(foreground);
    pango_font_description_free(description);
}

static void preview_changed(GtkWidget *widget G_GNUC_UNUSED, gpointer data)
{
    update_preview((App *)data);
}

static GtkWidget *make_spin(double min, double max, double step)
{
    GtkWidget *spin = gtk_spin_button_new_with_range(min, max, step);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin), TRUE);
    gtk_widget_set_hexpand(spin, TRUE);
    return spin;
}

static void grid_row(GtkGrid *grid, int row, const char *text, GtkWidget *widget)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(grid, label, 0, row, 1, 1);
    gtk_grid_attach(grid, widget, 1, row, 1, 1);
}

static GtkWidget *new_grid(void)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);
    return grid;
}

static void set_defaults(App *app)
{
    gtk_font_chooser_set_font(GTK_FONT_CHOOSER(app->font_button), "Monospace 10");
    set_color(app->background, "#002b36");
    set_color(app->foreground, "#839496");
    set_color(app->cursor_color, "#00ffff");
    set_color(app->selection_background, "#1F7A52");
    set_color(app->selection_foreground, "#ffffff");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->columns), 95);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->rows), 25);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->scrollback), 4096);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->scrollbar), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->scrollbar_right), TRUE);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->scrollbar_width), 14);
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->cursor_shape), 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->cursor_blink), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->visual_bell), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->scroll_on_key), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->scroll_on_output), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->allow_bold), TRUE);
    update_preview(app);
}

static gboolean key_has(GKeyFile *key_file, const char *key)
{
    return g_key_file_has_key(key_file, "Settings", key, NULL);
}

static void load_config(App *app)
{
    GKeyFile *key_file = g_key_file_new();
    GError *error = NULL;
    char *text;
    char *font;
    double font_size;

    set_defaults(app);
    if (!g_key_file_load_from_file(key_file, app->config_path,
                                   G_KEY_FILE_NONE, &error)) {
        g_clear_error(&error);
        g_key_file_unref(key_file);
        return;
    }

#define LOAD_COLOR(KEY, WIDGET) \
    do { \
        if (key_has(key_file, KEY)) { \
            text = g_key_file_get_string(key_file, "Settings", KEY, NULL); \
            if (text != NULL) { set_color(WIDGET, text); g_free(text); } \
        } \
    } while (0)

    LOAD_COLOR("background", app->background);
    LOAD_COLOR("foreground", app->foreground);
    LOAD_COLOR("cursor_color", app->cursor_color);
    LOAD_COLOR("selection_background", app->selection_background);
    LOAD_COLOR("selection_foreground", app->selection_foreground);

    font = key_has(key_file, "font_family")
        ? g_key_file_get_string(key_file, "Settings", "font_family", NULL)
        : g_strdup("Monospace");
    font_size = key_has(key_file, "font_size")
        ? g_key_file_get_double(key_file, "Settings", "font_size", NULL) : 10.0;
    text = g_strdup_printf("%s %.1f", font != NULL ? font : "Monospace", font_size);
    gtk_font_chooser_set_font(GTK_FONT_CHOOSER(app->font_button), text);
    g_free(text);
    g_free(font);

#define LOAD_INT(KEY, WIDGET) \
    do { if (key_has(key_file, KEY)) \
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(WIDGET), \
            g_key_file_get_integer(key_file, "Settings", KEY, NULL)); } while (0)
#define LOAD_BOOL(KEY, WIDGET) \
    do { if (key_has(key_file, KEY)) \
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WIDGET), \
            g_key_file_get_boolean(key_file, "Settings", KEY, NULL)); } while (0)

    LOAD_INT("columns", app->columns);
    LOAD_INT("rows", app->rows);
    LOAD_INT("scrollback", app->scrollback);
    LOAD_INT("scrollbar_width", app->scrollbar_width);
    if (key_has(key_file, "cursor_shape"))
        gtk_combo_box_set_active(GTK_COMBO_BOX(app->cursor_shape),
            g_key_file_get_integer(key_file, "Settings", "cursor_shape", NULL));
    LOAD_BOOL("scrollbar", app->scrollbar);
    LOAD_BOOL("scrollbar_right", app->scrollbar_right);
    LOAD_BOOL("cursor_blink", app->cursor_blink);
    LOAD_BOOL("visual_bell", app->visual_bell);
    LOAD_BOOL("scroll_on_key", app->scroll_on_key);
    LOAD_BOOL("scroll_on_output", app->scroll_on_output);
    LOAD_BOOL("allow_bold", app->allow_bold);

#undef LOAD_COLOR
#undef LOAD_INT
#undef LOAD_BOOL
    update_preview(app);
    g_key_file_unref(key_file);
}

static gboolean save_config(App *app, GError **error)
{
    GKeyFile *key_file = g_key_file_new();
    PangoFontDescription *description;
    const char *family;
    double size;
    char *data;
    gsize length;
    char *directory;
    char *background;
    char *foreground;
    char *cursor;
    char *selection_background;
    char *selection_foreground;
    gboolean result;

    description = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(app->font_button));
    family = pango_font_description_get_family(description);
    size = (double)pango_font_description_get_size(description) / PANGO_SCALE;
    if (size <= 0.0)
        size = 10.0;

    background = color_string(app->background);
    foreground = color_string(app->foreground);
    cursor = color_string(app->cursor_color);
    selection_background = color_string(app->selection_background);
    selection_foreground = color_string(app->selection_foreground);

    g_key_file_set_string(key_file, "Settings", "background", background);
    g_key_file_set_string(key_file, "Settings", "foreground", foreground);
    g_key_file_set_string(key_file, "Settings", "cursor_color", cursor);
    g_key_file_set_string(key_file, "Settings", "selection_background", selection_background);
    g_key_file_set_string(key_file, "Settings", "selection_foreground", selection_foreground);
    g_key_file_set_string(key_file, "Settings", "font_family",
                          family != NULL ? family : "Monospace");
    g_key_file_set_double(key_file, "Settings", "font_size", size);
    g_key_file_set_integer(key_file, "Settings", "columns",
                           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->columns)));
    g_key_file_set_integer(key_file, "Settings", "rows",
                           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->rows)));
    g_key_file_set_integer(key_file, "Settings", "scrollback",
                           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->scrollback)));
    g_key_file_set_boolean(key_file, "Settings", "scrollbar",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->scrollbar)));
    g_key_file_set_boolean(key_file, "Settings", "scrollbar_right",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->scrollbar_right)));
    g_key_file_set_integer(key_file, "Settings", "scrollbar_width",
                           gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->scrollbar_width)));
    g_key_file_set_integer(key_file, "Settings", "cursor_shape",
                           gtk_combo_box_get_active(GTK_COMBO_BOX(app->cursor_shape)));
    g_key_file_set_boolean(key_file, "Settings", "cursor_blink",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->cursor_blink)));
    g_key_file_set_boolean(key_file, "Settings", "visual_bell",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->visual_bell)));
    g_key_file_set_boolean(key_file, "Settings", "scroll_on_key",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->scroll_on_key)));
    g_key_file_set_boolean(key_file, "Settings", "scroll_on_output",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->scroll_on_output)));
    g_key_file_set_boolean(key_file, "Settings", "allow_bold",
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->allow_bold)));

    data = g_key_file_to_data(key_file, &length, error);
    directory = g_path_get_dirname(app->config_path);
    if (g_mkdir_with_parents(directory, 0700) != 0) {
        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                    "Cannot create %s: %s", directory, g_strerror(errno));
        result = FALSE;
    } else {
        result = g_file_set_contents(app->config_path, data, (gssize)length, error);
    }

    g_free(directory);
    g_free(data);
    g_free(background);
    g_free(foreground);
    g_free(cursor);
    g_free(selection_background);
    g_free(selection_foreground);
    pango_font_description_free(description);
    g_key_file_unref(key_file);
    return result;
}

static void show_message(App *app, GtkMessageType type, const char *title,
                         const char *message)
{
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, type,
        GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static gboolean save_or_report(App *app)
{
    GError *error = NULL;
    if (!save_config(app, &error)) {
        show_message(app, GTK_MESSAGE_ERROR, app->tr->error_title,
                     error != NULL ? error->message : "Unknown error");
        g_clear_error(&error);
        return FALSE;
    }
    return TRUE;
}

static void save_clicked(GtkButton *button G_GNUC_UNUSED, gpointer data)
{
    App *app = data;
    if (save_or_report(app))
        show_message(app, GTK_MESSAGE_INFO, app->tr->saved_title,
                     app->tr->saved_message);
}

static void test_clicked(GtkButton *button G_GNUC_UNUSED, gpointer data)
{
    App *app = data;
    GError *error = NULL;
    gchar *arguments[] = { (gchar *)"essoraterm", NULL };

    if (!save_or_report(app))
        return;
    if (!g_spawn_async(NULL, arguments, NULL, G_SPAWN_SEARCH_PATH,
                       NULL, NULL, NULL, &error)) {
        show_message(app, GTK_MESSAGE_ERROR, app->tr->error_title,
                     error != NULL ? error->message : "Cannot start EssoraTerm");
        g_clear_error(&error);
    }
}

static void restore_clicked(GtkButton *button G_GNUC_UNUSED, gpointer data)
{
    App *app = data;
    GtkWidget *dialog;
    gint response;

    dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "%s", app->tr->restore_message);
    gtk_window_set_title(GTK_WINDOW(dialog), app->tr->restore_title);
    gtk_dialog_add_button(GTK_DIALOG(dialog), app->tr->cancel, GTK_RESPONSE_CANCEL);
    gtk_dialog_add_button(GTK_DIALOG(dialog), app->tr->restore, GTK_RESPONSE_ACCEPT);
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    if (response != GTK_RESPONSE_ACCEPT)
        return;

    if (g_remove(app->config_path) != 0 && errno != ENOENT) {
        show_message(app, GTK_MESSAGE_ERROR, app->tr->error_title,
                     g_strerror(errno));
        return;
    }
    set_defaults(app);
}

static GtkWidget *build_appearance(App *app)
{
    GtkWidget *grid = new_grid();

    app->font_button = gtk_font_button_new();
    gtk_font_chooser_set_show_preview_entry(GTK_FONT_CHOOSER(app->font_button), TRUE);
    app->background = gtk_color_button_new();
    app->foreground = gtk_color_button_new();
    app->cursor_color = gtk_color_button_new();
    app->selection_background = gtk_color_button_new();
    app->selection_foreground = gtk_color_button_new();
    app->preview = gtk_label_new(app->tr->preview);
    gtk_widget_set_name(app->preview, "essoraterm-preview");
    gtk_label_set_xalign(GTK_LABEL(app->preview), 0.0f);
    gtk_widget_set_hexpand(app->preview, TRUE);

    grid_row(GTK_GRID(grid), 0, app->tr->font, app->font_button);
    grid_row(GTK_GRID(grid), 1, app->tr->background, app->background);
    grid_row(GTK_GRID(grid), 2, app->tr->foreground, app->foreground);
    grid_row(GTK_GRID(grid), 3, app->tr->cursor_color, app->cursor_color);
    grid_row(GTK_GRID(grid), 4, app->tr->selection_background, app->selection_background);
    grid_row(GTK_GRID(grid), 5, app->tr->selection_foreground, app->selection_foreground);
    gtk_grid_attach(GTK_GRID(grid), app->preview, 0, 6, 2, 1);

    g_signal_connect(app->font_button, "font-set", G_CALLBACK(preview_changed), app);
    g_signal_connect(app->background, "color-set", G_CALLBACK(preview_changed), app);
    g_signal_connect(app->foreground, "color-set", G_CALLBACK(preview_changed), app);
    return grid;
}

static GtkWidget *build_terminal(App *app)
{
    GtkWidget *grid = new_grid();

    app->columns = make_spin(40, 300, 1);
    app->rows = make_spin(10, 100, 1);
    app->scrollback = make_spin(0, 1000000, 256);
    app->scrollbar = gtk_check_button_new_with_label(app->tr->scrollbar);
    app->scrollbar_right = gtk_check_button_new_with_label(app->tr->scrollbar_right);
    app->scrollbar_width = make_spin(6, 40, 1);

    grid_row(GTK_GRID(grid), 0, app->tr->columns, app->columns);
    grid_row(GTK_GRID(grid), 1, app->tr->rows, app->rows);
    grid_row(GTK_GRID(grid), 2, app->tr->scrollback, app->scrollback);
    gtk_grid_attach(GTK_GRID(grid), app->scrollbar, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->scrollbar_right, 0, 4, 2, 1);
    grid_row(GTK_GRID(grid), 5, app->tr->scrollbar_width, app->scrollbar_width);
    return grid;
}

static GtkWidget *build_behavior(App *app)
{
    GtkWidget *grid = new_grid();

    app->cursor_shape = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->cursor_shape), app->tr->cursor_block);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->cursor_shape), app->tr->cursor_underline);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->cursor_shape), app->tr->cursor_bar);
    app->cursor_blink = gtk_check_button_new_with_label(app->tr->cursor_blink);
    app->visual_bell = gtk_check_button_new_with_label(app->tr->visual_bell);
    app->scroll_on_key = gtk_check_button_new_with_label(app->tr->scroll_on_key);
    app->scroll_on_output = gtk_check_button_new_with_label(app->tr->scroll_on_output);
    app->allow_bold = gtk_check_button_new_with_label(app->tr->allow_bold);

    grid_row(GTK_GRID(grid), 0, app->tr->cursor_shape, app->cursor_shape);
    gtk_grid_attach(GTK_GRID(grid), app->cursor_blink, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->visual_bell, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->scroll_on_key, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->scroll_on_output, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->allow_bold, 0, 5, 2, 1);
    return grid;
}

static void build_window(App *app)
{
    GtkWidget *main_box;
    GtkWidget *notebook;
    GtkWidget *buttons;
    GtkWidget *restore;
    GtkWidget *test;
    GtkWidget *save;
    GtkWidget *close;
    GError *error = NULL;

    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), app->tr->title);
    gtk_window_set_default_size(GTK_WINDOW(app->window), 650, 540);
    gtk_window_set_position(GTK_WINDOW(app->window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(app->window), 8);
    gtk_window_set_icon_from_file(GTK_WINDOW(app->window),
                                  "/usr/share/pixmaps/essoraterm.png", &error);
    g_clear_error(&error);
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(app->window), main_box);

    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(main_box), notebook, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_appearance(app),
                             gtk_label_new(app->tr->appearance));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_terminal(app),
                             gtk_label_new(app->tr->terminal));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_behavior(app),
                             gtk_label_new(app->tr->behavior));

    buttons = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(buttons), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(buttons), 8);
    restore = gtk_button_new_with_label(app->tr->restore);
    test = gtk_button_new_with_label(app->tr->test);
    close = gtk_button_new_with_label(app->tr->close);
    save = gtk_button_new_with_label(app->tr->save);
    gtk_style_context_add_class(gtk_widget_get_style_context(save), "suggested-action");
    gtk_container_add(GTK_CONTAINER(buttons), restore);
    gtk_container_add(GTK_CONTAINER(buttons), test);
    gtk_container_add(GTK_CONTAINER(buttons), close);
    gtk_container_add(GTK_CONTAINER(buttons), save);
    gtk_box_pack_end(GTK_BOX(main_box), buttons, FALSE, FALSE, 0);

    g_signal_connect(restore, "clicked", G_CALLBACK(restore_clicked), app);
    g_signal_connect(test, "clicked", G_CALLBACK(test_clicked), app);
    g_signal_connect(save, "clicked", G_CALLBACK(save_clicked), app);
    g_signal_connect_swapped(close, "clicked", G_CALLBACK(gtk_widget_destroy), app->window);
    gtk_widget_set_can_default(save, TRUE);
    gtk_widget_grab_default(save);
}

int main(int argc, char **argv)
{
    App app;
    const char *home;

    memset(&app, 0, sizeof(app));
    setlocale(LC_ALL, "");
    gtk_init(&argc, &argv);
    app.tr = translation_for_locale();
    if (strcmp(app.tr->lang, "ar") == 0)
        gtk_widget_set_default_direction(GTK_TEXT_DIR_RTL);

    home = home_directory();
    if (home == NULL) {
        fprintf(stderr, "EssoraTerm: cannot determine the home directory\n");
        return 1;
    }
    app.config_path = g_build_filename(home, ".config", "essoraterm",
                                       "settings.conf", NULL);
    build_window(&app);
    load_config(&app);
    gtk_widget_show_all(app.window);
    gtk_main();
    g_free(app.config_path);
    return 0;
}
