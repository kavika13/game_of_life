input = [
    "...................*........",
    "..................***.......",
    ".............***....**......",
    "............*..***..*.**....",
    "...........*...*.*..*.*.....",
    "...........*....*.*.*.*.**..",
    ".............*....*.*...**..",
    ".****.....*.*....*...*.***..",
    ".*...**.*.***.**.........**.",
    ".*.....**.....*.............",
    "..*..**.*..*..*.**..........",
    "........*.*.*.*.*.*.....****",
    "..*..**.*..*..*..**.*.**...*",
    ".*.....**...*.*.*...**.....*",
    ".*...**.*.**..*..*..*.**..*.",
    ".****.....*.*.*.*.*.*.......",
    "...........**.*..*..*.**..*.",
    "..............*.....**.....*",
    "..**.........**.***.*.**...*",
    "...***.*...*....*.*.....****",
    "...**...*.*....*............",
    "...**.*.*.*.*....*..........",
    "......*.*..*.*...*..........",
    ".....**.*..***..*...........",
    ".......**....***............",
    "........***.................",
    ".........*..................",
]

for y, row in enumerate(input):
    for x, cell in enumerate(row):
        if cell == '*':
            print('({}, {})'.format(x, y))
