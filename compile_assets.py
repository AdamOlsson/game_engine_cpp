#!/usr/bin/env python3
"""
Asset Compiler Script

Automatically discovers and compiles shader, image, and font assets into C++ resource files.
Generates both .cpp implementation files and .h header files for each asset.

Usage:
    python compile_assets.py [--verbose] [--dry-run]

Directory Structure:
    - Shaders: src/game_engine_sdk/render_engine/shaders/{vertex,fragment}/*.spv
    - Images: assets/images/*.{png,jpg,jpeg,bmp,tga,gif}
    - Fonts: assets/fonts/*.{png,bmp,tga} (each with individual .json config)

Output:
    - Shaders: src/game_engine_sdk/render_engine/resources/shaders/{vertex,fragment}/{name}/
    - Images: src/game_engine_sdk/render_engine/resources/images/{name}/
    - Fonts: src/game_engine_sdk/render_engine/resources/fonts/{name}/
"""

import argparse
import json
import logging
import os
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(levelname)s: %(message)s'
)
logger = logging.getLogger(__name__)

# Asset type constants
class AssetType:
    SHADER = "ShaderResource"
    IMAGE = "ImageResource"
    FONT = "FontResource"

@dataclass
class Asset:
    """Represents an asset to be compiled."""
    filename: str
    source_dir: Path
    output_cpp: Path
    output_header: Path
    resource_type: str
    resource_name: str
    atlas_dims: Optional[Tuple[int, int]] = None
    char_dims: Optional[Tuple[int, int]] = None

    @property
    def source_path(self) -> Path:
        """Full path to the source file."""
        return self.source_dir / self.filename

    @property
    def base_name(self) -> str:
        """Base filename without extension."""
        return self.source_path.stem

    @property
    def variable_name(self) -> str:
        """Variable name for xxd output."""
        return self.filename.replace(".", "_")

class FontConfig:
    """Manages font configuration from individual JSON files."""
    
    DEFAULT_ATLAS_SIZE = (512, 512)
    DEFAULT_CHAR_SIZE = (64, 64)
    
    def _get_config_path(self, font_path: Path) -> Path:
        """Get the config file path for a font file."""
        # For font.png, look for font.json in the same directory
        return font_path.parent / f"{font_path.stem}.json"
    
    def _load_config(self, config_path: Path) -> Dict:
        """Load font configuration from JSON file."""
        if not config_path.exists():
            logger.info(f"Font config {config_path} not found. Creating example.")
            self._create_example_config(config_path)
            return {}
        
        try:
            with open(config_path, 'r') as f:
                config = json.load(f)
                logger.debug(f"Loaded font config from {config_path}")
                return config
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in {config_path}: {e}")
            return {}
        except Exception as e:
            logger.error(f"Error loading font config from {config_path}: {e}")
            return {}
    
    def _create_example_config(self, config_path: Path):
        """Create an example font configuration file."""
        example_config = {
            "atlas_width": 512,
            "atlas_height": 512,
            "char_width": 64,
            "char_height": 64
        }
        
        try:
            config_path.parent.mkdir(parents=True, exist_ok=True)
            with open(config_path, 'w') as f:
                json.dump(example_config, f, indent=2)
            logger.info(f"Created example font config at {config_path}")
        except Exception as e:
            logger.error(f"Could not create example font config: {e}")
    
    def get_dimensions(self, font_path: Path) -> Tuple[Tuple[int, int], Tuple[int, int]]:
        """Get atlas and character dimensions for a font file."""
        config_path = self._get_config_path(font_path)
        config = self._load_config(config_path)
        
        if config:
            atlas_dims = (
                config.get("atlas_width", self.DEFAULT_ATLAS_SIZE[0]),
                config.get("atlas_height", self.DEFAULT_ATLAS_SIZE[1])
            )
            char_dims = (
                config.get("char_width", self.DEFAULT_CHAR_SIZE[0]),
                config.get("char_height", self.DEFAULT_CHAR_SIZE[1])
            )
            return atlas_dims, char_dims
        else:
            logger.warning(f"No config found for {font_path.name}, using defaults")
            return self.DEFAULT_ATLAS_SIZE, self.DEFAULT_CHAR_SIZE

