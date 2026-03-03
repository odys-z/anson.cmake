import unittest
from idlelib.browser import file_open
from typing import cast

from anson.io.odysz.anson import Anson
from semanticshare.gen.cmake import CSettings, AnsonMsgAst, gen_tier_req


class DocsMsgTest(unittest.TestCase):
    def test_docsreq(self):
        # work folder: ..
        settings: CSettings = cast(CSettings, Anson.from_file("test-registry/t-02-docsreq-config.json"))

        # TODO


if __name__ == '__main__':
    unittest.main()
    t = DocsMsgTest()
    t.test_docsreq()

