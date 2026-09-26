# Contributing to tesseract

Small, fast browser engine for low-end devices. Keep it lean:
no dependency you don't use, no allocation in hot paths, every Kit
testable on its own (`Tools/Debug/*`).

## Naming convention

| What | Rule | Example |
|---|---|---|
| Files, structs, classes, enums | PascalCase | `Draw.hpp`, `TextField` |
| Functions, methods | PascalCase | `DrawText`, `ParseUrl` |
| Namespaces | PascalCase | `Tess::Draw` |
| Variables, members, params | snake_case | `window_width` |
| Constants | UPPER_SNAKE | `MAX_DEPTH` |
| Enum values | UPPERCASE_ prefix from the enum name | `TK_Text` (`TokenKind`) |

No camelCase. Third-party API names (SDL, TTF) stay as-is.
