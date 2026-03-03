import unittest

from src.anson_parser import strip_comment

class HandleCommentTest(unittest.TestCase):
    def test_parser(self):
        tests = [['T comment', '  /** T comment   */'],
                 ['s comment', '// s comment  '],
                 ['', '/* */'],
                 ['', '/* **/'],
                 ['', '/**/'],
                 ['', '/***/'],
                 ['', '/********/'],
                 ['', '//   '],
                 ['xy z', '// xy z  '],
                 ['', '/** */'],
                 ['x', """
                      /**
                        x
                       */ """],
                 ['* x', """/**
                       * x
                       */ """] ]
        for tst in tests:
            self.assertEqual(tst[0], strip_comment(tst[1]), tst[1])

if __name__ == '__main__':
    unittest.main()
    t = HandleCommentTest()
    t.test_parser()
