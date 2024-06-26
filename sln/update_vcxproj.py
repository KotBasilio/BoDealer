import os
import sys

def replace_platform_toolset_in_vcxproj_files(old_version, new_version, root_dir='.'):
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith('.vcxproj'):
                filepath = os.path.join(dirpath, filename)
                with open(filepath, 'r', encoding='utf-8') as file:
                    content = file.read()
                
                old_toolset = f'<PlatformToolset>{old_version}</PlatformToolset>'
                new_toolset = f'<PlatformToolset>{new_version}</PlatformToolset>'
                
                new_content = content.replace(old_toolset, new_toolset)
                
                if new_content != content:
                    with open(filepath, 'w', encoding='utf-8') as file:
                        file.write(new_content)
                    print(f"Updated: {filepath}")
                else:
                    print(f"No changes needed: {filepath}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python update_vcxproj.py <old_version> <new_version>")
        sys.exit(1)
    
    old_version = sys.argv[1]
    new_version = sys.argv[2]
    
    replace_platform_toolset_in_vcxproj_files(old_version, new_version)
