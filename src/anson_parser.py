"""
"""
import os
import re
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Tuple, cast

import tree_sitter_cpp as stcpp
from anson.io.odysz.anson import Anson
from anson.io.odysz.common import LangExt
from semanticshare.gen.cmake import CSettings

# requires tree_sitter 0.25.2 (Python 3.10?)
from tree_sitter import Language, Parser, Query, QueryCursor, Node

from src.query_strings import field_id_isfunc, qv, template_class_dag


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
    def __init__(self, cname: str, base: str = None) -> None:
        super().__init__()
        self.cname = cname
        self.base = base
        self.fields = []
        self.ctors = []
        self.funcs = []
        # print(self.__type__, cname)

@dataclass
class MetaTempl(Anson):
    templ: str = None
    '''
    Template Identifier
    '''
    ttype: str = None
    ''' Required type, e.g. AnsonBody '''

    to_special: List[str] = field(default_factory=list)
    '''
    E.g. 'EchoReq'
    '''

    class_meta: MetaClass = None

    def __init__(self, templ_id: str, templ_comments: str, class_meta: MetaClass) -> None:
        def parse_comments(t_comm: str):
            if LangExt.isblank(t_comm):
                return ''
            cmms = strip_comment(t_comm).split('::')
            return cmms[1] if cmms[0].strip() == 'anson' else t_comm

        super().__init__()
        self.templ = templ_id
        self.ttype = parse_comments(templ_comments)
        # self.cname = class_name
        self.class_meta = class_meta


def strip_comment(comment):
    return re.sub(r'\s*((/\*\*+/)|(/\*+)\s*|\s*(\*+)/)|(\s*$)|(\s*//\s*)',
                  '', comment)


def T(caps: dict, qv: str):
    return cast(Node, caps[qv][0]).text.decode('utf8') if qv in caps else None


def has(caps: dict, qv: str) -> bool:
    return qv in caps


def extract_enum_data(caps: Dict[str, List[Node]], found_enums: Dict[str, MetaEnum]):
    """Extracts enum names and values into MetaEnum objects."""
    ename = T(caps, qv.enum_name)
    if ename not in found_enums:
        found_enums[ename] = MetaEnum(etype=ename)

    if qv.enum_val in caps:
        for _ in caps[qv.enum_val]:
            # val_text = v_node.text.decode('utf8')
            val_text = T(caps, qv.enum_val)
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


def extract_class_member(caps: Dict[str, List[Node]], found_classes: Dict[str, MetaClass]) -> MetaClass:
    """Processes fields, methods, and constructors for a MetaClass."""
    cname = T(caps, qv.class_name)

    meta: MetaClass
    if cname not in found_classes:
        base = T(caps, qv.base_name)
        meta = MetaClass(cname=cname, base=base)
        found_classes[cname] = meta
    else:
        meta = found_classes[cname]

    # 1. Handle Fields (and filter out methods/static)
    if qv.field_decl in caps:
        is_static = any(s.text.decode('utf8') == "static" for s in caps.get(qv.storage, []))

        print('Field:', T(caps, qv.field_name),
                        T(caps, qv.field_type),
                        T(caps, qv.field_regist))

        if not is_static:
            meta.fields.append((T(caps, qv.field_type), T(caps, qv.field_decl)))
        else:
            print("Ignore Static")

    # 2. Handle Explicit Constructors (from declaration or function_definition)
    elif has(caps, qv.ctor_name):
        ctor_name = T(caps, qv.ctor_name)
        params_node = caps[qv.ctor_params][0]
        paras = get_parameter_types(params_node)
        if ctor_name == cname:
            meta.ctors.append(paras)
            print("Ctor :", ctor_name, paras)
        else:
            meta.funcs.append(paras)
            print("Func :", ctor_name, paras)

    return meta


def extract_templs(caps: Dict[str, List[Node]], templs: Dict[str, MetaTempl], classes: Dict[str, MetaClass]):
    tp_name = T(caps, qv.templ_class)

    if not has(caps, qv.templ_body):
        print("Not Reachable: Ignore template classes forward declaration.", tp_name)
        return

    templ_meta = MetaTempl(tp_name, T(caps, qv.templ_entity), extract_class_member(caps, classes))
    if templ_meta.ttype in classes:
        templs[templ_meta.ttype] = templ_meta
        print("Resolving template", T(caps, qv.templ_class), T(caps, qv.templ_type), ":", templ_meta.ttype)
    else:
        print("Ignoring template", T(caps, qv.templ_class), T(caps, qv.templ_type), templ_meta.ttype)
        print(f'    Make sure base type annotation is correct and is registered before {T(caps, qv.templ_class)}:', templ_meta.ttype)


def parse_anson(config: CSettings, namespace="anson", qstr=template_class_dag):
    src_files = config.headers if hasattr(config, 'headers') else []
    CPP_LANGUAGE = Language(stcpp.language())
    parser = Parser(CPP_LANGUAGE)

    with open("qstr.txt", "w") as f:
        print(qstr, file=f)
    query = Query(CPP_LANGUAGE, qstr)
    cursor = QueryCursor(query)

    found_enums  : Dict[str, MetaEnum]  = {}
    found_classes: Dict[str, MetaClass] = {}
    found_templs : Dict[str, MetaTempl] = {}

    for header in src_files:
        if not os.path.exists(header): continue
        with open(header, "rb") as f:
            print('\n', header)
            print(' ' + '=' * len(header))
            tree = parser.parse(f.read())

        for match in cursor.matches(tree.root_node):
            caps = match[1]

            if qv.enum_name in caps:
                extract_enum_data(caps, found_enums)

            if has(caps, qv.templ_entity):
                # extract_class_member(caps, found_classes)
                extract_templs(caps, found_templs, found_classes)

            if qv.class_name in caps:
                extract_class_member(caps, found_classes)

    return found_enums, found_templs, found_classes
