import itertools

from matchlib.matchers import greedy_string_tiling
from matchlib.util import TokenMatchSet


RESULT_KEYS = ["id_a", "id_b", "match_indexes", "similarity"]


def _match_all(config, pairs_to_compare):
    """
    Compare all pairs and return an iterator over the matches.
    """
    minimum_match_length = config.get("minimum_match_length", 1)
    minimum_similarity = config.get("minimum_similarity", -1)
    similarity_precision = config.get("similarity_precision")
    optional_round = (lambda x: round(x, similarity_precision)) if similarity_precision is not None else (lambda x: x)
    for a, b, in pairs_to_compare:
        if max(a["longest_authored_tile"], b["longest_authored_tile"]) < minimum_match_length:
            # Skip pairs that have too few unique tokens
            continue
        # Get the string pair that will be compared
        tokens_a, tokens_b = a["tokens"], b["tokens"]
        if "checksum" in a and "checksum" in b and a["checksum"] == b["checksum"]:
            # Skip syntax token matching and create a full match of all tokens
            matches = TokenMatchSet.full_match_from_length(min(len(tokens_a), len(tokens_b)))
            # Match of all tokens
            similarity = 1.0
        else:
            # Compare unique syntax tokens, ignoring marked tokens
            # If no marks are given, assume no tokens are marked
            marks_a = a.get("ignore_marks", '0' * len(tokens_a))
            marks_b = b.get("ignore_marks", '0' * len(tokens_b))
            matches = greedy_string_tiling(tokens_a, marks_a, tokens_b, marks_b, minimum_match_length)
            avg_unique_tokens = (a["authored_token_count"] + b["authored_token_count"]) / 2
            similarity = matches.token_count() / avg_unique_tokens if avg_unique_tokens > 0 else 0
        if similarity > minimum_similarity:
            yield [a["id"], b["id"], matches.json(), optional_round(similarity)]


def match_all_combinations(config, string_data_iter):
    """
    Given a configuration dict and an iterable of string data, do string similarity comparisons for all 2-combinations without replacement for the input data.
    Return an iterator over matches.
    """
    return _match_all(config, itertools.combinations(string_data_iter, 2))


def match_to_others(config, string_data, other_data_iter):
    """
    Compare one string data object to all other objects in other_data_iter.
    Return an iterator over matches.
    """
    return _match_all(config, ((string_data, other_data) for other_data in other_data_iter))
