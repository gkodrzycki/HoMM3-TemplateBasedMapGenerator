#pragma once
#include "../map/Map.hpp"
#include "./Global.hpp"

class Map;

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
    void generateSmallRectangle(Map &map, int width, int height, int offsetX, int offsetY);

    const std::vector<bool> &getGrid() const { return m_grid; }

  private:
    int countAliveNeighbors(int x, int y) const;
    void doSimulationStep();
    void doSimulationStepSmallRectangle(int width, int height, int offsetX, int offsetY);
    void eraseTooSmallComponents();

    AutomataConfig m_config;
    std::vector<bool> m_grid;
    std::vector<bool> m_tempGrid;
};
