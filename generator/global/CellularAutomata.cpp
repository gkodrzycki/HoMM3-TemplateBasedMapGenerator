#include "CellularAutomata.hpp"

CellularAutomata::CellularAutomata(const AutomataConfig &config)
    : m_config(config), m_grid(config.width * config.height, false),
      m_tempGrid(config.width * config.height, false) {}

void CellularAutomata::generate(Map &map) {

    // Read from class Map objects of border and set them as alive to create a natural boundary
    for (int y = 0; y < m_config.height; ++y) {
        for (int x = 0; x < m_config.width; ++x) {
            auto tilePtr = map.getTile(int3(x, y, 0));
            if (tilePtr != nullptr && tilePtr->isTileType("B")) {
                m_grid[y * m_config.width + x] = (map.getRNG().nextFloat() < 0.7f);
            } else {
                m_grid[y * m_config.width + x] =
                    (map.getRNG().nextFloat() < m_config.initialFillProbability);
            }
        }
    }

    for (int i = 0; i < m_config.smoothingIterations; ++i) {
        doSimulationStep();
    }
}

int CellularAutomata::countAliveNeighbors(int x, int y) const {
    int count = 0;
    // Ewaluacja pełnego sąsiedztwa Moore'a
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0)
                continue;

            int nx = x + i;
            int ny = y + j;

            // Interpretacja krańców przestrzeni jako mur strukturalny
            if (nx < 0 || nx >= m_config.width || ny < 0 || ny >= m_config.height) {
                count++;
            } else if (m_grid[ny * m_config.width + nx]) {
                count++;
            }
        }
    }
    return count;
}

void CellularAutomata::doSimulationStep() {
    for (int y = 0; y < m_config.height; ++y) {
        for (int x = 0; x < m_config.width; ++x) {
            int aliveNeighbors = countAliveNeighbors(x, y);
            int index          = y * m_config.width + x;

            if (m_grid[index]) {
                m_tempGrid[index] = (aliveNeighbors >= m_config.survivalLimit);
            } else {
                m_tempGrid[index] = (aliveNeighbors >= m_config.birthLimit);
            }
        }
    }
    // Nadpisanie w czasie stałym dla relokacji wskaźników po swap wektorów pod spodem
    m_grid.swap(m_tempGrid);
}
