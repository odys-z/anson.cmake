from dataclasses import dataclass
from pathlib import Path
from typing import cast, List

from anson.io.odysz.anson import Anson
from anson.io.odysz.common import Utils, LangExt
from semanticshare.io.odysz.reflect import AnsonBodyAst, PeerSettings, AnsonAst, primtypes


@dataclass
class MsgLines:
    start_header = '''#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>

namespace anson {'''
    '''
    [0] pragma once ...
    '''

    class_decl = '''
class {} : public anson::{} {{
public:
    inline static const std::string _type_ = "{}";'''
    '''
    [1] public class {Req} : public anson::{AnsonBody} { _type_={} ...
    '''

    struct_A = '''
    struct A {'''
    '''
    [2] stuct A {
    '''
    # A.a ...

    act_enum = '''
        inline static const string {} = "{}";'''
    '''
    [3] inline static const string...
    '''

    def class_fields(self, ast: AnsonAst):
        fields = []

        for fn, fd in ast.fields.items():
            fields.append(f'    {primtypes.C20[fd["dataAnclass"]]} {fn};\n')

        return fields

    def class_ctors(self, ast: AnsonBodyAst) -> List[str]:
        '''
        :param ctorstrs: e.g.
            [[["echo", "string", "m"], ["r/query"]],
            [[], ["r/query"]]]
        :return:
            EchoReq() : AnsonBody(_type_);
            EchoReq(string m) : AnsonBody(m, _type_);
        '''
        ctors = []
        for ctorss in ast.ctors:
            # e.g. [['r/peer-test'], ['string', 'echo', 'm']]
            parlist, fieldini = [], []
            if LangExt.len(ctorss[0]) == 0:
                base_ini_list = '_type_'
            else:
                base_ini_list = ', '.join([*ctorss[0], '_type_'])

            for parass in ctorss[1:]:
                if LangExt.len(parass) == 0:
                    continue
                if LangExt.len(parass) != 3:
                    Utils.warn("Error: ", parass)
                    continue
                parlist.append(parass[0] + " " + parass[2])
                fieldini.append(f'{parass[1]}({parass[2]})')

            ctors.append(f'    {ast.c_class()}({", ".join(parlist)}) : {ast.c_base()}({base_ini_list}){"," if len(fieldini) > 0 else ""} {", ".join(fieldini)} {{}};\n')
        return ctors

    inline_static = True

    # 0: echoreq, 1: EchoReq
    load_ast = '''void load_{0}Ast(AstMap &asts, const string &ast_path) {{
    specialize_msg_astpth<{1}>(asts, ast_path,
      [](meta_factory<{1}> &entf, AnsonBodyAst *ast) {{'''

    entt_ctor = '''
    entf.ctor<&{0}{1}>();'''

    def entt_ctors(self, ast: AnsonAst) -> List[str]:
        '''
        :param ctorstrs: e.g.
            [[["echo", "string", "m"], ["r/query"]],
            [[], ["r/query"]]]
        :return: .ctor<>().ctor<string>()
        '''
        # return [f'        entf.ctor<{c}>();\n' for ctor in ast.ctors for c in ctor[0]]
        ctorss = []
        for ctor in ast.ctors:
            lst = []
            for c in ctor[1:]:
                if len(c) > 0:
                    lst.append(c[0])
            ctorss.append(f'        entf.ctor<{", ".join(lst)}>();\n')

        return ctorss

    entt_data = '''
        entf.data<&{0}::{1}>("{1}");'''

    field_getter0 = '''
        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {{
            if (ast->fields.contains(fieldname)) {{
                auto& concrete = static_cast<const {0}&>(ans);'''
    field_getif ='''
                if ("{0}" == fieldname)
                    return entt::forward_as_meta(concrete.{0});'''
    field_getter9 = '''
            }}

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {{
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }}

            anerror("get_field_instance<{0}>(): Failed to get entt instance (meta_any)");
            return {{ }};
        }};
  }});
}}'''

    end_ns = '\n}'

    def specialize_req(self, ast: AnsonBodyAst) -> List[str]:
        '''
        Example
        =======
        class EchoReq: public AnsonBody {
        public:
            inline static const std::string _type_ = "io.odysz.semantic.jserv.echo.EchoReq";
            struct A {
                inline static const string echo = "echo";
                inline static const string inet = "inet";
            };

            string echo;
            EchoReq() : AnsonBody("r/query", EchoReq::_type_) {}
            EchoReq(string echo) : AnsonBody("r/query", EchoReq::_type_), echo(echo) {}
        };

        inline static void load_echoAst_expect(AstMap &asts, const string &ast_path) {
            specialize_msg_astpth<EchoReq>(asts, ast_path,
              [](meta_factory<EchoReq> &entf, AnsonBodyAst *ast) {

                entf.data<&EchoReq::echo>("echo");

                ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
                    if (ast->fields.contains(fieldname)) {
                        auto& concrete = static_cast<const EchoReq&>(ans);
                        if ("echo" == fieldname)
                            return entt::forward_as_meta(concrete.echo);
                    }

                    if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                        AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                        return bast->get_field_instance(ans, fieldname);
                    }

                    anerror("get_field_instance<EchoReq>(): Failed to get entt instance (meta_any)");
                    return {};
                };
            });
        }
        :param ast:
        :return: formatted source header lines
        '''
        return [self.class_decl.format(ast.c_class(), ast.c_base(), ast.dataAnclass),
                self.struct_A,
                *[f'\n        inline static const string {k} = "{v}";' for k, v in ast.A.items()],
                '\n\t};\n',
                *self.class_fields(ast),
                *self.class_ctors(ast),
                '};\n',

                # load_ast()
                '\n' + ('inline static ' if self.inline_static else '') + self.load_ast.format(
                    ast.c_class().lower(), ast.c_class()),
                *[self.entt_data.format(ast.c_class(), fn) for fn, _ in ast.fields.items()],
                '\n',
                *self.entt_ctors(ast),
                self.field_getter0.format(ast.c_class()),
                *[self.field_getif.format(fn) for fn, _ in ast.fields.items()],
                self.field_getter9.format(ast.c_class()),
                ]


def gen_cpp_peer(settings: PeerSettings, ast_folder: Path):
    '''
    :param settings:
    :param ast_folder:
    :return:
    '''

    msglines = MsgLines()

    gen_pth = Path(settings.cpp_gen)
    gen_pth.parent.mkdir(parents=True, exist_ok=True)

    with open(gen_pth, 'w') as gen:
        gen.writelines(msglines.start_header)

        for astjson in settings.anRequests:
            if Path(ast_folder / astjson).exists():
                ast = cast(AnsonBodyAst, Anson.from_file(str(ast_folder / astjson)))
                # Utils.log_arr(msglines.class_ctors(ast))
                # Utils.log_arr(msglines.specialize_req(ast))
                gen.writelines(msglines.specialize_req(ast))
            else:
                Utils.warn('Cannot find file ' + astjson)

        gen.writelines(msglines.end_ns)


def gen_peers(settings: PeerSettings, config_path: Path) -> None:
    # gen_ts_peer(settings)
    # gen_py_peer(settings)
    gen_cpp_peer(settings, config_path)
