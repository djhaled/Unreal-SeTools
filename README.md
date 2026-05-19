# Unreal SeTools (C2Model)

Batch-import Call of Duty **SEModel** (`.semodel`) files into Unreal Engine—static and skeletal meshes, animations, and materials from companion `_images` folders.

This is an **editor automation plugin** for asset pipelines, not a shipped game.

## What it does

- Imports `.semodel` meshes and animations through the Content Browser
- Pulls materials and textures from `_images` directories when present
- Targets Unreal Engine 5 editor workflows on Windows

## Why it exists

Manual conversion and re-import of SEModel assets was repetitive. The plugin automates import so modding and learning projects move faster.

## Quick start

1. Clone into `YourProject/Plugins/C2Model` (folder name may match plugin layout in the repo).
2. Regenerate Visual Studio project files from the `.uproject`.
3. Build **Development Editor** for your platform.
4. In the editor: **Content Browser → Import** and select `.semodel` files.

## Requirements

- Unreal Engine 5.0+ (see `C2Model.uplugin`)
- Windows 64-bit
- Visual Studio with C++ tools

## Project status

See commit history.

## License

See repository license file.
