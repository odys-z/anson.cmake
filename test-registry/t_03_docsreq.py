import unittest
from pathlib import Path
from typing import cast

from anson.io.odysz.anson import Anson
from semanticshare.io.odysz.semantier import PeerSettings, AnsonMsgAst, gen_peers


class EchoMsgTest(unittest.TestCase):
    def test_echo(self):
        # work folder: ..
        settings: PeerSettings = cast(PeerSettings, Anson.from_file("test-registry/t-03-docs-config.json"))

        testpath = Path('test-registry')
        gen_peers(settings, testpath)

        with open(testpath / 'gen/cpp/t03-docs.expect.hpp', 'r') as e, open(testpath / settings.header, 'r') as f:
            self.assertEqual(e.readlines(), f.readlines())

        with open(testpath / 'gen/cpp/t03-json.expect.hpp', 'r') as e, open(testpath / settings.json_h, 'r') as f:
            self.assertEqual(e.readlines(), f.readlines())

if __name__ == '__main__':
    unittest.main()
    t = EchoMsgTest()
    t.test_echo()
