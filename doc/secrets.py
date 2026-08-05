import sys
import requests
from bs4 import BeautifulSoup

def print_secret_message(url):
    page = requests.get(url)
    parsed = BeautifulSoup(page.text, "html.parser")
    message = {}
    max_x = 0
    max_y = 0

    rows = parsed.find_all("tr")[1:]
    for row in rows:
        cells = row.find_all("td")

        x = int(cells[0].get_text())
        char = cells[1].get_text()
        y = int(cells[2].get_text())

        message[(x, y)] = char
        max_x = max(max_x, x)
        max_y = max(max_y, y)

    for y in range(max_y, -1, -1):
        line = ""

        for x in range(max_x + 1):
            line += message.get((x, y), " ")

        print(line)

if __name__ == "__main__":
    print_secret_message(sys.argv[1])
