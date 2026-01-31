# HoMM3-MapGen-v2 - PROGRESS_FINAL

## TL;DR
Projekt generuje grywalne mapy Heroes of Might and Magic III (format `.h3m`, pod VCMI) na podstawie prostego szablonu JSON (`layout.json`). Pipeline wygląda tak:

`layout.json` → C++ (`Generator`) → abstrakcja `Map` + "placery" (regiony/strefy, miasta, drogi, granice, kopalnie, skarby, strażnicy) → wygenerowany skrypt `generated_script.lua` → wykonanie Lua → zapis `.h3m`.

W repo są także narzędzia do przetwarzania danych o jednostkach (`CRTRAITS.*`) i test/utility `Units`, który buduje mapę testową z rozstawionymi stworami.
---

## 1. Co robi generator
- Wczytuje szablon mapy (`layout.json`) opisujący regiony, strefy (zones) i połączenia.
- Przypisuje każdemu polu mapy identyfikator strefy i typ terenu.
- Umieszcza kluczowe obiekty: miasta startowe, kopalnie podstawowe (tartak + kopalnia rudy), skarby (artefakty), przeszkody graniczne oraz strażników.
- Łączy strefy drogami po siatce (pathfinding BFS).
- Eksportuje całość do Lua i wykonuje, aby zapisać finalną mapę `.h3m`.

---

## 2. Struktura repo

Najważniejsze elementy:

- `layout.json` - wejściowy szablon mapy.
- `generator/Generator.cpp` - entrypoint, CLI, uruchomienie pipeline’u oraz generacja/wykonanie Lua.
- `generator/layoutInfo/*` - parser JSON → `LayoutInfo/RegionInfo/ZoneInfo/ConnectionInfo`.
- `generator/map/Map.*` - główna abstrakcja mapy (siatka `Tile`, obiekty, strefy, regiony).
- `generator/map/placers/*` - moduły rozmieszczania elementów mapy.
- `generator/mapInfo/*` - modele obiektów: `Town`, `Mine`, `Creature`, `Road`, `Treasure` (Artifact/Resource), `Obstacle`, `Tile`.
- `generator/global/*` - narzędzia ogólne: `Random`, `GridSearch` (BFS/Dijkstra/A*), typy `int3`.
- `generator/luaUtils/luaHelpers.*` - eksport mapy do Lua (wywołania API homm3lua).
- `CRTRAITS.*` + `scripts/convert_crtraits.py` - dane i konwersja parametrów jednostek, w tym `fight_value`.
- `tests/Units.cpp` - program testowy do sprawdzenia, że jednostki/creature placement działa dla dużego spektrum stworów.

---

## 3. Format wejściowy: `layout.json`

`layout.json` jest jedynym obowiązkowym wejściem generatora.

### 3.1. Struktura
- `debug` (int, opcjonalne) - poziom debugu (im większy, tym dokładniej wypisujemy stan mapy po każdej operacji).
- `name`, `description` (string) - metadane mapy.
- `size` (string: `S|M|L|XL`) - rozmiar mapy.
- `difficulty` (string, np. `easy`) - trudność (tylko opisowa dla gracza, nie ma faktycznego wpływu na mapę).
- `regions` - lista regionów, każdy z:
  - `id` (int), `name` (string)
  - `zones` - lista stref; każda strefa ma:
    - `id` (int)
    - `terrain` (np. `grass`, `snow`, …)
    - `faction` (np. `Stronghold`, `Rampart`)
    - `owner_id` (string liczbowy, np. `"1"`)
    - `type` (np. `starting_zone`, `support_zone`)
    - (opcjonalnie) `size` i inne pola dziedziczone z regionu przez fallback.
- `connections` - lista połączeń stref:
  - `from_zone` (int)
  - `dest_zone` (int)

### 3.2. Fallback wartości z regionu do strefy
`ZoneInfo::deserializeZone` używa `getWithRegionFallback`, który:
- bierze wartość z obiektu strefy jeśli jest,
- w przeciwnym razie używa wartości domyślnej z regionu,
- jeśli nie ma żadnej z nich - rzuca wyjątek.

