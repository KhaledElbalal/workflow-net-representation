#ifndef PETRINETS_PETRINET_H
#define PETRINETS_PETRINET_H

#include "types.h"

class PetriNet {
public:
    PetriNet(); // Constructor declaration

    bool addPlace(Place &place);
    bool addTransition(const Transition &transition);
    bool addArc(const Place &place, const Transition &transition);
    bool addArc(const Transition &transition, const Place &place);

private:
    Places places;
public:
    [[nodiscard]] const Places &getPlaces() const;

    void setPlaces(const Places &places);

    [[nodiscard]] const Transitions &getTransitions() const;

    [[maybe_unused]] void setTransitions(const Transitions &transitions);

    [[nodiscard]] const PlaceMap &getPlaceMap() const;

    [[maybe_unused]] void setPlaceMap(const PlaceMap &placeMap);

    [[nodiscard]] const TransitionMap &getTransitionMap() const;

    [[maybe_unused]] void setTransitionMap(const TransitionMap &transitionMap);

    [[nodiscard]] const ReverseTransitionMap &getReverseTransitionMap() const;

    [[maybe_unused]] void setReverseTransitionMap(const ReverseTransitionMap &reverseTransitionMap);

    [[nodiscard]] const Arcs &getArcs() const;

    [[maybe_unused]] void setArcs(const Arcs &arcs);

    void consumeToken(int);

    void addToken(int);

private:
    Transitions transitions;
    PlaceMap placeMap;
    TransitionMap transitionMap;
    ReverseTransitionMap reverseTransitionMap;

    Arcs arcs;
};

#endif //PETRINETS_PETRINET_H