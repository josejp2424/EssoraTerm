# EssoraTerm changes

## 1.1.0-1

- Added `essoraterm-settings`, a native C/GTK3 preferences application.
- Added per-user colors, font, font size, geometry, scrollback, scrollbar,
  cursor and behavior settings.
- Added a small C launcher which reads
  `~/.config/essoraterm/settings.conf` and applies resources without `xrdb`.
- The real emulator is installed as
  `/usr/lib/essoraterm/essoraterm-bin`.
- Added **Preferences** to the translated native contextual menu.
- Added `essoraterm --preferences` and a settings desktop entry.
- Integrated the new EssoraTerm SVG, PNG and embedded XPM icon set.
- Added GTK3 development/runtime dependency detection.

## 1.0.0-3

- Added an early X11 development dependency check.
- Detects missing Xt, Xaw, Xmu, Xpm, Xft and Fontconfig headers.
- Verifies that `-lXt`, `-lXaw` and `-lXpm` can actually link, catching
  systems that contain runtime `.so.N` libraries but not development links.
- Added `scripts/install-build-deps.sh` for Debian/Devuan/Essora. It requires
  root and never invokes `sudo`.
- Improved Puppy/EssoraPup guidance: load the DEVX matching the exact ISO.

## 1.0.0-2

- Fixed `configure: error: no icons found`.
- The source patcher now installs `essoraterm.svg`, `essoraterm_48x48.png`
  and `essoraterm_48x48.xpm` into the extracted xterm icon directory before
  running `configure`.
- Added a build-time validation so configuration never starts with missing
  EssoraTerm icons.
- The final package cleanup still removes all upstream xterm/uxterm icons and
  installs only the EssoraTerm hicolor icon set.

## 1.0.0-1

- Initial EssoraTerm source package based on xterm patch #410.

## 1.0.0-4

- Added the default EssoraTerm Solarized-dark palette.
- Enabled Xft rendering with Monospace Regular, antialiasing and autohinting.
- Set the initial size to 95x25 and enabled the 8-pixel scrollbar.
- Left window placement to JWM/EssoraWM so the terminal can be centered.
- Documented why xterm.js addons cannot be loaded into native xterm.

## 1.0.0-6

- El menú contextual captura el puntero X11 mientras está abierto.
- Un clic en cualquier lugar fuera del menú ahora lo cierra sin ejecutar acciones.
- El puntero se libera mediante `XtNpopdownCallback`, incluso al seleccionar una opción.
- Se integra el nuevo archivo `/etc/X11/app-defaults/EssoraTerm` enviado por el mantenedor.
- Tamaño Xft inicial aumentado a 10.0.
- Barra de desplazamiento ubicada a la derecha y ampliada a 14 píxeles.
- El botón izquierdo permite saltar y arrastrar directamente el indicador de desplazamiento.
- Indicador de desplazamiento mínimo de 24 píxeles y colores Essora.

## 1.0.0-5

- Replaced the traditional Xaw spring-loaded context menu with a persistent
  EssoraTerm popup opened on right-button release.
- A normal right-click now opens the menu and it remains visible without
  keeping the mouse button pressed.
- Added normal pointer-motion tracking so menu entries highlight while the
  pointer moves with no button held.
- Left click activates the highlighted item; middle/right click only closes
  the popup.
- Added Essora green hover feedback and a clearer 22-pixel menu row height.
- Kept Shift+right-click for xterm's original selection extension and
  Ctrl+right-click for the original font menu.
