import json


class TokenMatchSet:

    def __init__(self):
        self.store = []

    def extend(self, match_set):
        self.store.extend(match_set.store)

    def add(self, match):
        self.store.append(match)

    def add_non_overlapping(self, match):
        if any(match.overlaps(m) for m in self.store):
            return False
        self.store.append(match)
        return True

    def clear(self):
        del self.store[:]

    def all(self):
        return self.store

    def reverse(self):
        r = TokenMatchSet()
        r.store.extend(m.reversed() for m in self.store)
        return r

    def match_count(self):
        return len(self.store)

    def token_count(self):
        return sum(m.length for m in self.store)

    def json(self):
        sorted_match_list = [[m.a, m.b, m.length] for m in sorted(self.store, key=lambda m: m.a)]
        return json.dumps(sorted_match_list, separators=(",", ":"))


class TokenMatch:

    def __init__(self, a, b, length):
        self.a = a
        self.b = b
        self.length = length

    def overlaps(self, another):
        return ((self.a > another.a - self.length and self.a < another.a + self.length)
                or
                (self.b > another.b - self.length and self.b < another.b + self.length))

    def reversed(self):
        return TokenMatch(self.b, self.a, self.length)


def safe_div(a, b):
    return a / b if b > 0 else 0.0


def top_marks(length, top):
    for _ in range(top):
        yield True
    for _ in range(top, length):
        yield False


def bitstring(bools):
    """
    Return a string of bits from an iterable over booleans.
    """
    return ''.join(str(int(b)) for b in bools)
