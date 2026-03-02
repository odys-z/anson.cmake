import unittest
from typing import cast

from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings

from src.anson_parser import parse_anson

class TemplateClassTest(unittest.TestCase):
    def test_parser(self):
        # work folder: ..
        settings: CSettings = cast(CSettings, Anson.from_file("test-registry/test.json"))

        enums, clss, tmpls = parse_anson(settings)
        self.assertTrue('AnsonBody' in tmpls, 'AnsonBody in resolving!')
        self.assertFalse('T comment' in tmpls, 'T comment not in resolving!')
        self.assertFalse('// T comment' in tmpls, '// T comment not in resolving!')
        self.assertFalse('s comment' in tmpls, 'typename s // s comment => class TwoTypeParamsLine2')


if __name__ == '__main__':
    unittest.main()
    t = TemplateClassTest()
    t.test_parser()