---

## 4. Build i uruchamianie

### 4.1. Zależności
Z README:
- `Lua 5.4`
- `qt5-tools`
- `boost`
- `nlohmann/json`
- `magic-enum`

W praktyce generator linkuje się z Lua (`-llua`), a eksport do `.h3m` jest robiony przez Lua (via `homm3lua` w `.so`).

### 4.2. Budowanie
Makefile udostępnia:
- `make` - buduje: `dist/homm3lua.so`, `Generator`, `Units`.
- `make run` - buduje `Generator` i uruchamia ().
- `make units` - buduje `Units` i uruchamia (nasz test, czy potrafimy generować wszystkie jednostki).
- `make format` / `make format-check`
- `make lint`
- `make clean` - Czyści pliki `.o` z folderu `generator`
- `make hard_clean` - usuwa wszystkie pliki `.o`

### 4.3. Uruchamianie generatora
Binary: `./Generator`

Obsługiwane argumenty:
- `--seed <int>` - ustawia seed RNG.
- `--location <path>` - gdzie zapisać `.h3m`.

Domyślna lokalizacja wyjścia (gdy brak `--location`):
- `~/.local/share/vcmi/Maps/test.h3m`

Generator tworzy `generated_script.lua` i go wykonuje.

### 4.4. Program `Units`
Binary: `./Units`

Argumenty:
- `--location <path>` - docelowa ścieżka `.h3m`.
- `--crtraits <path>` - ścieżka do `CRTRAITS.json`.

Domyślne wyjście:
- `~/.local/share/vcmi/Maps/units.h3m`

Cel: masowe sprawdzenie, czy tworzenie obiektów `Creature` i eksport do Lua działa dla listy stworów w `CRTRAITS.json`.

---

## 5. Pipeline generacji: od layoutu do `Map`

Cały pipeline jest zebrany w `Map::generateMap()`:

1) `initMap()`
- Inicjuje siatkę `Tile` o rozmiarze zależnym od `layoutInfo.size`.
- Losuje `basicResourceCount[4]` (0..15) - ilości surowców towarzyszących kopalniom bazowym.

2) `RegionPlacer`
- `generateRegions()`:
  - Tworzy `Region` i `Zone` na podstawie `LayoutInfo`.
  - Ustawia centra stref na bazie algorytmu zachłannego.
  - Na podstawie wyniku poprzedniego algorytmu wyznaczamy strefę kazdego Tile'a

3) `TownPlacer::placeTowns()`
- Dla stref `type == "starting_zone"` tworzy `Town` w centrum strefy.
- Oznacza sąsiedztwo poprzez `Map::fixNeighbourTiles` (część jako `TILE_TAKEN` / `TILE_RESERVED`).
- Wyznacza „wejście” do miasta i oznacza je `TILE_OCCUPIED`.

4) `RoadPlacer::placeRoads()`
- Wyznacza punkty połączeń dla stref:
  - dla stref z miastem: wejście do miasta,
  - dla pozostałych: środek strefy.
- Dla każdej deklaracji w `layout.connections` znajduje ścieżkę BFS po siatce:
  - zakazuje przechodzenia przez tile `TILE_TAKEN`.
- Dla znalezionej ścieżki:
  - dodaje obiekt `Road` z `tier=1`,
  - oznacza wszystkie tile na ścieżce jako `TILE_ROAD`.

5) `BorderPlacer::reserveBorderTiles()`
- Wyznacza granice stref:
  - tile jest graniczny, jeśli w sąsiedztwie 8-kierunkowym jest tile o innym `zoneID`.
- Oznacza granice:
  - `TILE_BORDER_INNER` dla samych granicznych,
  - `TILE_BORDER_OUTER` dla rozszerzenia BFS o głębokość 1.
- Rezerwacje nie nadpisują obiektów/miast/drog i podobnych.

