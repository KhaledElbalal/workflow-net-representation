#include "../include/Utility.h"
#include "../include/Constants.h"

#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <queue>
#include <set>
#include <cmath>
#include <iostream>


Transitions latestFiredTransitions;


/// @brief Converts a positive integer to its ordinal form.
/// @param num (Int) The number to convert.
/// @return (String) The ordinal form of the number.
std::string convertToOrdinal(int num)
{
    if (num < 1)
    {
        throw std::invalid_argument("Input must be a positive integer.");
    }
    std::string suffix;
    if (num % 10 == 1 && num % 100 != 11)
    {
        suffix = "st";
    }
    else if (num % 10 == 2 && num % 100 != 12)
    {
        suffix = "nd";
    }
    else if (num % 10 == 3 && num % 100 != 13)
    {
        suffix = "rd";
    }
    else
    {
        suffix = "th";
    }
    return std::to_string(num) + suffix;
}

/// @brief Gets the places that are enabled (Have a token) in the given Petri net.
/// @param net Petri net state.
/// @return (std::vector<Place>) A vector containing the places that are enabled.
Places getEnabledPlaces(const PetriNet &net)
{
    Places enabledPlaces;
    for (const auto & place : net.getPlaces())
    {
        if (place.tokens > 0)
        {
            enabledPlaces.push_back(place);
        }
    }
    return enabledPlaces;
}

/// @brief Gets a vector of transitions that can be fired in the given Petri net.
/// @param net Petri net state.
/// @return (std::vector<Transition>) A vector containing the transitions that can be fired/are enabled.
Transitions getEnabledTransitions(const PetriNet &net)
{
    Transitions enabledTransitions;
    for (auto &transition : net.getTransitions())
    {
        bool enabled = true;
        for (const auto &arc : net.getArcs())
        {
            if (arc.direction == ArcDirection::PlaceToTransition)
            {
                if (arc.to == net.getTransitionMap().at(transition.name))
                {
                    if (net.getPlaces().at(arc.from).tokens < 1)
                    {
                        enabled = false;
                        break;
                    }
                }
            }
        }
        if (enabled)
        {
            enabledTransitions.push_back(transition);
        }
    }
    return enabledTransitions;
}

/// @brief Fires a transition in the given Petri net.
/// @param net Petri net state.
/// @param transition Transition to be fired, must be enabled.
void fireTransition(PetriNet &net, const Transition &transition)
{
    for (auto &arc : net.getArcs())
    {
        if (arc.direction == ArcDirection::PlaceToTransition)
        {
            if (arc.to == net.getTransitionMap().at(transition.name))
            {
                net.consumeToken(arc.from);
            }
        }
        else
        {
            if (arc.from == net.getTransitionMap().at(transition.name))
            {
                net.addToken(arc.to);
            }
        }
    }
}

/// @brief Reverses a transition firing in the given Petri net.
/// @param net Petri net state.
/// @param transition Transition to be unfired, must be valid to unfire.
void unFireTransition(PetriNet &net, const Transition &transition)
{
    for (auto &arc : net.getArcs())
    {
        if (arc.direction == ArcDirection::PlaceToTransition)
        {
            if (arc.to == net.getTransitionMap().at(transition.name))
            {
                net.addToken(arc.from);
            }
        }
        else
        {
            if (arc.from == net.getTransitionMap().at(transition.name))
            {
                net.consumeToken(arc.to);
            }
        }
    }
}

/// @brief Checks if the given Petri net is in a final state.
/// @param net Petri net state.
/// @return (Bool) True if the Petri net is in a final state, false otherwise.
bool isFinalState(const PetriNet &net, int target)
{
    for (const auto &place : net.getPlaces())
    {
        if (place.tokens > 0 && place.name != "o")
        {
            return false;
        }
        if ((place.tokens == 0 || place.tokens != target) && place.name == "o")
        {
            return false;
        }
    }
    return true;
}

