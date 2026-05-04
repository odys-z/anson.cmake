import unittest
from pathlib import Path
from typing import cast

from anson.io.odysz.anson import Anson
from semanticshare.io.odysz.semantier import PeerSettings, AnsonMsgAst, gen_peers


class EchoMsgTest(unittest.TestCase):
    def test_echo(self):
        # work folder: ..
        settings: PeerSettings = cast(PeerSettings, Anson.from_file("test-registry/t-01-config.json"))

        echo_ast = cast(AnsonMsgAst, Anson.from_file('test-registry/ast/echo.ast.json'))
        self.assertEqual('io.odysz.semantic.jserv.echo.EchoReq', echo_ast.anclass, 'EchoReq to resolving!')

        testpath = Path('test-registry')
        gen_peers(settings, testpath)

        with open('test-registry/gen/cpp/t01-echo.expect.hpp', 'r') as e, open(testpath / settings.header, 'r') as f:
            self.assertEqual(e.readlines(), f.readlines())

        with open('test-registry/gen/cpp/t01-json.expect.hpp', 'r') as e, open(testpath / settings.json_h, 'r') as f:
            self.assertEqual(e.readlines(), f.readlines())

if __name__ == '__main__':
    unittest.main()
    t = EchoMsgTest()
    t.test_echo()
