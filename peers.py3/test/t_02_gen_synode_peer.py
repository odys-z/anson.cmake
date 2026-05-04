from pathlib import Path
from typing import cast
from unittest import TestCase

from anson.io.odysz.anson import Anson
from semanticshare.io.odysz.reflect import PeerSettings

from src.semantic_peer.io.oz.semanticpeer.generator import gen_peers


class GenAnsonPeerTest(TestCase):

    def test_(self):
        testpath = Path('test')
        settings = cast(PeerSettings, Anson.from_file(str(testpath / 'settings/t_02-synode-cpp.json')))

        gen_peers(settings, testpath)

        with (open('../tests/expect/t_02_synode.hpp', 'r') as e,
              open(settings.cpp_gen, 'r') as f):
            self.assertEqual(e.readlines(), f.readlines())