/// @brief Converts a Petri net to an SVG file.
/// @param net Petri net to convert.
/// @param filename Filename to save the SVG file to. (Must include .svg in the end)
void writePetriNetToSVG(const PetriNet &net, const std::string &filename)
{
    // Constants
    const int width = SPACE_BETWEEN_NODES * std::max((int)net.getPlaces().size(), (int)net.getTransitions().size()) + 100;
    const int height = SPACE_BETWEEN_NODES * 2 + 100;

    std::ofstream file(filename);
    // Define SVG header
    file << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width=")" << width << "\" height=\"" << height
         << "\">" << std::endl;
    file << "<defs>" << std::endl;
    // Define arrowheads using Xiketic and Blue Jeans colors
    file << R"(<marker
      id="arrowheadBlue"
      viewBox="0 0 10 10"
      refX="1"
      refY="5"
      markerUnits="strokeWidth"
      markerWidth="5"
      markerHeight="5"
      orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#38B6FF" />)"
         << std::endl;
    file << "</marker>" << std::endl;
    file << R"(<marker
      id="arrowhead"
      viewBox="0 0 10 10"
      refX="1"
      refY="5"
      markerUnits="strokeWidth"
      markerWidth="5"
      markerHeight="5"
      orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#121420" />)"
         << std::endl;
    file << "</marker>" << std::endl;
    file << "</defs>" << std::endl;
    // Write places
    for (int i = 0; i < net.getPlaces().size(); i++)
    {
        const int x = SPACE_BETWEEN_NODES + i * SPACE_BETWEEN_NODES;
        const int y = SPACE_BETWEEN_NODES;
        file << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << NODE_RADIUS
             << R"(" stroke="#121420" stroke-width="2" fill="white" />)" << std::endl;
        int offset = -5;
        if (net.getPlaces()[i].tokens)
        {
            file << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"" << NODE_RADIUS / 2
                 << R"(" stroke="none" fill="#121420" />)" << std::endl;
            offset = NODE_RADIUS + 20;
        }

        file << "<text x=\"" << x << "\" y=\"" << y - offset << R"(" font-size="20" text-anchor="middle">)"
             << net.getPlaces()[i].name << "</text>" << std::endl;
    }

    // Write transitions
    for (int i = 0; i < net.getTransitions().size(); i++)
    {
        const int x = SPACE_BETWEEN_NODES + i * SPACE_BETWEEN_NODES;
        const int y = SPACE_BETWEEN_NODES * 2;
        file << "<rect x=\"" << x - NODE_RADIUS << "\" y=\"" << y - NODE_RADIUS << "\" width=\"" << 2 * NODE_RADIUS << "\" height=\""
             << 2 * NODE_RADIUS << R"(" stroke="#121420" stroke-width="2" fill="white" />)" << std::endl;
        file << "<text x=\"" << x << "\" y=\"" << y + 5 << R"(" font-size="20" text-anchor="middle">)"
             << "t" + std::to_string(i + 1) << "</text>" << std::endl;
        file << "<text x=\"" << x << "\" y=\"" << y + NODE_RADIUS + 20 << R"(" font-size="20" text-anchor="middle">)"
             << net.getTransitions()[i].name << "</text>" << std::endl;
    }

    // Write arcs
    for (auto &entry : net.getArcs())
    {
        // Constants, calculate start and end points coordinates
        const int from = entry.from;
        const int to = entry.to;
        const int x1 = SPACE_BETWEEN_NODES + from * SPACE_BETWEEN_NODES;
        const int y1 = SPACE_BETWEEN_NODES;
        const int x2 = SPACE_BETWEEN_NODES + to * SPACE_BETWEEN_NODES;
        const int y2 = SPACE_BETWEEN_NODES * 2;

        // Calculate normalized direction vector
        const int dx = x2 - x1;
        const int dy = y2 - y1;
        const double length = std::sqrt(dx * dx + dy * dy) - 20;
        const double nx = dx / length;
        const double ny = dy / length;
        // Decide direction based on ArcDirection
        if (entry.direction == ArcDirection::PlaceToTransition)
            file << "<line x1=\"" << x1 + nx * NODE_RADIUS << "\" y1=\"" << y1 + ny * NODE_RADIUS << "\" x2=\"" << x2 - nx * NODE_RADIUS << "\" y2=\"" << y2 - ny * NODE_RADIUS << "\" stroke=\"#121420\" stroke-width=\"2\" marker-end=\"url(#arrowhead)\"/>" << std::endl;
        else
            file << "<line x1=\"" << x1 + nx * NODE_RADIUS << "\" y2=\"" << y1 + ny * NODE_RADIUS << "\" x2=\"" << x2 - nx * NODE_RADIUS << "\" y1=\"" << y2 - ny * NODE_RADIUS << "\" stroke=\"#38B6FF\" stroke-width=\"2\" marker-end=\"url(#arrowheadBlue)\"/>" << std::endl;
    }
    // End SVG file
    file << "</svg>" << std::endl;
    file.close();
}

