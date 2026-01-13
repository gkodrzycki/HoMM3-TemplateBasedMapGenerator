# Co zostało zrobione i dalsze plany
## Dodanie granic [65b2557d057d0b56fc5b2fc862f79b6841afa4bf]
Wyznaczanie pól granicznych i pól w danym promieniu, stawianie przeszkody (Pine Tree) na każdym z tych pól.

Do rozważenie wprowadzenie jakichś nieregularności/nierówności na samych krawędziach stref.

## Dodanie połączeń (dróg) [0e41be7f4abf72126d20d434c59b609dbdf8a0d7]
Drogi łączące połączone strefy są łączone ścieżkami od zamku do zamku lub środka strefy. Połączenia uwzględniają przeszkody neutralne i zamki (nie kolidują z nimi), a te wychodzące z zamków dbają o to, żeby wychodziły z faktycznego wejścia/wyjścia.

Dodanie handlowania portali/innych typów połączeń? Na razie mamy tylko podstawowe drogi

## Umożliwienie stawiania potworów i ich ewaluacja [d097e8b8374c5040d80be57c187eb17293cf4b36]
Rozwiązanie problemu API z niestawianiem niektórych potworów. Napisanie funkcji umożliwiającej postawienie przeciwników z danymi parametrami na zdefiniowanych pozycjach. Dodatkowo spisanie plików `CRTRAITS.*` z `fight_value` wyliczanymi na podstawie wzoru z vcmi biorącego pod uwagę moc i inne statystyki.

W planach sensowne umieszczenie (na przejściach granicznych, przy skarbach i kopalniach). Ustalenie jakoś poziomu trudności strefy i umieszczanie odpowiednio trudnych przeciwników.

## Dodanie bazowych kopalni [In progresss on basic_mines and basic-mines-tests]
Stawianie podstawowych kopalni (ore mine, sawmill) w okolicy zamków. Obecnie tworzymy trójkąt o danym obwodzie i na jego wierzchołkach stawiamy budynki. Jeśli losowanie nie powiedzie się za pierwszym razem, to próbujemy ponownie (max 100 razy).

W planach chyba będzie zmiana kolejności etapów generowania (chcemy dać stawianie kopalni wyżej), dzięki czemu zamiast odrzucać jakieś losowanie, może uda nam się lekko zmodyfikować mapę, umożliwiając postawienie, ale bez zaburzania balansu (na przykład, gdy kopalnia jest blisko krawędzi, możemy nieco przesunąć granicę co dodatkowo wprowadzi lekkie urozmaicenie itp.).

# Bardziej odległe, dalsze plany
1) Ulepszenie stawiania miast/wyznaczania środka strefy (prawdopodobnie użycie czegoś podobnego do [tego](https://github.com/Sko0owi/HOMM3-mapgen/tree/main/generator))
2) Przygotowanie architektury pod mutowanie.
3) Dodanie jakichś losowych skarbów gdzieś na mapie

# Trudności
Chyba nic niezwykłego, czasem problemy przy zdecydowaniu się co ma być w templatce, a co nie itd.:D

# Jaka szansa, że się uda
To nie jest tak, że się uda albo, że się nie uda... Przede wszystkim liczy się przygoda i droga, którą przebyliśmy wspólnie!
A tak to z mocą przyjaźni realnie powinno się udać wygenerować prostą mapę tzn. mapa zawiera podstawowe kopalnie (w strefach, gdzie są zamki z graczami), skarby losowo rozmieszczone w zonie (skrzynki/surowce) oraz strażnicy pomiędzy zonami. Wszystkie wymagane strefy oczywiście mają być połączone.
