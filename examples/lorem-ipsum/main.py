import collections
import random
random.seed(42)

import gst
import jinja2

# Let match_edge = {
#   < 0, if starting pos of a match
#   > 0, if ending pos of a match
#   = 0, otherwise
# }
Char = collections.namedtuple("Char", ("char", "match_edge", "match_number"))

output_html_path = "index.html"
min_match_length = 50
num_match_colors = 4

# Load data
with open("text1.txt") as f:
    text1 = f.read()
text2 = ''
for line in text1.splitlines():
    text2 += ' '.join(l for l in line.split(' ') if random.random() > 0.05)

# Get matches and sort in ascending order by starting indexes of matches in text1
match_list = gst.match(text1, '', text2, '', min_match_length)
match_list.sort()

# Annotate every character that is the first or last character of a match
# This makes it trivial to generate HTML around these positions
text1 = [Char(char=c, match_edge=0, match_number=0) for c in text1]
text2 = [Char(char=c, match_edge=0, match_number=0) for c in text2]
for match_num, (i1, i2, length) in enumerate(match_list, start=1):
    text1[i1] = Char(char=text1[i1].char, match_edge=-1, match_number=match_num)
    text2[i2] = Char(char=text2[i2].char, match_edge=-1, match_number=match_num)
    text1[i1+length-1] = Char(char=text1[i1+length-1].char, match_edge=1, match_number=match_num)
    text2[i2+length-1] = Char(char=text2[i2+length-1].char, match_edge=1, match_number=match_num)

template = jinja2.Environment(loader=jinja2.FileSystemLoader(".")).get_template("template.html")
html = template.render(
        text1=text1,
        text2=text2,
        num_match_colors=num_match_colors,
        min_match_length=min_match_length)

with open(output_html_path, "w") as out_f:
    print(html, file=out_f)
print("wrote", output_html_path)
