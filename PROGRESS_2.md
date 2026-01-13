# Co zostało zrobione i dalsze plany
## Dodanie granic [65b2557d057d0b56fc5b2fc862f79b6841afa4bf]
Wyznaczamy pola graniczne i pola w danym promieniu, a następnie stawiamy przeszkodę (Pine Tree) na każdym z nich.

Do rozważenia jest wprowadzenie nieregularności/nierówności na samych krawędziach stref.

## Dodanie połączeń (dróg) [0e41be7f4abf72126d20d434c59b609dbdf8a0d7]
Strefy są łączone drogami wyznaczanymi ścieżkami od zamku do zamku lub do środka strefy. Połączenia uwzględniają przeszkody neutralne i zamki (nie kolidują z nimi), a odcinki wychodzące z zamków startują z faktycznego wejścia/wyjścia.

Dalszy plan to dodanie obsługi portali/innych typów połączeń. Na razie są tylko podstawowe drogi.

## Umożliwienie stawiania potworów i ich ewaluacja [d097e8b8374c5040d80be57c187eb17293cf4b36]
Rozwiązaliśmy problem API, przez który część potworów się nie pojawiała. Dodaliśmy funkcję pozwalającą stawiać przeciwników z zadanymi parametrami w określonych pozycjach. Powstały też pliki `CRTRAITS.*` z `fight_value` liczonym na podstawie wzoru z VCMI uwzględniającego moc i inne statystyki.

W planach sensowne rozmieszczenie (na przejściach granicznych, przy skarbach i kopalniach) oraz ustalenie poziomu trudności strefy, by dobierać odpowiednio wymagających przeciwników.

## Dodanie bazowych kopalni [In progress on basic_mines and basic-mines-tests]
Stawiamy podstawowe kopalnie (ore mine, sawmill) w okolicy zamków. Tworzymy trójkąt o zadanym obwodzie i na jego wierzchołkach umieszczamy budynki. Jeśli losowanie się nie powiedzie za pierwszym razem, próbujemy ponownie (maksymalnie 100 razy).

Planujemy podnieść etap stawiania kopalni w kolejności generowania. Zamiast odrzucać niektóre nieudane losowania, być może lekko zmodyfikujemy mapę, by umożliwić postawienie budynku bez zaburzania balansu (np. gdy kopalnia jest blisko krawędzi, można minimalnie przesunąć granicę, co dodatkowo wprowadzi urozmaicenie).

# Bardziej odległe, dalsze plany
1) Ulepszenie stawiania miast/wyznaczania środka strefy (prawdopodobnie użycie czegoś podobnego do [tego](https://github.com/Sko0owi/HOMM3-mapgen/tree/main/generator)).
2) Przygotowanie architektury pod mutowanie.
3) Dodanie kilku losowych skarbów na mapie.

# Trudności
Chyba nic niezwykłego, czasem problemy przy zdecydowaniu się co ma być w templatce, a co nie itd.:D

# Jaka szansa, że się uda
To nie jest tak, że się uda albo że się nie uda... Przede wszystkim liczy się przygoda i droga, którą przebyliśmy wspólnie!
A z mocą przyjaźni realnie powinniśmy wygenerować prostą mapę: z podstawowymi kopalniami (w strefach z zamkami graczy), losowo rozmieszczonymi skarbami w zonie (skrzynki/surowce) oraz strażnikami pomiędzy zonami. Wszystkie wymagane strefy oczywiście mają być połączone.
