# HOMM3 map generator & fairness evaluation — PROGRESS_1


## Co zostało zrobione:
### 1) Implementacja layoutInfo
Dodanie klasy, która jest w stanie sparsować plik json opisujący mapę.

### 2) Implementacja generateMap
Dodanie warstwy abstrakcji, która jest odpowiedzialna za przechowywanie kluczowych elementów mapy pozyskanych z `layoutInfo`. Jest łącznikiem między naszą abstrakcją, a fizyczną reprezentacją mapy.

### 3) Implementacja Map
#### 3.1) Implementacja generateRegions
Tworzy wstępny zestaw regionów na podstawie `LayoutInfo`, przypisując im identyfikatory, typy i domyślne atrybuty (np. frakcję i właściciela).
Wynik jest przechowywany w strukturach `RegionMap`/`ZoneMap` jako punkt wyjścia dla dalszego rozmieszczania.


#### 3.2) Implementacja placeRegions
Rozmieszcza regiony na siatce mapy.
Aktualizuje kafelki (`TileMap`) tak, aby odzwierciedlały przynależność do konkretnych regionów/stref.

#### 3.3) Implementacja placeTowns
Umieszcza miasta w strefach startowych zgodnie z danymi z layoutu (frakcja, właściciel, strefa, nazwa).
Dodane obiekty trafiają do wektora obiektów mapy i mogą być wypisane/serializowane dalej.

### 4) Implementacja generateLuaScript & executeLuaScript
Generuje skrypt Lua odzwierciedlający aktualny stan mapy (regiony, strefy, obiekty) z użyciem modułu `homm3lua`.
Uruchamia wygenerowany skrypt, aby uzyskać końcową mapę (w formacie `.h3m`).

## Dalsze plany
### 1) Dodanie granic
### 2) Dodanie połączeń (dróg)
### 3) Dodanie bazowych kopalni
### 4) Ulepszenie stawiania miast/wyznaczania środka strefy
### 5) Zaimplementowanie podstawowego mutowania mapy
