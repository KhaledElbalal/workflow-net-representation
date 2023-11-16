# Technical Report on Programmatically Representing, Verifying and Drawing Workflow Nets Using C++

**Done by Khaled Abdelreheam Said, 20217004.**

**Under the Supervision of Assoc. Prof. Iman Helal**

## About

### Introduction

Petri nets are a popular way to formally and abstractly define business processes. They are used as a foundation for creating process languages. When we say "formal," we mean that process models described using Petri nets have clear and unambiguous meanings. Petri nets are considered "abstract" because they focus solely on the functional and process perspectives of a business process and ignore all other aspects of its execution environment.

Petri nets is basically the mathematical representation of concurrent systems. Petri net is a directed graph consisting of places, transitions and directed edges between them, Petri nets are bipartite graphs, meaning edges between 2 Places or 2 Transitions aren’t allowed.

The goal behind this report is to represent a way to programmatically represent Petri Nets, Workflow nets, decide their validaty and soundness, as well as draw them graphically. C++ and SVG are used to achieve this goal. We first begin by formally defining Petri Nets & Workflow nets as well as the soundess criteria, we then go over a brief description of the program and finally talk about how to use the program.

### Formal Definition

Petri Net can be descriped as the tuple $(P,T,F)$ where:

- $P$ is a finite set of places
- $T$ is a finite set of transition where $T \cap P = \empty$
- $F$ is a flow relation where: $F \subseteq (P \times T) \cup (T \times P)$
- A place $p \in P$ is an input place of a transition $t \in T$, iff there is a directed arc from $p$ $$ to $t$ [$(p,t) \in F$. The set of input places for a transition $t$ is denoted by $\cdot t$
- A place $p \in P$ is an output place of a transition $t \in T$, iff there is a directed arc from $p$ $$ to $t$ [$(t,p) \in F$. The set of output places for a transition $t$ is denoted by $t \cdot$
- $\cdot p$ denote the set of transitions that share p as input places
- $p \cdot$ denote the set of transitions that share p as output places

However Petri Nets represent the static structure of the system, to represent the dynamic behavior we introduce markings, the marking of a Petri $(P,T,F)$  is defined by a function $M:P\rightarrow \N$  mapping the set of places onto the natural numbers, where $\N$ is the set of natural numbers including zero.

So A Petri Net System is a pair $(PN,M)$
With a Petri Net $PN = (P,T,F)$
Initial Marking $M$

### Workflow nets

We then get the idea to model business processes using Petri Nets, places represent states, transition represent activities, edges represent the control flow, tokens can carry structured values, and process instances’s behavior is represented by firing rules.

We then get the following set of rules to define a workflow net in addition to the Petri Net rules:

- There needs to be a specific input place $i$
- There needs to be a specific output place $o$
- $i$ must have no incoming edges, and $o$ must have no outgoing edges

### Deciding soundness

This assignment is about taking a Workflow net and programmatically deciding soundness and the validity of the workflow net. To do this we must first define the properties for soundness.

1. For each token on the initial place $i$, exactly one token appears on the final place.
2. If a token appears on the final place $o$, all other places are empty.
3. Each transition can be enabled.

---

## Programming Guidelines

- The implementation was developed using C++17 with standard libraries, making it cross-compatible across different OSes, architectures, and compilers.
- Testing was conducted on an ARM architecture running MacOS with the g++ compiler.
- Deoxygen comments were used to provide comprehensive documentation for each function, promoting seamless integration with an IDE.
- Namespaces weren’t utilized to adhere to best practices. And to ensure compliance with clang & vc++, `#include <bits/stdc++>` wasn’t used, which is also a best practice.
- The code underwent rigorous linting using Clang-Tidy, ensuring adherence to industry-standard coding conventions and highlighting potential errors or warnings.
- A Petri Net graphical representation uses SVG format.

---

## Program Details (Brief)

### The PetriNet Datastructure

Prerequiste to representing the Petrinet Data Structure, we need simple structures to represent the Places, Transitions & Arcs.

We then define a vector of each to represent the entire Petrinet, we use maps to enable O(1) translation between names and indices.

The Data Structure has functions to add a place, transition, arc, it is fairly limited.

### Some Helper functions

- `isFinalState`: Determines if the Petrinet only has tokens on the output and none anywhere else
- `FireTransition`: Fires a specific Transition (doesn’t check if it can be fired)
- `unFireTransition`: Undos the firing of a Transition
- `GetEnabledTransitions`: Returns a vector of all transitions that can be fired
- `GetEnabledPlaces`: Returns a vector of all places that have atleast one token
- `convertToOrdinal`: Converts a number to it’s ordinal form

### Representing the Petri Net graphically

We use an SVG canvas, we set the width of the file to the max of (number of transition or places) * space + 100 for some offset. We then draw the places next to each other as circles at the top half and at the bottom half we draw the transitions as rectangles. After that we draw the arrows, we determine the orientation based on the arc direction and calculate the direction vector. Based on the markings we draw the tokens.

### Verifiying it is a Workflow Net

- Iterate over the vector to get the count of input and output places, return false if they are invalid (not 1 & 1) and if there are tokens in non input places.
- Iterate over the arcs to verify no edges go into input and no edges go out of output.

### Verifying Soundess

- Given that it is a sound petri net, we use DFS to explore the possible states and search for one that matches the final conditions. We keep track of the fired transitions and the depth, if the depth exceeds 100 it terminates. If the final state is found we unfire the transitions to save each of the states as an SVG then return true.
- We keep track of all the states that we reached in a vector/set pair and verify that all of them reach the output, the program prints the deadlock states if there is any.

## Drawing the reachability graph

- We first get the states that we reached, and the transitions between them, assigning each an initial space in 2D Space (Trying to group connected nodes together)
- We then employ the Fruchterman-Reingold Algorithm. The Fruchterman-Reingold algorithm is a force-directed graph layout algorithm that aims to position the nodes of a graph in a way that minimizes edge crossings and evenly distributes the nodes. The algorithm begins by randomly assigning initial positions to the nodes. It then applies attractive forces between connected nodes, causing them to move closer to each other. Additionally, it applies repulsive forces between all nodes, causing them to repel each other. The algorithm iteratively adjusts the positions of the nodes based on these forces until the system reaches a state of equilibrium, where the total energy of the system is minimized. As a result, the nodes tend to settle into a visually pleasing layout with well-spaced and non-overlapping edges.
- After that we rescale the nodes to fit within the SVG space and draw them. We use bended arcs to define transitions and print the transition name in the middle.

---

## Program Usage

### Input

First the program will ask if we would like to insert a text file as input instead, use 0 if you would like to use a text file (`pn1.txt`, `pn2.txt`, `pn2-v1.txt`, `pn2-v2.txt`, `pn2-v3.txt`, `pn3.txt`, `pn4.txt`) are included which are the examples in the lecture. Use 1 if you would like to enter the elements dynamically.

Let’s define $x,y,z \geq0$ as the number of places, transitions and arcs respectively.

The first line of input contains the integer $x$, followed by $x$ integers representing the number of tokens at places $i,P_i,P_{i+1},...,P_{x-2},o$ (Note that a non-zero value or a non positive value on the initial place results in an invalid Workflow net)

The next line of input contains the integer $y$, followed by $y$ lines where each line $i$ is a string that represents the name of the transitions $t_i$.

The next line of input contains the integer $z$, followed by $2z$ lines containing $a_i,b_i$ where $a_i$ is the from attribute in the arc, $b_i$ is the to attribute, a$_i$ and $b_i$ could be a transition or a place, but they can’t both be of the same type.

A runtime error will be raised if one of the values is invalid.

### Output

List of Places

List of Transitions and labels

List of

- Workflow Net:
    - True, if it is a valid workflow net
    - False, otherwise
- Soundness:
    - True, if it is sound
        - If it is sound, then a valid path will be printed and all SVG files in order will be saved (1st.svg, 2nd.svg, etc.)
    - False, otherwise
        - Only a 1st.svg file will be saved to present the initial state of the Petri Net
        - Deadlock states and unfireable transitions will also be printed
- Then the program generates a reachability graph saved reachability_graph.svg)

---