class AssetDiscovery:
    """Discovers assets in the filesystem."""
    
    # File extensions for each asset type
    IMAGE_EXTENSIONS = ['.png', '.jpg', '.jpeg', '.bmp', '.tga', '.gif']
    FONT_EXTENSIONS = ['.png', '.bmp', '.tga']
    
    def __init__(self):
        self.font_config = FontConfig()
    
    def _to_resource_name(self, base_name: str, suffix: str = "") -> str:
        """Convert filename to PascalCase resource name."""
        # Split on underscores and capitalize each word
        words = base_name.split('_')
        pascal_case = ''.join(word.capitalize() for word in words)
        return pascal_case + suffix
    
    def find_shaders(self) -> List[Asset]:
        """Find all shader assets."""
        assets = []
        
        # Vertex shaders
        vertex_dir = Path("src/game_engine_sdk/render_engine/shaders/vertex")
        if vertex_dir.exists():
            for spv_file in vertex_dir.glob("*.spv"):
                resource_name = self._to_resource_name(spv_file.stem, "Vertex")
                output_dir = Path(f"src/game_engine_sdk/render_engine/resources/shaders/vertex/{spv_file.stem}")
                
                assets.append(Asset(
                    filename=spv_file.name,
                    source_dir=vertex_dir,
                    output_cpp=output_dir / f"{spv_file.stem}.cpp",
                    output_header=output_dir / f"{spv_file.stem}.h",
                    resource_type=AssetType.SHADER,
                    resource_name=resource_name
                ))
        
        # Fragment shaders
        fragment_dir = Path("src/game_engine_sdk/render_engine/shaders/fragment")
        if fragment_dir.exists():
            for spv_file in fragment_dir.glob("*.spv"):
                resource_name = self._to_resource_name(spv_file.stem, "Fragment")
                output_dir = Path(f"src/game_engine_sdk/render_engine/resources/shaders/fragment/{spv_file.stem}")
                
                assets.append(Asset(
                    filename=spv_file.name,
                    source_dir=fragment_dir,
                    output_cpp=output_dir / f"{spv_file.stem}.cpp",
                    output_header=output_dir / f"{spv_file.stem}.h",
                    resource_type=AssetType.SHADER,
                    resource_name=resource_name
                ))
        
        return assets
    
    def find_images(self) -> List[Asset]:
        """Find all image assets."""
        assets = []
        images_dir = Path("assets/images")
        
        if not images_dir.exists():
            logger.warning(f"Images directory {images_dir} does not exist")
            return assets
        
        for ext in self.IMAGE_EXTENSIONS:
            for image_file in images_dir.glob(f"*{ext}"):
                resource_name = self._to_resource_name(image_file.stem, "Image")
                output_dir = Path(f"src/game_engine_sdk/render_engine/resources/images/{image_file.stem}")
                
                assets.append(Asset(
                    filename=image_file.name,
                    source_dir=images_dir,
                    output_cpp=output_dir / f"{image_file.stem}.cpp",
                    output_header=output_dir / f"{image_file.stem}.h",
                    resource_type=AssetType.IMAGE,
                    resource_name=resource_name
                ))
        
        return assets
    
    def find_fonts(self) -> List[Asset]:
        """Find all font assets."""
        assets = []
        fonts_base_dir = Path("assets/fonts")
        
        if not fonts_base_dir.exists():
            logger.warning(f"Fonts directory {fonts_base_dir} does not exist")
            return assets
        
        # Iterate through each font subdirectory
        for font_dir in fonts_base_dir.iterdir():
            if not font_dir.is_dir():
                continue
                
            for ext in self.FONT_EXTENSIONS:
                for font_file in font_dir.glob(f"*{ext}"):
                    # Skip JSON config files
                    if font_file.suffix == ".json":
                        logger.debug(f"Skipping {font_file}")
                        continue
                    logger.debug(f"Reading {font_file}") 
                    resource_name = self._to_resource_name(font_file.stem, "Font")
                    output_dir = Path(f"src/game_engine_sdk/render_engine/resources/fonts/{font_file.stem}")
                    
                    atlas_dims, char_dims = self.font_config.get_dimensions(font_file)
                    
                    assets.append(Asset(
                        filename=font_file.name,
                        source_dir=font_dir,  # Use the specific font directory
                        output_cpp=output_dir / f"{font_file.stem}.cpp",
                        output_header=output_dir / f"{font_file.stem}.h",
                        resource_type=AssetType.FONT,
                        resource_name=resource_name,
                        atlas_dims=atlas_dims,
                        char_dims=char_dims
                    ))
        
        return assets