/// @brief Determines if a Petri net is a workflow net
/// @param net Petri net to check
/// @return (bool) true if the net is a workflow net, false otherwise
bool isWorkflowNet(const PetriNet &net)
{
    int iCount = 0, oCount = 0;
    int iIndex = -1, oIndex = -1;
    // Check if there is a specific input place i and output place o
    for (int i = 0; i < net.getPlaces().size(); i++)
    {
        const Place &place = net.getPlaces()[i];
        if (place.name == "i")
        {
            if (place.tokens < 1)
            {
                return false;
            }
            iCount++;
            iIndex = i;
        }
        else if (place.tokens > 0)
        {
            // If there is a non-input place with tokens, return false
            return false;
        }
        else if (place.name == "o")
        {
            oCount++;
            oIndex = i;
        }
    }
    if (iCount != 1 || oCount != 1)
    {
        // If there is not exactly one input place i and one output place o, return false
        return false;
    }
    // Check that i has no incoming edges and o has no outgoing edges
    return std::all_of(net.getArcs().begin(), net.getArcs().end(), [&](const Edge &arc)
    {
        if (arc.to == iIndex && arc.direction == ArcDirection::TransitionToPlace) {
            return false;
        }
        if (arc.from == oIndex && arc.direction == ArcDirection::PlaceToTransition) {
            return false;
        }

        return true; });
}


/// @brief Performs a depth-first search on a Petri net to find a valid path
/// @param states states found
/// @param statesSet set to keep track of duplicate states
/// @param initialTokens Initial tokens in the input place
/// @param net  Petri net to search
/// @param firedTransitions Transitions that have already been fired
/// @param depth Current depth of the search
/// @return (bool) true if a valid path was found, false otherwise
bool getFinalSpace(PetriNet net, std::vector<std::pair<Places, Transitions>> &states, std::unordered_set<Places, PlacesHash> &statesSet, int initialTokens = 1, Transitions firedTransitions = {}, int depth = 0) // NOLINT(misc-no-recursion)
{
    // Instant termination conditions
    bool flag = false;
    if (depth > 100)
    {
        return false;
    }
    if (depth == 0 && !isWorkflowNet(net))
    {
        return false;
    }
    // Get all enabled transitions
    Transitions enabledTransitions = getEnabledTransitions(net);
    if (!statesSet.count(net.getPlaces()))
    {
        statesSet.insert(net.getPlaces());
        states.emplace_back(net.getPlaces(), firedTransitions);
    }
    else
    {
        return false;
    }
    // If there are no enabled transitions, return false
    if (enabledTransitions.empty())
    {
        return false;
    }
    // Enable each transition and check if the net is in a final state
    for (auto &transition : enabledTransitions)
    {
        fireTransition(net, transition);
        firedTransitions.push_back(transition);
        if (isFinalState(net, initialTokens))
        {
            // If the net is in a final state, print all states leading to it, then return true
            latestFiredTransitions = firedTransitions;
            writePetriNetToSVG(net, convertToOrdinal((int)firedTransitions.size() + 1) + ".svg");
            for (int i = (int)firedTransitions.size() - 1; i >= 0; i--)
            {
                Places enabledPlaces = getEnabledPlaces(net);
                unFireTransition(net, firedTransitions[i]);
                writePetriNetToSVG(net, convertToOrdinal(i + 1) + ".svg");
            }
            Places enabledPlaces = getEnabledPlaces(net);
            return true;
        }
        if (getFinalSpace(net, states, statesSet, initialTokens, firedTransitions, depth + 1))
        {
            flag = true;
        }
        firedTransitions.pop_back();
        // Undo the transition
        unFireTransition(net, transition);
    }
    return flag;
}

