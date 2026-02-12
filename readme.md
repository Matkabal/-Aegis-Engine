# Engine Game — Custom C++20 Game Engine

> Experimental high-performance game engine built from scratch for study, architecture exploration and deep engine design.

This project aims to build a modern, modular and scalable game engine inspired by AAA architecture principles.

---

# 📌 Vision

The goal is to create:

- A modular C++20 engine
- Vulkan-based renderer (future)
- Lua scripting layer
- ECS-based architecture
- Custom editor
- Clean separation between runtime and tooling

This project is educational and long-term oriented.

---

# 🏗 Repository Structure

```
/
├── engine/        # Core runtime (engine source code)
├── editor/        # Game editor (will use engine as dependency)
├── apps/          # Executables that use the engine (sandbox, demos)
├── assets/        # Test assets (shaders, textures, models)
├── docs/          # Architecture documentation and decisions
│   └── adr/       # Architecture Decision Records
├── scripts/       # Helper build and utility scripts
├── third_party/   # Optional external dependencies (if not using package manager)
└── CMakeLists.txt # Root build configuration
```

---

# 🧠 Architectural Philosophy

The engine is built around strict module boundaries:

- **Engine** → Pure runtime
- **Editor** → Tooling layer (depends on engine)
- **Apps** → Example applications using engine
- **Assets** → External data only
- **Docs** → Decisions and technical documentation

### Important Rule

The engine must NEVER depend on the editor.

---

# 🧩 Modules (Planned)

Inside `/engine`:

```
engine/
├── core/        # Logging, memory, utilities
├── math/        # Vectors, matrices, transforms
├── ecs/         # Entity Component System
├── renderer/    # Rendering abstraction (Vulkan planned)
├── platform/    # Window, input, OS layer
├── runtime/     # Game loop
├── include/
└── src/
```

---

# 🛠 Technical Stack

| Component | Technology |
|------------|------------|
| Language | C++20 |
| Build System | CMake |
| Renderer (planned) | Vulkan |
| Scripting (planned) | Lua |
| Formatting | clang-format |
| Static Analysis | clang-tidy |

---

# ⚙️ Requirements

- CMake 3.25+
- C++20 compatible compiler
    - GCC 11+
    - Clang 14+
    - MSVC (Visual Studio 2022)
- (Future) Vulkan SDK

---

# 🚀 Build Instructions

## Linux / macOS

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Windows (MSVC)

```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

---

# ▶ Running Sandbox

After building:

Linux/macOS:
```bash
./apps/sandbox/sandbox
```

Windows:
```
.\apps\sandbox\Release\sandbox.exe
```

---

# 🧪 Development Workflow

Recommended:

```bash
./scripts/validate_build.sh
```

This script validates the M0 build workflow end-to-end:

- Cleans `build/debug` and `build/release`
- Configures + builds Debug
- Configures + builds Release
- Runs `sandbox` in both configurations

Useful options:

```bash
./scripts/validate_build.sh --skip-run  # Build only
./scripts/validate_build.sh --no-clean  # Reuse existing build folders
```

This makes build checks reproducible before moving to the next milestone.

---

# 📐 Code Standards

- C++20 mandatory
- Warnings enabled at high level
- Zero warnings policy
- No implicit conversions
- No raw new/delete outside memory module
- RAII everywhere

---

# 🎨 Formatting

This project uses **clang-format**.

To format manually:

```bash
clang-format -i engine/**/*.cpp engine/**/*.hpp
```

---

# 🔍 Static Analysis

This project is prepared for **clang-tidy**.

Example usage:

```bash
clang-tidy engine/src/*.cpp -- -std=c++20
```

Warnings may be treated as errors in the future.

---

# 📜 Architecture Decision Records (ADR)

All major technical decisions are documented under:

```
docs/adr/
```

Each ADR contains:

- Context
- Decision
- Consequences

This prevents architectural drift.

---

# 📦 Dependency Management

Two strategies supported:

1. vcpkg (recommended)
2. Conan
3. Manual third_party (only if necessary)

Decision will be documented as ADR.

---

# 🧭 Roadmap (High-Level)

- [x] M0 — Repository foundation
- [ ] M1 — Window + main loop
- [ ] M2 — Vulkan initialization
- [ ] M3 — Basic renderer
- [ ] M4 — ECS implementation
- [ ] M5 — Lua integration
- [ ] M6 — Editor base

---

# 🛡 Project Principles

- Clean module boundaries
- No circular dependencies
- Engine independent of editor
- Test via sandbox app
- Document architectural decisions
- Prefer composition over inheritance
- Explicit over implicit

---

# 📚 Documentation

Architecture overview:
```
docs/architecture.md
```

Build guide:
```
docs/build.md
```

ADRs:
```
docs/adr/
```

---

# 🎯 Long-Term Goals

- AAA-like architecture quality
- Clear separation between engine and game logic
- High-performance renderer
- Scriptable gameplay layer
- Visual editor for non-programmers

---

# 🧑‍💻 Author

Personal long-term research project.

---

# ⚠ Status

Early foundation stage (M0).
APIs are unstable and subject to change.

