import unittest
from idlelib.browser import file_open
from typing import cast

from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings, AnsonMsgAst, gen_tier_req


class EchoMsgTest(unittest.TestCase):
    def test_echo(self):
        # work folder: ..
        settings: CSettings = cast(CSettings, Anson.from_file("test-registry/t-01-config.json"))

        echo_ast = cast(AnsonMsgAst, Anson.from_file('test-registry/echo.ast.json'))
        self.assertEqual('io.odysz.semantic.jprotocol.EchoReq', echo_ast.body, 'EchoReq to resolving!')
        gen_tier_req(settings, echo_ast)

        with file_open('t-01-header.expect', 'r') as e, file_open(settings.header, 'r') as f:
            self.assertEqual(e.readlines(), f.readlines())

        with file_open('t-01-json-reg.expect', 'r') as e, file_open(settings.json_h, 'r') as f:
            self.assertEqual(e.readlines(), f.readlines())

if __name__ == '__main__':
    unittest.main()
    t = EchoMsgTest()
    t.test_echo()