/// @brief Determines if all transitions in a PetriNet could be enabled
/// @param net Petri net to check
/// @param possibleTransitions Set of transitions that could be enabled
/// @param initialTokens Number of tokens initially in the input place
/// @param depth Maximum depth of the search
/// @return (bool) True if all transitions could be enabled, false otherwise
bool allTransitionsEnabled(PetriNet net, TransitionSet &possibleTransitions, int initialTokens = 1, int depth = 0) // NOLINT(misc-no-recursion)
{
    // Instant termination conditions
    if (depth > 100)
    {
        return false;
    }
    if (depth == 0 && !isWorkflowNet(net))
    {
        return false;
    }
    // Get all enabled transitions
    Transitions enabledTransitions = getEnabledTransitions(net);
    // If there are no enabled transitions, return false
    if (enabledTransitions.empty())
    {
        return false;
    }
    else if (possibleTransitions.size() == net.getTransitions().size())
    {
        return true;
    }
    // Enable each transition and check if the net is in a final state
    for (auto &transition : enabledTransitions)
    {
        possibleTransitions.insert(transition);
        fireTransition(net, transition);
        if (isFinalState(net, initialTokens))
        {
            return enabledTransitions.size() == net.getTransitions().size();
        }
        if (allTransitionsEnabled(net, possibleTransitions, initialTokens, depth + 1))
        {
            return true;
        }
        // Undo the transition
        unFireTransition(net, transition);
    }
    return false;
}

/// @brief Determines if a Petri net is sound
/// @param net Petri net to check
/// @return (bool) true if the net is sound, false otherwise
bool isSound(const PetriNet &net)
{
    if (!isWorkflowNet(net))
    {
        std::cout << "Not a workflow net" << std::endl;
        return false;
    }
    bool flag = true;
    TransitionSet possibleTransitions;
    if (!allTransitionsEnabled(net, possibleTransitions))
    {
        for (auto &transition : net.getTransitions())
        {
            if (possibleTransitions.find(transition) == possibleTransitions.end())
            {
                std::cout << "Transition " << transition.name << " couldn't be enabled" << std::endl;
            }
        }
        flag = false;
    }
    std::vector<std::pair<Places, Transitions>> places;
    std::unordered_set<Places, PlacesHash> statesSet;
    std::vector<std::pair<Places, Transitions>> states;
    int initialTokens = net.getPlaces()[net.getPlaceMap().at("i")].tokens;
    if (!getFinalSpace(net, states, statesSet, initialTokens, {}, 0))
    {
        std::cout << "Can't reach final state" << std::endl;
        flag = false;
    }
    PetriNet net2 = net;
    PlaceSet deadlocks;
    for (int i = 0; i < states.size(); i++)
    {
        net2.setPlaces(states[i].first) ;
        if (!getFinalSpace(net2, states, statesSet, initialTokens, states[i].second, 10))
        {
            if (getEnabledTransitions(net2).empty())
            {
                for (auto &place : net2.getPlaces())
                {
                    if (place.tokens > 0)
                    {
                        deadlocks.insert(place);
                    }
                }
            }
        }
    }
    if (!deadlocks.empty())
    {
        std::cout << "Deadlocks found: ";
        for (auto &place : deadlocks)
        {
            std::cout << place.name << " ";
        }
        std::cout << std::endl;
        flag = false;
    }
    // Soundness Check is Complete
    return flag;
}

//

/// @brief Helper function to generate a unique string identifier for a marking
/// @param places Vector of places to generate a marking for
/// @return (string) String representation of the marking
std::string markingToString(const Places &places)
{
    std::stringstream ss;
    for (const auto &place : places)
    {
        if (place.tokens > 0)
            ss << place.name << ",";
    }
    std::string s = ss.str();
    if (!s.empty())
    {
        s.pop_back();
    }
    return s;
}

