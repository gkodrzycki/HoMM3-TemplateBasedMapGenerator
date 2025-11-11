# HOMM3 map generator & fairness evaluation — PROGRESS_0

### Repo bazowe: 
**Sko0owi/HOMM3-mapgen**

### Cel ogólny: 
Generator map do Heroes of Might and Magic III + zestaw botów/heurystyk do obiektywnej (powtarzalnej) ewaluacji „sprawiedliwości” map dla graczy/lokacji startowych.

## 1) Zakres projektu (MVP)

#### Integracja z repo bazowym map generatora:

- Uruchomienie pipeline’u generowania map (seed → .h3m lub format roboczy). (planowany jest gruntowny refactor kodu napisanego w bazowym repo)

- Możliwość deterministycznego odtwarzania (seed, wersje).

#### Warstwa analityki statycznej (bez symulacji tury-za-turą):

- Parser map → graf regionów, POI (Point of Interest), łupy, wrogowie neutralni, dystanse (edge cost).

- „Lustrzane” porównywanie stref startowych graczy.

#### Boty-heurystyki lekkie (MVP):

- Bot-Eksploracja: szacuje czas/dystans do POI oraz priorytetyzację ścieżek (greedy z ograniczeniami).

- Bot-Walka (trudność): estymator „siły neutralnych” vs. typowe armie w turach 1–3 (model punktowy/krzywe kosztu).

- Bot-Ekonomia: szacuje czas do osiągnięcia poszczególnego poziomu rozwoju w mieście.

#### Raport fairness (HTML/Markdown):

- Tabele metryk per gracz + wykresy + jeden łączny F-score.
