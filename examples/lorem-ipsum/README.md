# random lorem ipsum greedy string tiling

`text1` is an extract from [*De finibus bonorum et malorum*](https://la.wikisource.org/wiki/De_finibus_bonorum_et_malorum/Liber_Primus) (Book 1) from Cicero, on Latin WikiSource.

`text2` is a copy of `text1`, from which words have been removed randomly with probability 0.05.

For generating the HTML example seen below, install `greedy-string-tiling` and [Jinja2](https://jinja.palletsprojects.com/en/2.11.x/), then run `main.py`:
```
pip install ../.. 'Jinja2~=2.11.2'
python main.py
```

## Example

![two columns of text side by side, with matching substrings highlighted using a background color](./example.png)
