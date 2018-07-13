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

    @classmethod
    def full_match_from_length(cls, l):
        ms = cls()
        ms.add_non_overlapping(TokenMatch(0, 0, l))
        return ms


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
