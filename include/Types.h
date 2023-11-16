#ifndef PETRINETS_TYPES_H
#define PETRINETS_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>

struct Node
{
    double x;
    double y;

    Node()
    {
        x = 0;
        y = 0;
    }

    Node(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
};

/// @brief Petri net place.
struct Place
{
    std::string name;
    int tokens;
    Place()
    {
        tokens = 0;
    }
    Place(std::string name, int tokens)
    {
        this->name = std::move(name);
        this->tokens = tokens;
    }
    bool operator==(const Place &other) const
    {
        return name == other.name;
    }
};

/// @brief Petri net transition.
struct Transition
{
    std::string name;
    Transition() = default;
    explicit Transition(std::string name)
    {
        this->name = std::move(name);
    }
    bool operator==(const Transition &other) const
    {
        return name == other.name;
    }
};

/// @brief Hash function for Petri net transitions.
struct TransitionHash
{
    size_t operator()(const Transition &transition) const
    {
        return std::hash<std::string>()(transition.name);
    }
};

/// @brief Hash function for Petri net places.
struct PlaceHash
{
    size_t operator()(const Place &place) const
    {
        return std::hash<std::string>()(place.name);
    }
};
/// @brief Hash function for a vector of places
struct PlacesHash
{
    size_t operator()(const std::vector<Place> &places) const
    {
        std::size_t seed = 0;
        for (const auto &place : places)
        {
            seed ^= place.tokens *PlaceHash{}(place) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
/// @brief Petri net arc direction custom data type.
enum class ArcDirection
{
    PlaceToTransition,
    TransitionToPlace
};
/// @brief Petri net arc.
struct Edge
{
    int from;
    int to;
    ArcDirection direction;
};

typedef std::vector<Place> Places;
typedef std::vector<Transition> Transitions;
typedef std::map<std::string, int> PlaceMap;
typedef std::map<std::string, int> TransitionMap;
typedef std::vector<Edge> Arcs;
typedef std::map<std::string, Transition> ReverseTransitionMap;
typedef std::unordered_set<Transition, TransitionHash> TransitionSet;
typedef std::unordered_set<Place, PlaceHash> PlaceSet;

#endif //PETRINETS_TYPES_H
