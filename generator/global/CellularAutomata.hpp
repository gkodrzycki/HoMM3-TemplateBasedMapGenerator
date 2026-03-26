#pragma once
#include "../map/Map.hpp"
#include "./Global.hpp"

struct AutomataConfig {
    int width;
    int height;
    float initialFillProbability;
    int birthLimit;
    int survivalLimit;
    int smoothingIterations;
};

class CellularAutomata {
  public:
    CellularAutomata(const AutomataConfig &config);
    ~CellularAutomata() = default;

    void generate(Map &map);

    const std::vector<bool> &getGrid() const { return m_grid; }

  private:
    int countAliveNeighbors(int x, int y) const;
    void doSimulationStep();

    AutomataConfig m_config;
    std::vector<bool> m_grid;
    std::vector<bool> m_tempGrid;
};
