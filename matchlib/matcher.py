import itertools

from matchlib.matchers import greedy_string_tiling
from matchlib.util import safe_div, TokenMatchSet


def _do_comparison(a, tokens_a, b, tokens_b, minimum_match_length):
    if "checksum" in a and "checksum" in b and a["checksum"] == b["checksum"]:
        # Matching checksums, skip token matching and set exact match
        matches = TokenMatchSet()
        # Match of all tokens
        matches.add_non_overlapping(TokenMatch(0, 0, min(len(tokens_a), len(tokens_b))))
        similarity = 1.0
    else:
        marks_a, marks_b = a.get("marks", ''), b.get("marks", '')
        matches = greedy_string_tiling(tokens_a, marks_a, tokens_b, marks_b, minimum_match_length)
        similarity = safe_div(matches.token_count(), (len(tokens_a) + len(tokens_b)) / 2)
    return matches, similarity


def _match_all(config, pairs_to_compare):
    minimum_match_length = config.get("minimum_match_length", 1)
    minimum_similarity = config.get("minimum_similarity", -1)
    for a, b, in pairs_to_compare:
        tokens_a, tokens_b = a["tokens"], b["tokens"]
        if max(len(tokens_a), len(tokens_b)) < minimum_match_length:
            continue
        matches, similarity = _do_comparison(a, tokens_a, b, tokens_b, minimum_match_length)
        if similarity > minimum_similarity:
            yield { "id_a": a["id"],
                    "id_b": b["id"],
                    "match_indexes": matches.json(),
                    "similarity": similarity }


def match_all_combinations(config, string_data_iter):
    """
    Given a configuration dict and an iterable of string data, do string similarity comparisons for all 2-combinations without replacement for the input data.
    Return an iterator over the resulting comparison dicts.
    """
    return _match_all(config, itertools.combinations(string_data_iter, 2))


def match_to_others(config, string_data, other_data_iter):
    """
    Compare one string data object to all other objects in other_data_iter.
    Return an iterator over the resulting comparison dicts.
    """
    return _match_all(config, ((string_data, other_data) for other_data in other_data_iter))
