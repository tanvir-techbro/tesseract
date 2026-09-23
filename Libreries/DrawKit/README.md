# DrawKit

Primitives and widgets for SDL rendering. No HTML knowledge —
`Tess::Draw` draws at coordinates; upper Kits decide what goes where.

## Primitives (`Fill.cpp`, `Outline.cpp`)

| Function | Does |
|---|---|
| `FillCircle(r, cx, cy, radius)` | Batched scanline fill, one draw call |
| `FillRoundedRect(r, rect, radius)` | Fill + corner circles |
| `DrawArc(r, cx, cy, radius, start, end)` | Arc dots, degrees (0=east, 90=south, y-down) |
| `DrawRoundedOutline(r, rect, radius)` | 4 edges + 4 arcs |
| `DrawText(r, txt, str, x, y)` | Set string + draw (`TTF_Text` owned by caller) |
| `DrawCaret(r, txt, x, y)` | Blinking caret, 530ms phase |
| `TextSize(txt, w, h)` | Measure current string |

Conventions: text color lives on `TTF_Text` (caller sets once),
caret color is the current SDL draw color (caller sets before calling).

## Widgets (`Widget.cpp`, planned)

Reusable `TextField` so text boxes are never copy-pasted:

```cpp
struct TextField {
      std::string value;   // contents
      size_t cursor = 0;   // caret offset into value (chars, ASCII for now)
      long selAnchor = -1; // selection anchor, -1 = none
      bool focused = false;
      float scrollX = 0;   // horizontal scroll for overflow
};
void FieldEvent(TextField &f, const SDL_Event &e, SDL_FRect box);
void FieldDraw(TextField &f, SDL_Renderer *r, TTF_Text *txt, SDL_FRect box);
```

Behavior contract:

* **Focus**: click inside `box` focuses, outside unfocuses. Only the
  focused field takes `TEXT_INPUT` / `KEY_DOWN`.
* **Scroll**: text wider than the box scrolls (`scrollX`), overflow is
  clipped to the box. Caret always visible while typing.
* **Move**: Left/Right = step, Home/End = edges, all with Shift to
  extend the selection.
* **Edit**: typing replaces selection (or inserts at caret), Backspace /
  Delete remove selection (or one char), Enter = submit (caller reads
  `value`, field keeps it).
* **Clipboard**: Ctrl+C/X copy/cut selection via `SDL_SetClipboardText`,
  Ctrl+V inserts `SDL_GetClipboardText`. Ctrl+A selects all.
* **Draw**: highlight rect behind selection (substring measure),
  then text, then caret when focused and selection is empty.

The URL bar in `Src/Tess.cpp` is instance #1. Future search boxes
and dialogs reuse the struct untouched.

## Non-goals

No layout, no HTML/CSS types, no per-frame allocation in draw paths
beyond text shaping (see RenderKit for caching).
