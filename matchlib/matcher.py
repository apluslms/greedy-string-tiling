import itertools

from matchlib.matchers import greedy_string_tiling
from matchlib.util import safe_div


def match_all(config, string_data_iter):
    """
    Given a configuration dict and an iterable of string data, return an iterator over all comparisons of the input data.
    Objects are compared only once, i.e. all comparison pairs are 2-combinations without replacement.
    """
    minimum_match_length = config.get("minimum_match_length", 1)
    minimum_similarity = config.get("minimum_similarity", -1)
    for a, b, in itertools.combinations(string_data_iter, 2):
        tokens_a, tokens_b = a["tokens"], b["tokens"]
        if max(len(tokens_a), len(tokens_b)) < minimum_match_length:
            continue
        marks_a, marks_b = a.get("marks", ''), b.get("marks", '')
        matches = greedy_string_tiling(tokens_a, marks_a, tokens_b, marks_b, minimum_match_length)
        similarity = safe_div(matches.token_count(), (len(tokens_a) + len(tokens_b)) / 2)
        if similarity > minimum_similarity:
            yield { "id_a": a["id"],
                    "id_b": b["id"],
                    "similarity": similarity }
