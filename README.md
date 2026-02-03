# hex-grid-shortest-path
This project requires the implementation of different functions, centered around the calculation of the shortest path on a hex-grid map with variable costs, focused on time and space efficiency.

---

## PROJECT SPECIFICATIONS
Implementing the following functions, all applying to a rectangular map divided into hexagonal tiles.
- init(y, x): initialize the map, receives a parameter y for number of columns and a number x for number of rows comprising the map. If a map already exists it overrides it.
- change_cost(y, x, v, r): given a center point of y,x coordinates, changes the cost of all hexagons within a radius of r according to a formula using v, between -10 and 10, and proportional to the hexagon's distance from the center. The cost of every out-bound air route also changes. Costs are limited to [0,100].
- toggle_air_route(y1, x1, y2, x2): enables an air route from an hexagon to another, with a maximum of 5 outbound routes for each hexagon. 
- travel_cost(y1, x1, y2, x2): calculates the cost of the shortest path, this takes into consideration the presence of possible air routes.

---

## LIMITATIONS
The program should run on terminal (only I/O, no GUI). Given a randomly generated input of tens of thousands of commands, the program should be able to execute the commands and return correct values, while still terminating before 10.0s and using less than 25MB to be evaluated with maximum grade. The project had to be completed by a set deadline.

---

## OBSERVATIONS
- The hexagonal grid is often used in mapping, because it allows to move diagonally in addition to horizontally/vertically. One possibility to deal with this type of grid is using cubic coordinates, but by using offset instead the position of the hexagon within the rectangular map (or matrix) can be identified more easily. With this approach, the calculation of the neighboring hexagons depends on the starting one's position, in particular whether it's placed on a even or odd row.
- Upon further inspection the air routes' costs are always equal to the one of the starting hexagon, thus there was no need to store their costs separately.

---

## DECISIONAL CHOICES
Because of the limit on overall complexity I traded cleanliness and elegance for better and faster performance: instead of using a struct (matrix of hexagons was my first idea), which would require frequent access to non contiguous parts of the memory, I decided to keep all my data in arrays. I also opted for Dial's algorithm, which optimizes Dijkstra's algorithm, since there was a limited range of edge weights.
The program had to run and execute multiple commands, therefore I had to keep track of the number of iteration/round: to avoid cleaning the data structures each time, which would require extra time consumption, I decided instead to keep everything and associate each value with a flag signaling its validity.
Pointers to the neighboring hexagons, even if the access to these is required often, are not stored because of the space limitation. Instead, their coordinates are calculated each time because such computation's time complexity is low when storing a small supporting array based on the hexagon's type.

RESULT AND FURTHER CONSIDERATIONS:
The completed project is able to satisfy all the requirements, in particular it surpassed every given test case and managed to pass the final test with around 9s of execution time and 16MB used.
The program's performance in terms of time could have been further improved by using a cache table, since it is specified that some areas are more often visited than others, but because I knew that Dial's algorithm was efficient enough I decided not to use up more space. It is still a possible tradeoff considering the remaining extra space.
