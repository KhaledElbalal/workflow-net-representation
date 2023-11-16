#include "../include/Petrinet.h"

PetriNet::PetriNet() = default;

/// @brief Adds a place to the Petri net.
/// @param place Place to add.
/// @return (Bool) True if the place was added, false if the place already exists.
bool PetriNet::addPlace(Place &place) {
    if (placeMap.find(place.name) != placeMap.end()) {
        return false;
    }
    places.push_back(place);
    placeMap[place.name] = static_cast<int>(places.size()) - 1;
    return true;
}

/// @brief Adds a transition to the Petri net.
/// @param transition Transition to add.
/// @return (Bool) True if the transition was added, false if the transition already exists.
bool PetriNet::addTransition(const Transition &transition) {
    if (transitionMap.find(transition.name) != transitionMap.end()) {
        return false;
    }
    transitions.push_back(transition);
    transitionMap[transition.name] = static_cast<int>(transitions.size()) - 1;
    reverseTransitionMap["t" + std::to_string(static_cast<int>(transitions.size()))] = transition;
    return true;
}

/// @brief Adds an arc to the Petri net.
/// @param place Place that the arc originates from.
/// @param transition Transition that the arc goes to.
/// @return (Bool) True if the arc was added, false if the arc already exists.
bool PetriNet::addArc(const Place &place, const Transition &transition) {
    int placeIndex = placeMap[place.name];
    int transitionIndex = transitionMap[transition.name];
    Edge edge = {placeIndex, transitionIndex, ArcDirection::PlaceToTransition};
    for (auto &arc : arcs) {
        if (arc.from == edge.from && arc.to == edge.to && arc.direction == edge.direction) {
            return false;
        }
    }
    arcs.push_back(edge);
    return true;
}

/// @brief Adds an arc to the Petri net.
/// @param transition Transition that the arc originates from.
/// @param place Place that the arc goes to.
/// @return (Bool) True if the arc was added, false if the arc already exists.
bool PetriNet::addArc(const Transition &transition, const Place &place) {
    int placeIndex = placeMap[place.name];
    int transitionIndex = transitionMap[transition.name];
    Edge edge = {transitionIndex, placeIndex, ArcDirection::TransitionToPlace};
    for (auto &arc : arcs) {
        if (arc.from == edge.from && arc.to == edge.to && arc.direction == edge.direction) {
            return false;
        }
    }
    arcs.push_back(edge);
    return true;
}

const Places &PetriNet::getPlaces() const {
    return places;
}

void PetriNet::setPlaces(const Places &p) {
    PetriNet::places = p;
}

const Transitions &PetriNet::getTransitions() const {
    return transitions;
}

[[maybe_unused]] void PetriNet::setTransitions(const Transitions &t) {
    PetriNet::transitions = t;
}

const PlaceMap &PetriNet::getPlaceMap() const {
    return placeMap;
}

[[maybe_unused]] void PetriNet::setPlaceMap(const PlaceMap &p) {
    PetriNet::placeMap = p;
}

const TransitionMap &PetriNet::getTransitionMap() const {
    return transitionMap;
}

[[maybe_unused]] void PetriNet::setTransitionMap(const TransitionMap &t) {
    PetriNet::transitionMap = t;
}

const ReverseTransitionMap &PetriNet::getReverseTransitionMap() const {
    return reverseTransitionMap;
}

[[maybe_unused]] void PetriNet::setReverseTransitionMap(const ReverseTransitionMap &map) {
    PetriNet::reverseTransitionMap = map;
}

const Arcs &PetriNet::getArcs() const {
    return arcs;
}

[[maybe_unused]] void PetriNet::setArcs(const Arcs &a) {
    PetriNet::arcs = a;
}

void PetriNet::addToken(const int tokenId)  {
    PetriNet::places[tokenId].tokens++;
}

void PetriNet::consumeToken(const int tokenId) {
    PetriNet::places[tokenId].tokens--;
}
