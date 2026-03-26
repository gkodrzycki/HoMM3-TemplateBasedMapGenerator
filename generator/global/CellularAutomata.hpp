#pragma once
#include "../map/Map.hpp"
#include "./Global.hpp"

struct AutomataConfig {
    int width;
    int height;
    float initialFillProbability; // Często w przedziale 0.4 - 0.55 [16]
    int birthLimit;               // Rekomendowane: 5 [15, 18]
    int survivalLimit;            // Rekomendowane: 4 [15, 18]
    int smoothingIterations;      // Ilość przebiegów ewolucyjnych (zwykle 4-5)
};

class CellularAutomata {
  public:
    CellularAutomata(const AutomataConfig &config);
    ~CellularAutomata() = default;

    // Główna procedura ewoluująca początkowy szum w organiczne struktury
    void generate(Map &map);

    // Eksport wygładzonej mapy: true oznacza obecność masywnego przeszkodzenia, false to pustka
    const std::vector<bool> &getGrid() const { return m_grid; }

  private:
    int countAliveNeighbors(int x, int y) const;
    void doSimulationStep();

    AutomataConfig m_config;
    std::vector<bool> m_grid;
    std::vector<bool> m_tempGrid; // Podwójne buforowanie zapobiega brudnym terytoriom
};
