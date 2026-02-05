import re
import sys
import os
import CppHeaderParser
from typing import cast
from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings


def generate_entt_registration(config: CSettings, namespace="anson"):
    src_files = config.headers if hasattr(config, 'headers') else []
    output_filename = config.json_h if hasattr(config, 'json_h') else "json.hpp"

    # Global symbol table to merge all classes across all files
    global_classes = {}

    # 1. Merge all ASTs first
    for header in src_files:
        try:
            cpp_header = CppHeaderParser.CppHeader(header)
            # Merge classes into global dictionary
            global_classes.update(cpp_header.classes)
        except Exception as e:
            print(f"// Error parsing file {header}: {e}", file=sys.stderr)

    output = []
    # Dynamic Includes
    for s in src_files:
        output.append(f'#include "{os.path.basename(s)}"')
    output.append('#include <entt/meta/factory.hpp>\n#include <entt/meta/meta.hpp>\n')

    output.append(f"namespace {namespace} {{")
    output.append("inline void register_meta() {")
    output.append("    using namespace entt::literals;\n")

    template_specializations = set()
    available_class_names = list(global_classes.keys())

    # 2. Process each class from the merged AST with guards
    for class_name, class_obj in global_classes.items():
        try:
            # Skip templates and abstract/virtual classes
            if "<" in class_name: continue
            if any(m.get("virtual") for scope in ["public", "protected"]
                   for m in class_obj["methods"][scope]):
                continue

            # --- Global Template Detection ---
            for base_candidate in available_class_names:
                try:
                    # Regex pattern dynamically built for the current class
                    pattern = re.compile(rf"{base_candidate}<\s*([^>]+)\s*>")

                    # Search through every property of every class in the global AST
                    for search_obj in global_classes.values():
                        for prop in search_obj["properties"]["public"]:
                            ptype = prop.get("type", "")
                            match = pattern.search(ptype)
                            if match:
                                # Extract and clean the inner type (the 'T')
                                inner_type = match.group(1).split("::")[-1].strip()
                                template_specializations.add((base_candidate, inner_type))
                except Exception as re_e:
                    print(f"// Error in template regex for {base_candidate}: {re_e}", file=sys.stderr)

            # 3. Standard Factory Generation
            output.append(f"    entt::meta_factory<{namespace}::{class_name}>()")
            output.append(f"        .type(\"{class_name}\"_hs)")

            # Dynamic Constructor Iteration
            public_methods = class_obj["methods"]["public"]
            for method in public_methods:
                if method["name"] == class_name:
                    params = method.get("parameters", [])
                    param_types = [p["type"].replace("string", "std::string") for p in params]
                    output.append(f"        .ctor<{', '.join(param_types)}>()")

            # Bases
            for inherit in class_obj.get("inherits", []):
                b = (inherit.get("class") or inherit.get("point")).split("::")[-1]
                output.append(f"        .base<{namespace}::{b}>()")

            # Fields
            for p in class_obj["properties"]["public"]:
                if not p.get("static"):
                    f = p["name"]
                    output.append(f"        .data<&{namespace}::{class_name}::{f}>(\"{f}\"_hs, \"{f}\")")
            output.append("    ;\n")
        except Exception as cls_e:
            print(f"// Error processing class {class_name}: {cls_e}", file=sys.stderr)
            continue

    # 4. Generate Specialized Template Registrations with guards
    for base_tmpl, inner_type in template_specializations:
        try:
            output.append(f"    // Specialized: {base_tmpl}<{inner_type}>")
            output.append(f"    entt::meta_factory<{namespace}::{base_tmpl}<{namespace}::{inner_type}>>()")
            output.append(f"        .type(\"{base_tmpl}_{inner_type}\"_hs)")
            output.append(f"        .base<{namespace}::Anson>()")
            # Explicitly mapping standard fields found in AnsonMsg
            output.append(
                f"        .data<&{namespace}::{base_tmpl}<{namespace}::{inner_type}>::body>(\"body\"_hs, \"body\")")
            output.append(
                f"        .data<&{namespace}::{base_tmpl}<{namespace}::{inner_type}>::port>(\"port\"_hs, \"port\")")
            output.append("    ;\n")
        except Exception as tmpl_e:
            print(f"// Error generating specialization {base_tmpl}<{inner_type}>: {tmpl_e}", file=sys.stderr)

    output.append("}\n}")

    with open(output_filename, "w") as f:
        f.write("\n".join(output))
    print(f"Generated {output_filename} successfully.")


if __name__ == "__main__":
    # Ensure anson.json exists and maps to CSettings
    settings: CSettings = cast(CSettings, Anson.from_file("anson.json"))
    generate_entt_registration(settings)