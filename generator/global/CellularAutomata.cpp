#include "CellularAutomata.hpp"

CellularAutomata::CellularAutomata(const AutomataConfig &config)
    : m_config(config), m_grid(config.width * config.height, false),
      m_tempGrid(config.width * config.height, false) {}

void CellularAutomata::generate(Map &map) {

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

    eraseTooSmallComponents();
}

void CellularAutomata::eraseTooSmallComponents() {
    bool visited[m_config.width * m_config.height];
    std::fill(visited, visited + m_config.width * m_config.height, false);

    auto passable = [&](const int3 &pos) { return m_grid[pos.y * m_config.width + pos.x]; };

    auto neighbours = [&](const int3 &pos) {
        std::array<int3, 4> out;
        out[0] = int3(pos.x + 1, pos.y, 0);
        out[1] = int3(pos.x - 1, pos.y, 0);
        out[2] = int3(pos.x, pos.y + 1, 0);
        out[3] = int3(pos.x, pos.y - 1, 0);
        return out;
    };

    for (int y = 0; y < m_config.height; ++y) {
        for (int x = 0; x < m_config.width; ++x) {
            int index = y * m_config.width + x;
            if (!m_grid[index] || visited[index])
                continue;

            vector<int3> component = bfs_claim_component(m_config.width, m_config.height,
                                                         int3(x, y, 0), neighbours, passable);
            if (component.size() < 10) {
                for (const auto &pos : component) {
                    m_grid[pos.y * m_config.width + pos.x] = false;
                }
            }
            for (const auto &pos : component) {
                int indexC      = pos.y * m_config.width + pos.x;
                visited[indexC] = true;
            }
        }
    }
}

int CellularAutomata::countAliveNeighbors(int x, int y) const {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0)
                continue;

            int nx = x + i;
            int ny = y + j;

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
    m_grid.swap(m_tempGrid);
}
