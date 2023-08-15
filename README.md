# Unreal-SeTools Plugin

Unreal-SeTools is a plugin for Unreal Engine 4.27 that allows importing Call of Duty `.semodel` and `.seanim` files into Unreal Engine as static or skeletal meshes and animation sequences.


#
## Features

- Import `.semodel` files as static or skeletal meshes.
- Import `.seanim` files as animation sequences.
- Automatic texture importing and linking.

## Installation

1. Download or clone the Unreal-SeTools repository.
2. Copy the plugin's folder to your project's `Plugins` directory.


## Compatibility

| Unreal Engine Version | `.semodel` Compatibility | `.seanim` Compatibility |
|-----------------------|-------------------------|-------------------------|
| 4.27                   | :heavy_check_mark:      | :heavy_check_mark:      |
| 5.3 (Separate Branch)                  | :heavy_check_mark:      | :x: 
## Usage

1. After installing the plugin, open your Unreal Engine project.
2. Navigate to the "File" menu and select "Import" to open the import dialog.
3. Choose the `.semodel` or `.seanim` file you want to import.
4. Configure import options.
5. Click "Import".

## Code Overview

The plugin contains two main factory classes: `UC2ModelAssetFactory` and `UC2AnimAssetFactory`. These factories handle the creation and import of `.semodel` and `.seanim` files, respectively.

### UC2ModelAssetFactory

- Import `.semodel` files and create static or skeletal meshes.
- Uses `C2Mesh` and `C2Material` classes to parse mesh data and material information.
- Automatically imports and links textures.

### UC2AnimAssetFactory

- Import `.seanim` files and create animation sequences.
- Uses `C2Anim` class to parse animation data.
- Creates animation tracks based on bone information.

## Contributing

Contributions are welcome! If you encounter issues or have suggestions for improvements, feel free to open an issue or submit a pull request on the [GitHub repository](https://github.com/djhaled/Unreal-SeTools).

## License

This plugin is released under the [MIT License](https://opensource.org/licenses/MIT).

## Support

For questions or support regarding the Unreal-SeTools plugin, you can reach out to me on discord "bkhimothy". 

