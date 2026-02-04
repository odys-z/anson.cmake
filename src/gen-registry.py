import CppHeaderParser
import sys
import os
# Gemini: from anson.anson import Anson
from anson.io.odysz.anson import Anson

'''
Gemini generated Anson class for the gen.json configuration.
============================================================

    from anson.anson import Anson
    #    anson.io.odysz.anson import Anson

    class CmakeSettings(Anson):
        """Mapping for io.oz.gen.cmake.settings"""
        def __init__(self):
            super().__init__()
            self.type = "io.oz.gen.cmake.settings" # not correct. Gemini doesn't know the type's rule. But shouldn't it can be fixed figured out?
            self.src = []
            self.gen = ""

    # IMPORTANT: Register the type so the parser knows which class to instantiate
    from anson.anson import AnsonProtocol # not correct.
    AnsonProtocol.register("io.oz.gen.cmake.settings", CmakeSettings)
    # ^ I don't have such code anymore. Where did Gemini found it?
    # Actually I think it's somewhere tried in order versions/ designs. 

'''

def generate_entt_registration(config_path="gen.json", namespace="anson"):
    # 1. Load config using Antson (Pure Pip / Pure Python)
    if not os.path.exists(config_path):
        print(f"Error: {config_path} not found.", file=sys.stderr)
        return

    # Gemini
    # with open(config_path, 'r') as f:
    #     # Based on anson_test.py, use Anson.fromJson
    #     config = Anson.fromJson(f.read())
    config = Anson.from_file(config_path)

    # 2. Extract settings from the Anson object
    src_files = config.src if hasattr(config, 'src') else []
    output_filename = config.gen if hasattr(config, 'gen') else "jsoh.h"

    all_classes = []

    # 3. Code Generation Header
    output = []
    for header in src_files:
        output.append(f'#include "{os.path.basename(header)}"')
    output.append('#include <entt/meta/factory.hpp>\n#include <entt/meta/meta.hpp>\n')
    output.append(f"namespace {namespace} {{")
    output.append("inline void register_meta() {")
    output.append("    using namespace entt::literals;\n")

    # 4. AST Parsing Loop
    for header in src_files:
        try:
            cpp_header = CppHeaderParser.CppHeader(header)
            for class_name, class_obj in cpp_header.classes.items():
                # Filter: Abstract/Virtual/Forward/Templates
                if "<" in class_name or ">" in class_name: continue

                # Check for virtual methods
                is_virtual = False
                for scope in ["public", "protected", "private"]:
                    if any(m.get("virtual") for m in class_obj["methods"][scope]):
                        is_virtual = True; break
                if is_virtual: continue

                # Extraction
                bases = []
                for inherit in class_obj.get("inherits", []):
                    b_name = inherit.get("class") or inherit.get("point")
                    if b_name: bases.append(b_name.split("::")[-1])

                fields = [p["name"] for p in class_obj["properties"]["public"] if not p.get("static")]
                has_ctor = any(len(m["parameters"]) == 0 for m in class_obj["methods"]["public"] if m["name"] == class_name)

                # Format as requested
                output.append(f"    entt::meta_factory<{namespace}::{class_name}>()")
                output.append(f"        .type(\"{class_name}\"_hs)")
                if has_ctor: output.append("        .ctor<>()")
                for b in bases: output.append(f"        .base<{namespace}::{b}>()")
                for f in fields: output.append(f"        .data<&{namespace}::{class_name}::{f}>(\"{f}\"_hs, \"{f}\")")
                output.append("    ;\n")
        except Exception as e:
            print(f"// Error parsing {header}: {e}", file=sys.stderr)

    output.append("}\n}")

    # 5. Save to the file defined in gen.json
    with open(output_filename, "w") as f:
        f.write("\n".join(output))
    print(f"Generated {output_filename} successfully.")

if __name__ == "__main__":
    generate_entt_registration()