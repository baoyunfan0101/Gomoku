# Gomoku (Five-in-a-row)
<div align="right">
	[<a href="README.md">中文</a> | English(Current)</a>]
</div>

## File Description
WindowsProject/AI-arith.cpp // core algorithm source file  
WindowsProject/AI-arith.h   // core algorithm header file  
WindowsProject/main.cpp     // UI interface and main function source file  
five-in-a-row.exe           // executable file: Gomoku mini-game

## Runtime Environment
Windows Desktop Application  
C++

## Project Description
The goal is to implement a human-vs-computer Gomoku game that satisfies the following conditions:  
- Human plays black pieces, computer plays white pieces;  
- Left-click to place a piece, right-click to undo a move.  

The method for solving this type of game problem is adversarial search. By designing an evaluation function, based on the minimax algorithm and α-β pruning, the human-computer game can be achieved.  

Starting from an empty board, after each move, the current board and related information are recorded as a state. Each state is treated as a node in a search tree, used to determine the next move and to support undo operations.  

## Core Algorithm
### On Adversarial Search
In artificial intelligence, an **Agent** is something that can perceive the environment and act within it. It perceives through sensors and influences the environment through actuators.  

**Game Theory** in mathematics is a branch of economics. When a problem involves multiple Agents, and each Agent is significantly affected by others, whether cooperative or competitive, the problem can be viewed as a **Game**. In AI, a game usually refers to a deterministic, turn-based, two-player, zero-sum game with complete information. Gomoku is a perfect example: if one side wins, the other side must lose.  

When solving such problems, we must consider not only our own actions but also the opponent’s actions. This approach is known as **Adversarial Search**.  

### Minimax Algorithm
The **minimax algorithm** is a classic adversarial search algorithm. With two Agents (human and AI), its process can be explained as follows:  

Suppose the current board state is known and it is the computer’s turn. For a two-ply search (AI move and opponent response), the process is:  
- Consider all possible moves the computer can make according to Gomoku rules;  
- For each candidate move, consider the opponent’s possible responses. The computer assumes its opponent is “rational,” always choosing the move that benefits itself the most.  

By this process, a minimax search tree can be constructed (the black portion in the figure below).  

![image](https://github.com/baoyunfan0101/five-in-a-row/blob/main/static/figure.png)

To evaluate each board configuration, define an evaluation function h(x). The higher the value of h(x), the better the position for the computer; lower values indicate disadvantage. The side trying to maximize h(x) is called MAX, and the other side is MIN. The definition and optimization of h(x) will be further analyzed in the “Evaluation Function Design” section.  

With h(x), the algorithm proceeds as follows: for each AI move, simulate the opponent’s best response (minimizing h(x)). For each AI move, take the minimum resulting h(x) (the MIN value). Then choose the AI move with the maximum of these MIN values. This process explains the name minimax.  

The above is only a two-ply search example. In practice, multi-ply searches are used. However, since each additional ply doubles the computation, the complexity grows exponentially.  

### α-β Pruning
**α-β pruning** is a technique used in minimax search trees to reduce computation cost.  

In minimax trees, MAX and MIN layers alternate. During search, each node maintains a pair (α, β), representing the minimum and maximum values achievable at that node. For a MAX node: if a child node produces a value smaller than α, then because the child belongs to a MIN layer, the value for this branch will definitely be smaller than α, and this branch can be pruned. Similarly, for MIN nodes, β-pruning applies.  

## Design
### Representation of State
As noted above, each board configuration is recorded as a `state`. The class `state` is defined where an object represents one game state. The 2D array `CB[][]` (ChessBoard) records the board: human (black, MIN) is -1, computer (white, MAX) is 1, and empty squares are 0.  

Other information and state-related functions are also encapsulated within the `state` class.  

The definition of `state` is as follows:  

```cpp
typedef class state {
public:
/* Basic information of the state */
    int CB[15][15] = { 0 }; // ChessBoard: AI (MAX)=1, opponent (MIN)=-1, empty=0
    int Last_i = -1;        // Last move row index
    int Last_j = -1;        // Last move column index
    int eva = INT_MIN;      // Evaluation function value

/* For building the search tree */
    state* father;          // Parent node
    vector<state*> child;   // Child nodes (stored in a container)

/* For alpha-beta pruning */
    int alpha = INT_MIN;
    int beta = INT_MAX;

/* Member functions of state */
    int F();                // Evaluation function
    int GoalTest();         // Goal test: returns 1 if AI (MAX) wins, -1 if opponent (MIN) wins, 0 otherwise
    state* minimax(int depth);  // Minimax search: returns the state after the next move
    void clear();           // Re-initialize temporary search data
}state;
```

### Building the Search Tree
The state class contains a pointer to its parent (`father`) and a container of children (`child`) to represent the search tree. This structure is used in the minimax and undo operations.  

### Evaluation Function Design
The adversarial search and α-β pruning process itself is well established. The key lies in designing the evaluation function for the Gomoku problem.  

Board patterns are analyzed and scored to ensure reasonable evaluation. The most common patterns are: Five-in-a-row, Open Four, Closed Four, Open Three, Closed Three, Open Two, Closed Two.  

Examples (X = black, O = white, + = empty spot):  

1. **Five-in-a-row**: winning condition. Score: 10000.  
   - XXXXX
2. **Open Four**: deadly threat. Score: 1000.  
   - +XXXX+
3. **Closed Four**: smaller threat. Score: 100.  
   - +XXXXO
   - X+XXX
   - XX+XX
4. **Open Three**: easily leads to Open Four. Score: 100.  
   - +XXX+
   - X+XX
5. **Closed Three**: smaller threat. Score: 10.  
   - ++XXXO
   - +X+XXO
   - +XX+XO
   - X++XX
   - X+X+X
   - O+XXX+O
6. **Open Two**: potential to form Open Three. Score: 10.  
   - ++XX++
   - +X+X+
   - X++X
7. **Closed Two**: very weak. Score: 1.  
   - +++XXO
   - ++X+XO
   - +X++XO
   - X+++X

| Pattern       | Score  |
|:-------------:|:------:|
| Five-in-a-row | 10000  |
| Open Four     | 1000   |
| Closed Four   | 100    |
| Open Three    | 100    |
| Closed Three  | 10     |
| Open Two      | 10     |
| Closed Two    | 1      |

For each state, break down the board into horizontal, vertical, and diagonal directions (“−”, “|”, “/”, “\”). Count occurrences of each pattern for both black and white. The evaluation is the black score minus the white score.  

### Analysis of Search Depth
Search depth strongly impacts optimality and efficiency.  

At minimum, two-ply searches are necessary: AI simulates its move and the opponent’s best response. With deeper search, AI can “look ahead” more moves and choose stronger strategies.  

However, blindly increasing depth is not always beneficial:  
- Humans may not always play optimally, so deeper AI search may overestimate the opponent.  
- Deeper searches cause exponential growth in computation, leading to significant lag in gameplay.  

## References
[1] Stuart J. Russell, Peter Norvig. *Artificial Intelligence: A Modern Approach (3rd ed.)*. Beijing: Tsinghua University Press, 2013: 64–95.  
[2] marble_xu. *Python Gomoku AI Implementation (2): Evaluation Function* [OL]. https://blog.csdn.net/marble_xu, 2019-05-23.