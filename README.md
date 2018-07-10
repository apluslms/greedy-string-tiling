# Greedy String Tiling

This Python package implements a C++ extension of the Running Karp-Rabin Greedy String Tiling algorithm [1], which can be used for source code similarity detection as has been done with the JPlag system [2].

## Installing

Without tests:
```
pip install git+https://github.com/Aalto-LeTech/greedy-string-tiling.git
```

With tests:
```
git clone --depth 1 https://github.com/Aalto-LeTech/greedy-string-tiling.git
cd greedy-string-tiling
pip install .
python3 test.py
```

## References

[1] Wise, M.J., 1993. String similarity via greedy string tiling and running Karp-Rabin matching. Online Preprint, Dec, 119. https://www.researchgate.net/publication/262763983_String_Similarity_via_Greedy_String_Tiling_and_Running_Karp-Rabin_Matching (Accessed 24th May 2018)

[2] Prechelt, L., Malpohl, G. and Philippsen, M., 2002. Finding plagiarisms among a set of programs with JPlag. J. UCS, 8(11), p.1016. https://page.mi.fu-berlin.de/prechelt/Biblio/jplagTR.pdf (Accessed 24th May 2018)