class AssetCompiler:
    """Compiles assets into C++ resource files."""
    
    # Template for C++ implementation files
    CPP_TEMPLATE = '''\
/// This file is auto generated by compile_assets.py
#include "{header_name}.h"

namespace {{
std::string resource_name = "{resource_name}";

{xxd_content}
}} // namespace

{create_function}
'''
    
    # Template for shader header files
    SHADER_HEADER_TEMPLATE = '''\
#pragma once
#include "graphics_pipeline/ShaderResource.h"
#include <memory>

class {class_name} {{
  public:
  static std::unique_ptr<graphics_pipeline::ShaderResource> create_resource();
}};
'''
    
    # Template for image header files
    IMAGE_HEADER_TEMPLATE = '''\
#pragma once
#include "game_engine_sdk/render_engine/resources/images/ImageResource.h"
#include <memory>

class {class_name} {{
  public:
    static std::unique_ptr<ImageResource> create_resource();
}};
'''
    
    # Template for font header files
    FONT_HEADER_TEMPLATE = '''\
#pragma once
#include "game_engine_sdk/render_engine/resources/fonts/FontResource.h"
#include <memory>

class {class_name} {{
  public:
    static std::unique_ptr<FontResource> create_resource();
}};
'''
    
    # Template for standard resource creation function
    STANDARD_CREATE_TEMPLATE = '''\
std::unique_ptr<{resource_type}> {class_name}::create_resource() {{
    return {resource_type}Builder()
        .name(std::move(resource_name))
        .length({var_name}_len)
        .bytes(&{var_name}[0])
        .build();
}}'''
    
    # Template for font resource creation function
    FONT_CREATE_TEMPLATE = '''\
std::unique_ptr<{resource_type}> {class_name}::create_resource() {{
    return {resource_type}Builder()
        .name(std::move(resource_name))
        .length({var_name}_len)
        .bytes(&{var_name}[0])
        .atlas_width({atlas_width})
        .atlas_height({atlas_height})
        .char_width({char_width})
        .char_height({char_height})
        .build();
}}'''
    
    def _run_xxd(self, asset: Asset) -> str:
        """Run xxd command to convert binary file to C array."""
        original_cwd = os.getcwd()
        
        try:
            # Change to source directory
            os.chdir(asset.source_dir)
            
            # Run xxd command
            result = subprocess.run(
                ['xxd', '-i', asset.filename],
                capture_output=True,
                text=True,
                check=True
            )
            
            return result.stdout
            
        except subprocess.CalledProcessError as e:
            logger.error(f"xxd failed for {asset.filename}: {e}")
            return f"// Error: xxd failed with return code {e.returncode}"
        except FileNotFoundError:
            logger.error("xxd command not found. Please install xxd.")
            sys.exit(1)
        finally:
            os.chdir(original_cwd)
    
    def _get_header_template(self, asset_type: str) -> str:
        """Get the appropriate header template for the asset type."""
        if asset_type == AssetType.SHADER:
            return self.SHADER_HEADER_TEMPLATE
        elif asset_type == AssetType.IMAGE:
            return self.IMAGE_HEADER_TEMPLATE
        elif asset_type == AssetType.FONT:
            return self.FONT_HEADER_TEMPLATE
        else:
            raise ValueError(f"Unknown asset type: {asset_type}")
    
    def _generate_create_function(self, asset: Asset) -> str:
        """Generate the create_resource function for the asset."""
        if asset.resource_type == AssetType.FONT:
            return self.FONT_CREATE_TEMPLATE.format(
                resource_type=asset.resource_type,
                class_name=asset.resource_name,
                var_name=asset.variable_name,
                atlas_width=asset.atlas_dims[0],
                atlas_height=asset.atlas_dims[1],
                char_width=asset.char_dims[0],
                char_height=asset.char_dims[1]
            )
        elif asset.resource_type == AssetType.SHADER:
            return self.STANDARD_CREATE_TEMPLATE.format(
                    resource_type="graphics_pipeline::" + asset.resource_type,
                class_name=asset.resource_name,
                var_name=asset.variable_name
            )
        else:
            return self.STANDARD_CREATE_TEMPLATE.format(
                resource_type=asset.resource_type,
                class_name=asset.resource_name,
                var_name=asset.variable_name
            )
    
    def compile_asset(self, asset: Asset, dry_run: bool = False) -> bool:
        """Compile a single asset."""
        try:
            logger.info(f"Compiling {asset.filename} -> {asset.resource_name}")
            
            if not asset.source_path.exists():
                logger.error(f"Source file not found: {asset.source_path}")
                return False
            
            if dry_run:
                logger.info(f"[DRY RUN] Would generate: {asset.output_cpp}")
                logger.info(f"[DRY RUN] Would generate: {asset.output_header}")
                return True
            
            # Create output directories
            asset.output_cpp.parent.mkdir(parents=True, exist_ok=True)
            asset.output_header.parent.mkdir(parents=True, exist_ok=True)
            
            # Generate xxd content
            xxd_content = self._run_xxd(asset)
            
            # Generate header file
            header_template = self._get_header_template(asset.resource_type)
            header_content = header_template.format(class_name=asset.resource_name)
            
            with open(asset.output_header, 'w') as f:
                f.write(header_content)
            
            # Generate implementation file
            create_function = self._generate_create_function(asset)
            cpp_content = self.CPP_TEMPLATE.format(
                header_name=asset.base_name,
                resource_name=asset.resource_name,
                xxd_content=xxd_content,
                create_function=create_function
            )
            
            with open(asset.output_cpp, 'w') as f:
                f.write(cpp_content)
            
            logger.info(f"✓ Generated {asset.output_cpp.name} and {asset.output_header.name}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to compile {asset.filename}: {e}")
            return False

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Compile assets into C++ resource files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose logging'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Show what would be done without actually doing it'
    )
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Discover all assets
    logger.info("Discovering assets...")
    discovery = AssetDiscovery()
    
    shaders = discovery.find_shaders()
    images = discovery.find_images()
    fonts = discovery.find_fonts()
    
    all_assets = shaders + images + fonts
    
    logger.info(f"Found {len(shaders)} shader(s), {len(images)} image(s), {len(fonts)} font(s)")
    
    if not all_assets:
        logger.warning("No assets found to compile")
        return 0
    
    # Compile assets
    compiler = AssetCompiler()
    success_count = 0
    
    for asset in all_assets:
        if compiler.compile_asset(asset, dry_run=args.dry_run):
            success_count += 1
    
    # Summary
    total = len(all_assets)
    if args.dry_run:
        logger.info(f"[DRY RUN] Would compile {total} asset(s)")
    else:
        logger.info(f"Successfully compiled {success_count}/{total} asset(s)")
        if success_count < total:
            return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
