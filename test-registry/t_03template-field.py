import unittest
from typing import cast

from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings

from src.anson_parser import parse_anson
from src.query_strings import qv

q_template_field = f"""(field_declaration 
            (storage_class_specifier)* @{qv.storage}
            type: (type_specifier (template_type
                   arguments: (template_argument_list
                      [
                        (type_identifier) @{qv.field_name}
                        (primitive_type) @type_param_id
                      ]
                  ))) @{qv.field_type}
            declarator: [
                ((field_identifier) @{qv.field_name}) @{qv.field_regist}
                ;; (reference_declarator (field_identifier) @{qv.field_name}) @{qv.field_regist}
                ;; (pointer_declarator declarator: (field_identifier) @{qv.field_name}) @{qv.field_regist}
                ;; (pointer_declarator declarator: (pointer_declarator declarator: (field_identifier) @{qv.field_name})) @{qv.field_regist}
                ;;
                ] 
            @{qv.field_decl}) @{qv.is_field}
"""

q_templ_class_frame = f"""(class_specifier
    name: (type_identifier) @{qv.class_name}
    (base_class_clause (type_identifier) @{qv.base_name})?
    body: (field_declaration_list 
      [
        {q_template_field}
      ]
    ))
"""


class TemplateFieldTest(unittest.TestCase):
    def test_parser(self):
        # work folder: ..
        settings: CSettings = cast(CSettings, Anson.from_file("test-registry/t_03.json"))

        enums, clss, tmpls = parse_anson(settings)
        self.assertTrue ('AnsonBody' in tmpls, 'AnsonBody in resolving!')

        enums, clss, tmpls = parse_anson(settings, qstr=q_templ_class_frame)
        # self.assertTrue ('AnsonBody' in tmpls, 'AnsonBody in resolving!')
        self.assertFalse('T comment' in tmpls, 'T comment not in resolving!')
        self.assertFalse('// T comment' in tmpls, '// T comment not in resolving!')
        self.assertFalse('s comment' in tmpls, 'typename s // s comment => class TwoTypeParamsLine2')


if __name__ == '__main__':
    unittest.main()
    t = TemplateFieldTest()
    t.test_parser()

