# HOMM3 map generator & fairness evaluation — README-MID

# What was done and further plans
## Adding borders [65b2557d057d0b56fc5b2fc862f79b6841afa4bf]
We designate border fields and fields within a given radius, and then place an obstacle (Pine Tree) on each of them.

We are considering adding irregularities/unevenness on the zone edges themselves.

## Adding connections (roads) [0e41be7f4abf72126d20d434c59b609dbdf8a0d7]
Zones are connected by roads defined by paths from castle to castle or to the center of the zone. Connections take into account neutral obstacles and castles (they don't collide with them), and segments exiting castles start from the actual entrance/exit.

Further plan is to add support for portals/other types of connections. For now, only basic roads.

## Enabling placement of creatures and their evaluation [d097e8b8374c5040d80be57c187eb17293cf4b36]
We solved the API problem that caused some creatures not to appear. We added a function allowing to place enemies with given parameters in specified positions. Files `CRTRAITS.*` were also created with `fight_value` calculated based on the formula from VCMI taking into account power and other statistics.

Plans include sensible placement (at border crossings, near treasures and mines) and determining the difficulty level of the zone to select appropriately challenging opponents.

## Adding basic mines [In progress on basic_mines and basic-mines-tests]
We place basic mines (ore mine, sawmill) near castles. We create a triangle with a given perimeter and place buildings at its vertices. If the randomization fails the first time, we try again (maximum 100 times).

We plan to raise the stage of placing mines in the generation order. Instead of rejecting some unsuccessful randomizations, we may slightly modify the map to enable building placement without disturbing the balance (e.g., when the mine is close to the edge, we can slightly shift the border, which additionally introduces variety).

# More distant, further plans
1) Improving town placement/determining zone centers (probably using something similar to [this](https://github.com/Sko0owi/HOMM3-mapgen/tree/main/generator)).
2) Preparing architecture for mutation.
3) Adding several random treasures on the map.

# What changed from the initial plan
We are planing to use two different templates (layout, blueprint), but for the sake of this project we decided to focus more on generating simple map rather than deciding in which template specific parameter will be. Therefore we won't be utilizing blueprint.

# Difficulties
Probably nothing unusual, sometimes problems with deciding what should be in the template and what not, etc. :D

# What are the chances it will succeed
It's not that it will succeed or it won't... Above all, the adventure and the path we have traveled together counts!

But with the power of friendship, we should realistically generate a simple map: with basic mines (in zones with player castles), randomly placed treasures in the zone (chests/resources) and guards between zones. All required zones should of course be connected.
