#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import shutil
import sys
from pathlib import Path


class PatchError(RuntimeError):
    pass


def read(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except OSError as exc:
        raise PatchError(f"cannot read {path}: {exc}") from exc


def write(path: Path, data: str) -> None:
    path.write_text(data, encoding="utf-8")


def replace_once(data: str, old: str, new: str, label: str) -> str:
    count = data.count(old)
    if count != 1:
        raise PatchError(
            f"{label}: expected exactly one match, found {count}. "
            "Use the xterm-410 source or update the patcher."
        )
    return data.replace(old, new, 1)


def regex_once(data: str, pattern: str, replacement: str, label: str) -> str:
    updated, count = re.subn(pattern, replacement, data, count=1,
                             flags=re.MULTILINE)
    if count != 1:
        raise PatchError(
            f"{label}: expected exactly one regex match, found {count}. "
            "Use the xterm-410 source or update the patcher."
        )
    return updated


def patch_ptyx(path: Path) -> None:
    data = read(path)
    data = replace_once(
        data,
        "\t,fontMenu\n#if OPT_TEK4014",
        "\t,fontMenu\n\t,editMenu\n#if OPT_TEK4014",
        "ptyx.h MenuIndex",
    )
    data = replace_once(
        data,
        "#define NUM_POPUP_MENUS 4",
        "#define NUM_POPUP_MENUS 5",
        "ptyx.h NUM_POPUP_MENUS",
    )
    write(path, data)


def patch_menu_h(path: Path) -> None:
    data = read(path)
    data = replace_once(
        data,
        "extern MenuEntry fontMenuEntries[];\n",
        "extern MenuEntry fontMenuEntries[];\n"
        "extern MenuEntry editMenuEntries[];\n",
        "menu.h edit menu declaration",
    )
    data = replace_once(
        data,
        "extern void HandlePopupMenu        PROTO_XT_ACTIONS_ARGS;\n",
        "extern void HandlePopupMenu        PROTO_XT_ACTIONS_ARGS;\n"
        "extern void HandleEssoraPopupEditMenu PROTO_XT_ACTIONS_ARGS;\n",
        "menu.h sticky popup declaration",
    )
    write(path, data)


def patch_button(path: Path) -> None:
    data = read(path)
    marker = "struct _SelectionList {"
    implementation = r'''
/*
 * EssoraTerm: select the visible screen and saved scrollback.
 */
void
HandleEssoraSelectAll(Widget w,
                      XEvent *event GCC_UNUSED,
                      String *params GCC_UNUSED,
                      Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;
    CELL first;
    CELL last;

    if ((xw = getXtermWidget(w)) != NULL) {
        TScreen *screen = TScreenOf(xw);

        memset(&first, 0, sizeof(first));
        memset(&last, 0, sizeof(last));
        screen->selectUnit = Select_ALL;
        ComputeSelect(xw, &first, &last, False, True);
    }
}

'''
    data = replace_once(
        data, marker, implementation + marker,
        "button.c EssoraTerm select-all action",
    )
    write(path, data)


def patch_charproc(path: Path) -> None:
    data = read(path)
    data = regex_once(
        data,
        r'^(?P<entry>[ \t]*\{[ \t]*"popup-menu",[ \t]*HandlePopupMenu[ \t]*\},[ \t]*\n)',
        r'\g<entry>    { "essoraterm-popup-edit-menu", HandleEssoraPopupEditMenu },\n',
        "charproc.c sticky EssoraTerm menu action",
    )
    write(path, data)


def patch_menu_c(path: Path) -> None:
    data = read(path)

    data = replace_once(
        data,
        "#include <xstrings.h>\n",
        "#include <xstrings.h>\n#include <essoraterm_i18n.h>\n",
        "menu.c i18n include",
    )
    data = replace_once(
        data,
        "#include <signal.h>\n",
        "#include <signal.h>\n#include <unistd.h>\n\n"
        "extern void HandleEssoraSelectAll PROTO_XT_ACTIONS_ARGS;\n",
        "menu.c select-all declaration",
    )
    data = replace_once(
        data,
        "static void do_vtfont          PROTO_XT_CALLBACK_ARGS;\n",
        "static void do_vtfont          PROTO_XT_CALLBACK_ARGS;\n"
        "static void do_essora_copy     PROTO_XT_CALLBACK_ARGS;\n"
        "static void do_essora_paste    PROTO_XT_CALLBACK_ARGS;\n"
        "static void do_essora_select_all PROTO_XT_CALLBACK_ARGS;\n"
        "static void do_essora_clear    PROTO_XT_CALLBACK_ARGS;\n"
        "static void do_essora_preferences PROTO_XT_CALLBACK_ARGS;\n",
        "menu.c EssoraTerm callback declarations",
    )

    edit_entries = r'''
MenuEntry editMenuEntries[] = {
    { "copy",          do_essora_copy,       NULL },
    { "paste",         do_essora_paste,      NULL },
    { "selectAll",     do_essora_select_all, NULL },
    { "line1",         NULL,                 NULL },
    { "preferences",   do_essora_preferences, NULL },
    { "line2",         NULL,                 NULL },
    { "clearTerminal", do_essora_clear,      NULL }
};

'''
    data = replace_once(
        data,
        "#if OPT_TEK4014\nMenuEntry tekMenuEntries[] = {",
        edit_entries + "#if OPT_TEK4014\nMenuEntry tekMenuEntries[] = {",
        "menu.c editMenuEntries",
    )
    data = replace_once(
        data,
        "    DATA( fontMenu),\n#if OPT_TEK4014",
        "    DATA( fontMenu),\n"
        "    DATA( editMenu),\n"
        "#if OPT_TEK4014",
        "menu.c menu_names",
    )
    data = regex_once(
        data,
        r"(^[ \t]*\+ XtNumber\(fontMenuEntries\)\n)(#if OPT_TEK4014)",
        r"\1\t\t\t  + XtNumber(editMenuEntries)\n\2",
        "menu.c unusedEntries allocation",
    )
    data = replace_once(
        data,
        "    case fontMenu:\n\tbreak;\n#if OPT_TEK4014",
        "    case fontMenu:\n"
        "\tbreak;\n"
        "    case editMenu:\n"
        "\tbreak;\n"
        "#if OPT_TEK4014",
        "menu.c unusedEntries switch",
    )

    label_code = r'''
                if (num == editMenu && entries[n].function != NULL) {
                    const char *label =
                        essoraterm_menu_label(entries[n].name);
                    if (label != NULL) {
                        XtVaSetValues(entries[n].widget,
                                      XtNlabel, label,
                                      (XtPointer) 0);
                    }
                }
'''
    data = replace_once(
        data,
        "\t\tlist->entries++;\n",
        label_code + "\t\tlist->entries++;\n",
        "menu.c translated menu labels",
    )
    data = replace_once(
        data,
        "    case 'f':\n\tme = fontMenu;\n\tbreak;\n",
        "    case 'f':\n"
        "\tme = fontMenu;\n"
        "\tbreak;\n"
        "    case 'e':\n"
        "\tme = editMenu;\n"
        "\tbreak;\n",
        "menu.c indexOfMenu",
    )
    data = replace_once(
        data,
        "    case fontMenu:\n\tif (created) {",
        "    case editMenu:\n"
        "\tbreak;\n\n"
        "    case fontMenu:\n"
        "\tif (created) {",
        "menu.c domenu edit switch",
    )

    callbacks = r'''
/*
 * EssoraTerm contextual menu.
 *
 * Xt menu callbacks do not receive the original pointer event. Use a
 * synthetic key event with the X server's last timestamp. This also avoids
 * forwarding a right-button event to terminal programs using mouse mode.
 */
static void
essoraterm_clipboard_event(XEvent *event)
{
    Display *display = XtDisplay((Widget) term);

    memset(event, 0, sizeof(*event));
    event->xkey.type = KeyPress;
    event->xkey.display = display;
    event->xkey.window = XtWindow((Widget) term);
    event->xkey.root = RootWindowOfScreen(XtScreen((Widget) term));
    event->xkey.time = XtLastTimestampProcessed(display);
    event->xkey.same_screen = True;
}

static void
do_essora_copy(Widget gw GCC_UNUSED,
               XtPointer closure GCC_UNUSED,
               XtPointer data GCC_UNUSED)
{
    XEvent event;
    String params[] = { (String) "CLIPBOARD" };
    Cardinal count = XtNumber(params);

    essoraterm_clipboard_event(&event);
    HandleCopySelection((Widget) term, &event, params, &count);
}

static void
do_essora_paste(Widget gw GCC_UNUSED,
                XtPointer closure GCC_UNUSED,
                XtPointer data GCC_UNUSED)
{
    XEvent event;
    String params[] = {
        (String) "CLIPBOARD",
        (String) "PRIMARY"
    };
    Cardinal count = XtNumber(params);

    essoraterm_clipboard_event(&event);
    HandleInsertSelection((Widget) term, &event, params, &count);
}

static void
do_essora_select_all(Widget gw GCC_UNUSED,
                     XtPointer closure GCC_UNUSED,
                     XtPointer data GCC_UNUSED)
{
    XEvent event;
    Cardinal count = 0;

    essoraterm_clipboard_event(&event);
    HandleEssoraSelectAll((Widget) term, &event, NULL, &count);
}

static void
do_essora_clear(Widget gw GCC_UNUSED,
                XtPointer closure GCC_UNUSED,
                XtPointer data GCC_UNUSED)
{
    VTReset(term, True, True);
}

static void
do_essora_preferences(Widget gw GCC_UNUSED,
                      XtPointer closure GCC_UNUSED,
                      XtPointer data GCC_UNUSED)
{
    pid_t child = fork();

    if (child == 0) {
        execl("/usr/bin/essoraterm-settings",
              "essoraterm-settings", (char *) 0);
        _exit(127);
    }
}

'''
    data = replace_once(
        data,
        "static void\ndo_redraw(Widget gw GCC_UNUSED,",
        callbacks + "static void\ndo_redraw(Widget gw GCC_UNUSED,",
        "menu.c EssoraTerm callback implementation",
    )

    sticky_popup = r'''
/*
 * EssoraTerm: open the edit menu as a persistent popup.
 *
 * XtGrabExclusive only controls Xt's widget dispatch; it does not perform an
 * XGrabPointer after the right button has already been released.  Without a
 * real pointer grab, clicks outside the popup never reach it and cannot close
 * it.  Grab the X pointer explicitly and release it from the shell popdown
 * callback.
 */
static Widget essoraterm_pointer_menu;
static Widget essoraterm_callback_menu;

static void
essoraterm_release_pointer(Widget menu,
                           XtPointer closure GCC_UNUSED,
                           XtPointer data GCC_UNUSED)
{
    Display *display = XtDisplay(menu);

    if (essoraterm_pointer_menu == menu) {
        XUngrabPointer(display, CurrentTime);
        essoraterm_pointer_menu = NULL;
        XFlush(display);
    }
}

static Time
essoraterm_event_time(const XEvent *event)
{
    if (event != NULL) {
        switch (event->type) {
        case ButtonPress:
        case ButtonRelease:
            return event->xbutton.time;
        case KeyPress:
        case KeyRelease:
            return event->xkey.time;
        default:
            break;
        }
    }
    return CurrentTime;
}

void
HandleEssoraPopupEditMenu(Widget w,
                          XEvent *event,
                          String *params GCC_UNUSED,
                          Cardinal *param_count GCC_UNUSED)
{
    String edit_params[] = { (String) "editMenu" };
    Cardinal edit_count = XtNumber(edit_params);
    Widget menu;
    Display *display;
    int grab_status;

    if (!domenu(w, event, edit_params, &edit_count))
        return;

    menu = obtain_menu(w, editMenu);
    if (menu == NULL)
        return;

    XtCallActionProc(w, "XawPositionSimpleMenu", event,
                     edit_params, edit_count);

    if (essoraterm_callback_menu != menu) {
        XtAddCallback(menu, XtNpopdownCallback,
                      essoraterm_release_pointer, NULL);
        essoraterm_callback_menu = menu;
    }

    XtPopup(menu, XtGrabExclusive);
    display = XtDisplay(menu);

    /* Ensure the override-redirect shell is viewable before grabbing it. */
    XSync(display, False);
    grab_status = XGrabPointer(display,
                               XtWindow(menu),
                               False,
                               ButtonPressMask | ButtonReleaseMask |
                               PointerMotionMask | EnterWindowMask |
                               LeaveWindowMask,
                               GrabModeAsync,
                               GrabModeAsync,
                               None,
                               None,
                               essoraterm_event_time(event));

    if (grab_status != GrabSuccess) {
        XtPopdown(menu);
        XBell(display, 0);
        return;
    }

    essoraterm_pointer_menu = menu;
}

'''
    data = replace_once(
        data,
        "/*\n * private interfaces - keep out!\n */\n",
        sticky_popup + "/*\n * private interfaces - keep out!\n */\n",
        "menu.c persistent popup handler",
    )
    write(path, data)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path, help="extracted xterm-410 source")
    parser.add_argument(
        "--project-root", type=Path,
        default=Path(__file__).resolve().parents[1],
    )
    args = parser.parse_args()
    source = args.source.resolve()
    project = args.project_root.resolve()

    required = ["ptyx.h", "menu.h", "menu.c", "button.c", "charproc.c", "configure"]
    missing = [name for name in required if not (source / name).is_file()]
    if missing:
        raise PatchError("missing source files: " + ", ".join(missing))

    version_h = source / "version.h"
    if version_h.exists() and "410" not in read(version_h):
        raise PatchError("this patch set is intended for xterm patch #410")

    icons_dir = source / "icons"
    icons_dir.mkdir(parents=True, exist_ok=True)

    shutil.copy2(project / "patches" / "essoraterm_i18n.h",
                 source / "essoraterm_i18n.h")

    # Replace xterm's compiled fallback icon with the EssoraTerm design.
    shutil.copy2(project / "assets" / "mini.xterm_48x48.xpm",
                 icons_dir / "mini.xterm_48x48.xpm")

    # xterm's configure script validates --with-icon-name against files in
    # source/icons.  Install our own icon there before configure runs.
    # The package staging step later removes every upstream xterm icon and
    # installs only the EssoraTerm hicolor set from assets/icons.
    shutil.copy2(
        project / "assets" / "icons" / "hicolor" / "scalable" / "apps" / "essoraterm.svg",
        icons_dir / "essoraterm.svg",
    )
    shutil.copy2(
        project / "assets" / "icons" / "hicolor" / "48x48" / "apps" / "essoraterm.png",
        icons_dir / "essoraterm_48x48.png",
    )
    shutil.copy2(project / "assets" / "mini.xterm_48x48.xpm",
                 icons_dir / "essoraterm_48x48.xpm")

    patch_ptyx(source / "ptyx.h")
    patch_menu_h(source / "menu.h")
    patch_button(source / "button.c")
    patch_charproc(source / "charproc.c")
    patch_menu_c(source / "menu.c")

    (source / ".essoraterm-patched").write_text(
        "EssoraTerm patch set 4 for xterm-410\n", encoding="utf-8"
    )
    print(f"EssoraTerm patch applied successfully to {source}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except PatchError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
