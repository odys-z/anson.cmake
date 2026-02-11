"""

"""
import os
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Tuple, cast

import tree_sitter_cpp as stcpp
from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings
from tree_sitter import Language, Parser, Query, QueryCursor, Node

from query_strings import field_id_isfunc, qv


@dataclass
class MetaEnum(Anson):
    etype: str
    enums: List[str] = field(default_factory=list)

    def __init__(self, etype: str):
        super().__init__()
        self.etype = etype
        self.enums = []

@dataclass
class MetaClass(Anson):
    cname: str
    base: Optional[str] = None
    fields: List[Tuple[str, str]] = field(default_factory=list)  # (type, name)
    ctors: List[List[str]] = field(default_factory=list)  # list of arg types
    funcs: List[List[str]] = field(default_factory=list)  # list of arg types

    # Gemini lost the classical __init__(), resulting in error:
    # TypeError: MetaClass.__init__() missing 2 required positional arguments: 'verbose' and '__type__'
    def __init__(self, cname: str, base: str=None):
        super().__init__()
        self.cname = cname
        self.base = base
        self.fields = []
        self.ctors = []
        self.funcs = []
        # print(self.__type__, cname)


def T(caps: dict, qv: str):
    return cast(Node, caps[qv][0]).text.decode('utf8') if qv in caps else None


def has(caps: dict, qv: str) -> bool:
    return qv in caps

def extract_enum_data(caps: Dict[str, List[Node]], found_enums: Dict[str, MetaEnum]):
    """Extracts enum names and values into MetaEnum objects."""
    ename = caps["enum_name"][0].text.decode('utf8')
    if ename not in found_enums:
        found_enums[ename] = MetaEnum(etype=ename)

    if "enum_val" in caps:
        for v_node in caps["enum_val"]:
            val_text = v_node.text.decode('utf8')
            if val_text not in found_enums[ename].enums:
                found_enums[ename].enums.append(val_text)


def get_parameter_types(params_node: Node) -> List[str]:
    """Helper to extract pure types from a parameter list, ignoring variable names."""
    type_list = []
    for param in params_node.children:
        if param.type == "parameter_declaration":
            type_node = param.child_by_field_name("type")
            if not type_node: continue

            full_type = type_node.text.decode('utf8')
            decl_node = param.child_by_field_name("declarator")

            # Drill through pointers/references to get the full type string
            curr = decl_node
            while curr and curr.type in ["pointer_declarator", "reference_declarator"]:
                for child in curr.children:
                    if child.type in ["*", "&", "&&"]:
                        full_type += child.text.decode('utf8')
                curr = curr.child_by_field_name("declarator")

            type_list.append(full_type.strip())
    return type_list


def extract_class_member(caps: Dict[str, List[Node]], found_classes: Dict[str, MetaClass]):
    """Processes fields, methods, and constructors for a MetaClass."""
    class_node = caps[qv.class_name][0]
    cname = class_node.text.decode('utf8')

    meta: MetaClass
    if cname not in found_classes:
        base = caps['base_name'][0].text.decode('utf8') if 'base_name' in caps else None
        meta = MetaClass(cname=cname, base=base)
        found_classes[cname] = meta
    else:
        meta = found_classes[cname]

    # 1. Handle Fields (and filter out methods/static)
    if "field_decl" in caps:
        is_static = any(s.text.decode('utf8') == "static" for s in caps.get("storage", []))
        decl_node = caps["field_decl"][0]

        print('Field:', caps['field_name'][0].text.decode('utf8'),
                       caps['field_type'][0].text.decode('utf8'),
                       caps['field_register'][0].text.decode('utf8'))

        ftype = caps["field_type"][0].text.decode('utf8')
        if not is_static:
            meta.fields.append((ftype, decl_node.text.decode('utf8')))
        else:
            print("Ignore static", ftype, decl_node.text)

    # 2. Handle Explicit Constructors (from declaration or function_definition)
    elif "ctor_name" in caps:
        ctor_name = caps["ctor_name"][0].text.decode('utf8')
        params_node = caps["ctor_params"][0]
        paras = get_parameter_types(params_node)
        if ctor_name == cname:
            meta.ctors.append(paras)
            print("Ctor :", ctor_name, paras)
        else:
            meta.funcs.append(paras)
            print("Func :", ctor_name, paras)

