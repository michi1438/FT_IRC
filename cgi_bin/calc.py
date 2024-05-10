#!/usr/bin/env python3
import os
import sys

error = """
        <br>
        <br>
        <br>
        <h2>ERROR BATARD</h2>
        <h7 style="color: red">(le prend pas mal hein, c'est juste pour rire, je t'aime bien quand même)</h7>
        """
def print_res(text):
    with open('html/index.html', 'r') as file:
        base_html = file.read()
    res = base_html.replace('<!-- contenu dynamique -->', text)
    print(res)

def err():
    print_res(error)
    sys.exit(1)

try:
    num1 = int(os.environ.get('QUERY_STRING').split('&')[0].split('=')[1])
    num2 = int(os.environ.get('QUERY_STRING').split('&')[1].split('=')[1])
except ValueError:
    err()

oprtr = os.environ.get('QUERY_STRING').split('&')[2].split('=')[1]

if num1 == 0 and num2 == 0 and oprtr == '+':
    b = "La tete a Toto"
elif oprtr == '+':
    b = num1 + num2
elif oprtr == '-':
    b = num1 - num2
elif oprtr == '*':
    b = num1 * num2
elif oprtr == '/' and num2 != 0:
    b = num1 / num2
else:
    err()

success = "<br><br><br><h3><p>{0} {3} {1} = {2}</p></h3>".format(num1, num2, b, oprtr)

print_res(success)
