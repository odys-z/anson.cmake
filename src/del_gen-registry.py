"""

"""
import os
from typing import List, Dict, Tuple, cast, Final

from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings

from del_anson_parser import parse_anson, MetaClass, MetaEnum, MetaTempl

indent: Final[str] = "    "

def special_templs(output: List[str],
                   templs: Dict[str, MetaTempl],
                   clsses: Dict[str, MetaClass],
                   namespace: str='anson'):
    '''
    Specialize templated class.
    :param output:
    :param templs:
    :return:
    '''
    for cname, info in clsses.items():
        if info.base not in templs: continue

        templ = templs[info.base]
        tempcname = templ.class_meta.cname + cname

        output.append(f"{indent}entt::meta_factory<{namespace}::{tempcname}>()")
        output.append(f"{indent * 2}.type(\"{tempcname}\"_hs)")

        # if qv.ctors in info:
        for params in info.ctors:
            output.append(f"{indent * 2}.ctor{params}()")

        if templ.class_meta.base:
            # Not the AnsonMsg case
            output.append(f"{indent * 2}.base<{namespace}::{templ.class_meta.base}>()")

        for ftype, fname in templ.class_meta.fields:
            output.append(f"{indent * 2}.data<&{namespace}::{templ.class_meta.cname}::{fname}>(\"{fname}\"_hs, \"{fname}\")")
        output.append(f"{indent * 2};\n")


def gen_entt_registry(founds: Tuple[Dict[str, MetaEnum],
                                    Dict[str, MetaTempl],
                                    Dict[str, MetaClass]],
                      settings: CSettings, namespace='anson') -> List[str]:
    found_enums, resolving_templs, found_classes = founds

    # 3. Generate Output
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
    output.append("inline void register_meta() {")

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

        # if qv.ctors in info:
        for params in info.ctors:
            output.append(f"{indent * 2}.ctor{params}()")

        if info.base and info.base in resolving_templs:
            output.append(f"{indent * 2}.base<{namespace}::{info.base}>()")
            resolving_templs[info.base].to_special.append(cname)

        # for ftype, fname in info["fields"]:
        for ftype, fname in info.fields:
            output.append(f"{indent * 2}.data<&{namespace}::{cname}::{fname}>(\"{fname}\"_hs, \"{fname}\")")
        output.append(f"{indent * 2};\n")

        special_templs(output, resolving_templs, found_classes)

    output.append("}\n}")

    with open(settings.json_h, "w") as f:
        f.write("\n".join(output))

    return output

if __name__ == "__main__":
    settings: CSettings = cast(CSettings, Anson.from_file("del_anson.json"))
    enums, templs, classes = parse_anson(settings)
    for name, node in enums.items():
        print(name, node)

    lines = gen_entt_registry((enums, templs, classes), settings)