6) `ObjectPlacer`
- `placeBasicMines()`:
  - dla każdego miasta losuje dwa punkty (B i C) w pobliżu (losowy trójkąt o obwodzie 30),
  - wymusza, by były w tej samej strefie co miasto oraz w granicach mapy,
  - stawia:
    - tartak (`MINE_SAWMILL`) + 2x drewno obok,
    - kopalnię rudy (`MINE_ORE_PIT`) + 2x ruda obok,
  - zajmuje pola surowców `TILE_OCCUPIED`.
- `placeTreasures()`:
  - zbiera wszystkie tile typu `TILE_FREE` per strefa,
  - losuje 3..5 skarbów na strefę,
  - losuje tier artefaktów (LOW/MID/HIGH),
  - wybiera wolny tile i stawia artefakt, oznaczając tile jako `TILE_OCCUPIED`.

7) `GuardPlacer::placeGuards()`
- `placeBorderGuards()`:
  - dla każdej drogi wybiera punkt na ścieżce, który ma najwięcej sąsiadów typu `TILE_BORDER_INNER` (heurystyka „wąskiego gardła”),
  - stawia tam strażnika typu BORDER.
- `placeMineGuards()`:
  - dla każdej kopalni stawia strażnika na wejścu.
- Dobór „mocy”:
  - border: targetPower=40000, poziom 6–7, upgrade=true,
  - mine:   targetPower=4000,  poziom 1–2.
- Dobór liczby jednostek:
  - wczytuje `CRTRAITS.json`, znajduje `fight_value` dla wylosowanego stworzenia,
  - `count = max(1, targetPower / fight_value)`.

8) `BorderPlacer::placeBorders()`
- Przechodzi po całej mapie i na tile typu `Border Inner`/`Border Outer` stawia przeszkodę `Pine Trees`.
- Ma wyjątek: jeśli tile nad danym polem jest drogą - omija (żeby nie „zatykać” drogi przeszkodą).

---

## 6. Eksport do Lua i zapis `.h3m`

Eksport jest w `generator/luaUtils/luaHelpers.*` i uruchamiany przez `generator/Generator.cpp`.

Generator:
- tworzy plik `generated_script.lua`,
- dodaje nagłówek `AddHeader` (ustawienie `package.cpath` i `require('homm3lua')`),
- tworzy instancję mapy w Lua: `homm3lua.new(..., homm3lua.SIZE_...)`,
- ustawia metadane (nazwa/opis/trudność),
- dokłada teren, obiekty i drogi,
- na końcu wywołuje `instance:write('<ścieżka>')`.

---

## 7. Debugowanie

- `layout.debug` steruje logami:
  - `>1`: `LayoutInfo.printLayout()`
  - `>0`: `Map.printMap(debug)` wypisuje siatkę kafli z kolorami/znakami.
- Tile debug char (z `TileType`):
  - `F` free
  - `T` taken
  - `R` reserved
  - `O` occupied
  - `r` road
  - `B` border inner
  - `b` border outer

---

## 9. Dane `CRTRAITS.*` i skrypt konwersji

- `scripts/convert_crtraits.py` konwertuje `CRTRAITS.TXT` → `CRTRAITS.csv` i/lub `CRTRAITS.json`.
- `CRTRAITS.json` jest używany przez `GuardPlacer` do doboru liczby jednostek na podstawie `fight_value` (wzorowane na VCMI).

Przykład użycia:
```bash
python3 scripts/convert_crtraits.py --input CRTRAITS.TXT --out-csv CRTRAITS.csv --out-json CRTRAITS.json
```

Wartości pobrane bezpośrednio z klienta VCMI.

---

## 10. Roadmapa / dalsze plany (zebrane + doprecyzowane)

1) Rozbudowa połączeń
- portale/monolity, alternatywne typy połączeń.

2) Lepsze granice
- nieregularność krawędzi, kontrola „przejść granicznych”, mniej „ścian drzew” (inne obiekty na granicach, jak góry itp.).

3) Content placement
- skarby (skrzynki/surowce), obiekty ekonomiczne, „punkty zainteresowania”, lepszy rozkład kopalń.
- powiązanie z trudnością strefy i strażnikami.

4) Mutowanie map i ewaluacja
- architektura do generowania wariantów (mutacje) + metryki balansu/fairness (pierwotny pomysł z PROGRESS_0).
