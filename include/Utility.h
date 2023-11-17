#ifndef PETRINETS_UTILITY_H
#define PETRINETS_UTILITY_H

#include "./Petrinet.h"

std::string convertToOrdinal(int num);
Places getEnabledPlaces(const PetriNet &net);
Transitions getEnabledTransitions(const PetriNet &net);
void fireTransition(PetriNet &net, const Transition &transition);
void unFireTransition(PetriNet &net, const Transition &transition);
bool isFinalState(const PetriNet &net, int target);
void writePetriNetToSVG(const PetriNet &net, const std::string &filename);
bool isWorkflowNet(const PetriNet &net);
bool getFinalSpace(PetriNet net, std::vector<std::pair<Places, Transitions>> &states, std::unordered_set<Places, PlacesHash> &statesSet, int initialTokens, Transitions firedTransitions, int depth);
bool allTransitionsEnabled(PetriNet net, TransitionSet &possibleTransitions, int initialTokens, int depth);
bool isSound(const PetriNet &net);
std::string markingToString(const Places &places);
void drawReachabilityGraphToSVG(const PetriNet &net, const std::string &filename);

extern Transitions latestFiredTransitions;

#endif //PETRINETS_UTILITY_H
