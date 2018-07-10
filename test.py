import unittest
import importlib
import string

from hypothesis import strategies, settings, given

# TODO test cases for:
# pattern is prefix of text
# pattern is suffix of text
# both
# pattern is longer than text
# varying minmatch length


class TestCase(unittest.TestCase):

    def assertCorrectMatchSubstringMapping(self, pattern, text, match):
        self.assertIsInstance(match, tuple)
        pattern_start, text_start, match_len = match
        self.assertEqual(pattern[pattern_start:pattern_start + match_len],
                         text[text_start:text_start + match_len])


class Test1Simple(TestCase):

    def test1_import(self):
        self.assertIsNotNone(importlib.import_module("gst"))

    def test2_single_full_match(self):
        import gst
        pattern = b"hello"
        text = b"how delightful, hello there"
        matches = gst.match(pattern, '', text, '', len(pattern))
        self.assertIsInstance(matches, list)
        self.assertEqual(len(matches), 1)
        self.assertCorrectMatchSubstringMapping(pattern, text, matches[0])

        pattern_str, text_str = pattern.decode("ascii"), text.decode("ascii")
        matches = gst.match(pattern_str, '', text_str, '', len(pattern_str))
        self.assertCorrectMatchSubstringMapping(pattern, text, matches[0])

    def test3_single_partial_match(self):
        import gst
        pattern = b"hello"
        text = b"we are in helsinki now"
        match_len = 3
        matches = gst.match(pattern, '', text, '', match_len)
        self.assertIsInstance(matches, list)
        self.assertEqual(len(matches), 1)
        self.assertCorrectMatchSubstringMapping(pattern, text, matches[0])

        pattern_str, text_str = pattern.decode("ascii"), text.decode("ascii")
        matches = gst.match(pattern_str, '', text_str, '', match_len)
        self.assertIsInstance(matches, list)
        self.assertEqual(len(matches), 1)
        self.assertCorrectMatchSubstringMapping(pattern, text, matches[0])

    def test4_no_match(self):
        import gst
        pattern = b"hello"
        text = b"go away, you nuisance"
        matches = gst.match(pattern, '', text, '', len(pattern))
        self.assertIsInstance(matches, list)
        self.assertEqual(len(matches), 0)

        pattern_str, text_str = pattern.decode("ascii"), text.decode("ascii")
        matches = gst.match(pattern_str, '', text_str, '', len(pattern_str))
        self.assertIsInstance(matches, list)
        self.assertEqual(len(matches), 0)


@strategies.composite
def tuples_of_text_and_substring(draw, text_min_size=0, text_max_size=100, alphabet=string.printable):
    text = draw(strategies.text(
        alphabet=alphabet,
        min_size=text_min_size,
        max_size=text_max_size))
    pattern_begin = draw(strategies.integers(
        min_value=0,
        max_value=len(text)))
    pattern_end = draw(strategies.integers(
        min_value=pattern_begin,
        max_value=len(text)))
    pattern = text[pattern_begin:pattern_end]
    return text, pattern


@strategies.composite
def text_substring_and_minmatch_length(draw, **kwargs):
    text, pattern = draw(tuples_of_text_and_substring(**kwargs))
    min_match_length = draw(strategies.integers(
            min_value=min(1, len(pattern)),
            max_value=min(100, len(pattern))))
    return text, pattern, min_match_length


class Test2RandomInputShortStrings(TestCase):

    def setUp(self):
        self.gst = importlib.import_module("gst")

    @settings(max_examples=300)
    @given(text_and_pattern=tuples_of_text_and_substring())
    def test1_full_match(self, text_and_pattern):
        text, pattern = text_and_pattern
        matches = self.gst.match(pattern, '', text, '', len(pattern))
        for match in matches:
            self.assertCorrectMatchSubstringMapping(pattern, text, match)

    @settings(max_examples=300)
    @given(text_and_pattern=tuples_of_text_and_substring())
    def test2_full_match_bytes(self, text_and_pattern):
        text, pattern = text_and_pattern
        pattern_bytes, text_bytes = pattern.encode("ascii"), text.encode("ascii")
        matches = self.gst.match(pattern_bytes, '', text_bytes, '', len(pattern_bytes))
        for match in matches:
            self.assertCorrectMatchSubstringMapping(pattern, text, match)


class Test3RandomInputLongStrings(TestCase):

    def setUp(self):
        self.gst = importlib.import_module("gst")

    @settings(max_examples=10)
    @given(text_and_pattern=tuples_of_text_and_substring(text_min_size=100, text_max_size=2000))
    def test1_full_match(self, text_and_pattern):
        text, pattern = text_and_pattern
        matches = self.gst.match(pattern, '', text, '', len(pattern))
        for match in matches:
            self.assertCorrectMatchSubstringMapping(pattern, text, match)

    @settings(max_examples=10)
    @given(text_and_pattern=tuples_of_text_and_substring(text_min_size=100, text_max_size=2000))
    def test2_full_match_bytes(self, text_and_pattern):
        text, pattern = text_and_pattern
        pattern_bytes, text_bytes = pattern.encode("ascii"), text.encode("ascii")
        matches = self.gst.match(pattern_bytes, '', text_bytes, '', len(pattern_bytes))
        for match in matches:
            self.assertCorrectMatchSubstringMapping(pattern, text, match)


if __name__ == "__main__":
    unittest.main(verbosity=2)