/// @brief Function to save a reachability graph of a petri net to an SVG file
/// @param net Petri net to generate the reachability graph for
/// @param filename Name of the file to save the SVG to
void drawReachabilityGraphToSVG(const PetriNet &net, const std::string &filename)
{
    std::queue<std::pair<Places, Transitions>> statesQueue;
    std::set<std::string> visitedStates;
    std::vector<std::tuple<std::string, std::string, std::string>> edges;

    statesQueue.push({net.getPlaces(), {}});

    while (!statesQueue.empty())
    {
        auto current = statesQueue.front();
        statesQueue.pop();

        auto currentState = current.first;
        auto currentTransitions = current.second;

        std::string currentStateStr = markingToString(currentState);
        if (visitedStates.find(currentStateStr) != visitedStates.end())
        {
            continue;
        }
        visitedStates.insert(currentStateStr);

        PetriNet currentStateNet = net;
        currentStateNet.setPlaces(currentState);

        Transitions enabledTransitions = getEnabledTransitions(currentStateNet);
        for (const auto &transition : enabledTransitions)
        {
            PetriNet nextStateNet = currentStateNet;
            fireTransition(nextStateNet, transition);
            std::string nextStateStr = markingToString(nextStateNet.getPlaces());
            edges.emplace_back(currentStateStr, nextStateStr, transition.name);

            statesQueue.emplace(nextStateNet.getPlaces(), currentTransitions);
        }
    }
    std::map<std::string, Node> positions;

    int start = 450;
    int incr = 100 / (int)visitedStates.size();

    std::set<std::string> visitedStates2 = visitedStates;

    std::map<std::string, std::string> isConnected;
    for (auto x : edges)
    {
        isConnected[std::get<0>(x)] = std::get<1>(x);
        isConnected[std::get<1>(x)] = std::get<0>(x);
    }

    for (const auto &x : visitedStates)
    {
        double randomX = start + incr;
        start += incr;
        double randomY = start + incr;
        std::vector<std::string> connectedStates;
        int offset = 50;
        for (const auto &y : visitedStates2)
        {
            if (isConnected[x] == y)
            {
                positions[y] = Node(randomX, randomY - offset);
                offset += 50;
                connectedStates.push_back(y);
            }
        }
        for (const auto &y : connectedStates)
        {
            visitedStates2.erase(y);
        }
        if (!positions.count(x))
            positions[x] = Node(randomX, randomY);
    }

    double K = 500;
    for (int i = 0; i < ITERATIONS; i++)
    {
        for (auto &x : positions)
        {
            for (auto &y : positions)
            {
                if (x.first != y.first)
                {
                    double force;
                    double x_dist = x.second.x - y.second.x;
                    double y_dist = x.second.y - y.second.y;
                    double dist = sqrt(x_dist * x_dist + y_dist * y_dist);
                    if (!isConnected.count(x.first) || isConnected[x.first] != y.first)
                    {
                        force = std::min(std::max(K * K / dist, MIN_FORCE), MAX_FORCE);
                    }
                    else
                    {
                        force = -1 * std::min(std::max(dist * dist / K, MIN_FORCE), MAX_FORCE);
                    }
                    x.second.x += x_dist / dist * force;
                    x.second.y += y_dist / dist * force;
                    y.second.x -= x_dist / dist * force;
                    y.second.y -= y_dist / dist * force;
                }
            }
        }
        K *= ATTRACTION_CONSTANT;
    }

    // Scale the positions to keep the same relative position on the canvas as the original graph
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::min();
    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::min();
    for (auto &x : positions)
    {
        min_y = std::min(min_y, x.second.y);
        max_y = std::max(max_y, x.second.y);
        min_x = std::min(min_x, x.second.x);
        max_x = std::max(max_x, x.second.x);
    }

    double scale = std::min((REACHABILITY_GRAPH_WIDTH - 2 * NODE_RADIUS) / (max_x - min_x),
                            (REACHABILITY_GRAPH_WIDTH - 2 * NODE_RADIUS) / (max_y - min_y));
    for (auto &x : positions)
    {
        x.second.x = (x.second.x - min_x) * scale + NODE_RADIUS;
        x.second.y = (x.second.y - min_y) * scale + NODE_RADIUS;
    }
    std::ofstream file(filename);
    // Define SVG header
    file
            << R"(<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" fill="white" version="1.1" width=")"
            << REACHABILITY_GRAPH_WIDTH << "\" height=\"" << REACHABILITY_GRAPH_WIDTH
            << "\">" << std::endl;
    file << "<defs>" << std::endl;
    // Define arrowheads using Xiketic and Blue Jeans colors
    file << R"(<marker
      id="arrowheadBlue"
      viewBox="0 0 10 10"
      refX="1"
      refY="5"
      markerUnits="strokeWidth"
      markerWidth="5"
      markerHeight="5"
      orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" fill="#38B6FF" />)"
         << std::endl;
    file << "</marker>" << std::endl;
    file << R"(<marker
                  id="arrowhead"
                  viewBox="0 0 10 10"
                  refX="1"
                  refY="5"
                  markerUnits="strokeWidth"
                  markerWidth="5"
                  markerHeight="5"
                  orient="auto">
                  <path d="M 0 0 L 10 5 L 0 10 z" fill="#121420" />)"
         << std::endl;
    file << "</marker>" << std::endl;
    file << "</defs>" << std::endl;
    for (auto [u, v] : positions)
    {
        std::string text = "{" + u + "}";
        file << "<text x=\"" << v.x << "\" y=\"" << v.y
             << R"(" text-anchor="middle" fill="black" font-size="20px" dy=".3em" font-weight="bold"> )"
             << text << "</text>" << std::endl;
    }
    std::map<std::pair<std::string, std::string>, std::vector<std::string>> grouped_edges;
    for (auto x : edges)
    {
        std::string to = std::get<0>(x);
        std::string from = std::get<1>(x);
        std::string transition = std::get<2>(x);
        grouped_edges[{to, from}].push_back(transition);
    }
    for (auto const &x : grouped_edges)
    {
        double x1 = positions[x.first.first].x;
        double y1 = positions[x.first.first].y;
        double x2 = positions[x.first.second].x;
        double y2 = positions[x.first.second].y;
        double offsetX = 100;
        double offsetY = 100;
        if (x.first.first < x.first.second)
        {
            offsetX = -offsetX;
            offsetY = -offsetY;
        }
        double controlX = (x1 + x2) / 2 - offsetX;
        double controlY = (y1 + y2) / 2 - offsetY; // Adjust the control point's Y coordinate as desired

        double angle = std::atan2(y2 - y1, x2 - x1);
        if (x.first.first < x.first.second)
        {
            file << "<path d=\"M " << x1 << " " << y1 + 20 * sin(angle) << " Q " << controlX << " " << controlY
                 << " " << x2 << " " << y2 - 20 * sin(angle)
                 << R"lit(" fill="none" stroke="#38B6FF" stroke-width="2" marker-end="url(#arrowheadBlue)"/>)lit"
                 << std::endl;
        }
        else
        {

            file << "<path d=\"M " << x1 << " " << y1 + 20 * sin(angle) << " Q " << controlX << " " << controlY
                 << " " << x2 << " " << y2 - 20 * sin(angle)
                 << R"lit(" fill="none" stroke="#121420" stroke-width="2" marker-end="url(#arrowhead)"/>)lit"
                 << std::endl;
        }

        std::string transitions;
        for (auto &t : x.second)
        {
            transitions += t + ", ";
        }
        transitions.insert(transitions.begin(), '{');
        transitions[transitions.size() - 2] = '}';
        double x3 = (x1 + x2) / 2 - offsetX / 2;
        double y3 = (y1 + y2) / 2 - offsetY / 2;
        if (x.first.first < x.first.second)
        {
            file << "<text x=\"" << x3 << "\" y=\"" << y3
                 << R"(" text-anchor="middle" fill="#121420" font-size="20px" dy=".3em" font-weight="bold">)"
                 << transitions << "</text>" << std::endl;
        }
        else
        {
            file << "<text x=\"" << x3 << "\" y=\"" << y3
                 << R"(" text-anchor="middle" fill="#38B6FF" font-size="20px" dy=".3em" font-weight="bold">)"
                 << transitions << "</text>" << std::endl;
        }
    }
    file << "</svg>" << std::endl;
    file.close();
}