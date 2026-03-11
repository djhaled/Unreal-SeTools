# Unreal C2Model Importer

Unreal Engine plugin for importing Call of Duty SEModel files (`.semodel`) with support for static/skeletal meshes, animations, and materials.

## Description

C2Model is an Unreal Engine editor plugin that enables direct import of SEModel files from Call of Duty games. It automatically handles mesh geometry, bone structures, skeletal animations, and material/texture importing with proper Unreal Engine asset pipeline integration.

### Features

- Import SEModel files (`.semodel`) directly into Unreal Engine
- Automatic detection of static vs skeletal meshes
- Animation import support
- Material and texture importing from `_images` folders
- Configurable import options via UI dialog
- Batch import support with "Import All" functionality
- Automatic texture path resolution and importing
- Support for normal maps, diffuse maps, and other material textures

## Installation

### Prerequisites

- Unreal Engine 4.27+ or 5.0+
- Visual Studio 2019 or later (Windows)

### Method 1: Project Plugin (Recommended)

1. Clone or download this repository into your project's `Plugins` folder:
   ```bash
   cd YourProject/Plugins
   git clone https://github.com/djhaled/Unreal-SeTools.git C2Model
   ```

2. Regenerate project files by right-clicking your `.uproject` file and selecting **"Generate Visual Studio project files"**

3. Build the project in Visual Studio (set to **Development Editor** configuration)

### Method 2: Engine Plugin

1. Clone into the engine's plugins directory:
   ```bash
   cd C:\Program Files\Epic Games\UE_5.X\Engine\Plugins\Marketplace
   git clone https://github.com/djhaled/Unreal-SeTools.git C2Model
   ```

2. Rebuild the engine or use the Unreal Build Tool

## Usage

### Importing a Model

1. In the Content Browser, click **Import**
2. Navigate to your `.semodel` file and select it
3. The import options dialog will appear:
   - **Mesh Type**: Choose between Auto-detect, Static Mesh, or Skeletal Mesh
   - **Import Materials**: Toggle material importing
   - **Import Textures**: Toggle texture importing
4. Click **Import** or **Import All** for batch operations

### File Structure Requirements

For automatic texture importing, place your model files with the following structure:
```
ModelFolder/
├── model.semodel
└── _images/
    ├── texture1.png
    ├── texture2_nog.tga
    └── ...
```

### Supported File Formats

- `.semodel` - SEModel mesh files
- `.seanim` - SEAnimation files (if animation support is enabled)

### Material Import Notes

- Textures ending with `_nog.TGA` are automatically configured with proper compression settings for normal maps
- Texture paths are automatically resolved relative to the `_images` folder
- Supported texture formats: PNG, TGA

## Building from Source

1. Clone the repository
2. Run the following from the plugin directory:
   ```bash
   "C:\Path\To\UnrealBuildTool.exe" -projectfiles -project="YourProject.uproject" -game -engine -progress
   ```
3. Open the generated `.sln` file in Visual Studio
4. Build with **Development Editor** configuration

## Project Structure

```
C2Model/
├── Source/
│   └── C2Model/
│       ├── Private/           # Implementation files
│       │   ├── Factories/     # Asset factories for importing
│       │   ├── Structures/    # Data structures for SEModel parsing
│       │   ├── Utils/         # Utility classes
│       │   └── Widgets/       # UI widgets for import options
│       └── Public/            # Public headers
├── Content/                   # Plugin content (if any)
└── Resources/                 # Plugin resources and icons
```

## Contributing

Contributions are welcome. Please follow the existing code style and submit pull requests with clear descriptions of changes.

## License

This project is provided as-is for educational and modding purposes. Call of Duty and related trademarks are property of their respective owners.

## Acknowledgments

- SEModel format by SE2Dev
- Unreal Engine community
