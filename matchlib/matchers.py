from gst import match as match_c_ext

from matchlib.util import TokenMatch, TokenMatchSet


def greedy_string_tiling(tokens_a, marks_a, tokens_b, marks_b, min_length):
    """
    Wrapper of the C++ extension gst.match, which implements the Running Karp-Rabin Greedy String Tiling algorithm by Michael J. Wise.
    """
    matches = TokenMatchSet()
    if len(tokens_a) < min_length or len(tokens_b) < min_length:
        return matches

    # Choose the shorter token string to be the pattern and the longer as text
    reverse = len(tokens_b) < len(tokens_a)
    pattern = tokens_b if reverse else tokens_a
    text = tokens_a if reverse else tokens_b
    pattern_marks = marks_b if reverse else marks_a
    text_marks = marks_a if reverse else marks_b

    match_list = match_c_ext(pattern, pattern_marks, text, text_marks, min_length)

    if reverse:
        matches.store = [TokenMatch(match[1], match[0], match[2]) for match in match_list]
    else:
        matches.store = [TokenMatch(*match) for match in match_list]

    return matches
