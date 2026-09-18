# PostalCodesPL

[![Release](https://img.shields.io/github/v/release/mi-bb/PostalCodesPL?include_prereleases&style=)](https://github.com/mi-bb/PostalCodesPL/releases/)
![Licence](https://img.shields.io/github/license/mi-bb/PostalCodesPL)
![Last Commit](https://img.shields.io/github/last-commit/mi-bb/PostalCodesPL)
[![SQLite](https://img.shields.io/badge/SQLite-%2307405e.svg?logo=sqlite&logoColor=white)](https://sqlite.org)

Database with Polish postal codes.

This project contains an SQLite database file with details of Polish postal codes
(city names, street names, voivodeship info).
The package contains examples in C and Python showing how to use the database.

<img src="other/screen_1.png" alt="Screenshot 1" style="height: 250px;"><img src="other/screen_2.png" alt="Screenshot 2" style="height: 250px;">

## Requirements

* SQLite 3

OS:

* GNU/Linux
* Windows

### Requirements (C examples)

* GCC

### Requirements (Python examples)

* Python 3
* GTK+ 3

## PostalCodesPL (polski)

Baza danych polskich kodów pocztowych.

Jest to baza danych SQLite zawierająca informacje o polskich kodach
pocztowych - kody oraz dane o miejscowościach i ulicach przypisanych
do tych kodów.
W paczce są przykładowe zastosowania bazy w Pythonie i C.
Za pomocą programu codes1 (codes1.py) po wpisaniu kodu pocztowego
zostają wyświetlone wszystkie informacje o tym kodzie.
W programie codes2 (codes2.py) można wyszukać kod pocztowy wybranej
miejscowości, województwa i ulicy.
W programie postc (postc.c) można przeglądać informacje o kodach
pocztowych podając kod pocztowy lub nazwę miejscowości.

## Copyright and License

Copyright (C) 2019-2026 Michal Babik

This project is licensed under the GPL-3.0 License — see [COPYING](COPYING) for details.