def extract_templs(caps: Dict[str, List[Node]], templs: dict):
    pass

def parse_anson(config: CSettings, namespace="anson"):
    src_files = config.headers if hasattr(config, 'headers') else []
    CPP_LANGUAGE = Language(stcpp.language())
    parser = Parser(CPP_LANGUAGE)
    query = Query(CPP_LANGUAGE, field_id_isfunc)
    cursor = QueryCursor(query)

    found_enums: Dict[str, MetaEnum] = {}
    found_classes: Dict[str, MetaClass] = {}
    found_templs: Dict[str, MetaClass] = {}

    for header in src_files:
        if not os.path.exists(header): continue
        with open(header, "rb") as f:
            tree = parser.parse(f.read())

        for match in cursor.matches(tree.root_node):
            caps = match[1]

            if "enum_name" in caps:
                extract_enum_data(caps, found_enums)

            # if "templated_entity" in caps:
            if has(caps, qv.templ_entity):
                extract_templs(caps, found_templs)
                print("Ignore template", T(caps, qv.templ_params), T(caps, qv.templ_entity), T(caps, qv.templ_name))

            if "class_name" in caps:
                extract_class_member(caps, found_classes)

    return found_enums, found_classes

def gen_entt_registry(founds, settings: CSettings, namespace='anson'):
    found_enums, found_classes = founds
    # 3. Generate Output
    indent = "    "
    output = ['#pragma once',
              '',
              '#include <entt/meta/factory.hpp>',
              '#include <entt/meta/meta.hpp>',
              '',
              'using namespace entt::literals;',
              '']

    for s in settings.src:
        output.append(f'#include "{os.path.basename(s)}"')

    output.append(f"\nnamespace {namespace} {{")
    output.append(f"inline void register_meta() {{")

    # Enum Registration (Ensures Port and MsgCode are registered)
    for ename, evals in found_enums.items():
        output.append(f"{indent}// Register {ename} enum")
        output.append(f"{indent}entt::meta_factory<{namespace}::{ename}>()")
        output.append(f"{indent * 2}.type(\"{ename}\"_hs)")
        for v in evals:
            output.append(f"{indent * 2}.data<{namespace}::{ename}::{v}>(\"{v}\"_hs, \"{v}\")")
        output.append(f"{indent * 2};\n")

    # Class Registration with Data Fields
    for cname, info in found_classes.items():
        if cname in found_enums: continue
        output.append(f"{indent}entt::meta_factory<{namespace}::{cname}>()")
        output.append(f"{indent * 2}.type(\"{cname}\"_hs)")

        if 'ctors' in info:
            for params in info["ctors"]:
                output.append(f"{indent * 2}.ctor{params}()")

        if info["base"]:
            output.append(f"{indent * 2}.base<{namespace}::{info['base']}>()")
        for ftype, fname in info["fields"]:
            output.append(f"{indent * 2}.data<&{namespace}::{cname}::{fname}>(\"{fname}\"_hs, \"{fname}\")")
        output.append(f"{indent * 2};\n")

    # # Specialized AnsonMsg Template Registration
    # for sub, base in anson_body_subclasses:
    #     output.append(f"{indent}// Specialized AnsonMsg for {sub}")
    #     output.append(f"{indent}entt::meta_factory<{namespace}::AnsonMsg<{namespace}::{sub}>>()")
    #     output.append(f"{indent * 2}.type(\"AnsonMsg{sub}\"_hs)")
    #     output.append(f"{indent * 2}.base<{namespace}::{base}>()")
    #
    #     # TODO FIXME LLM Error
    #     output.append(f"{indent * 2}.data<&{namespace}::AnsonMsg<{namespace}::{sub}>::port>(\"port\"_hs, \"port\")")
    #     output.append(f"{indent * 2}.data<&{namespace}::AnsonMsg<{namespace}::{sub}>::body>(\"body\"_hs, \"body\");\n")

    output.append("}\n}")

    with open(settings.json_h, "w") as f:
        f.write("\n".join(output))

if __name__ == "__main__":
    settings: CSettings = cast(CSettings, Anson.from_file("anson.json"))
    enums, classes = parse_anson(settings)
    for name, node in enums.items():
        print(name, node)


